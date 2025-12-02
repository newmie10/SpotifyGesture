#include <Arduino.h>
#include <WiFi.h>
#include "esp_camera.h"
#include "camera_config.h"
#include "img_converters.h"
#include <math.h>

#include <TensorFlowLite_ESP32.h>
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include "handnet_model_data.h"

// WiFi credentials
const char *ssid     = "NETGEAR69";
const char *password = "deeptrail850";
WiFiServer server(80);

int inference_count = 0;

// TensorFlow globals
namespace {
  const tflite::Model* model = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  TfLiteTensor* input = nullptr;
  TfLiteTensor* output = nullptr;

  constexpr int kTensorArenaSize = 85 * 1024;  // 85 KB (model needs ~82KB)
  alignas(16) uint8_t tensor_arena[kTensorArenaSize];
  
  constexpr int MODEL_W = 64;
  constexpr int MODEL_H = 64;
  float input_buffer[MODEL_W * MODEL_H];
}

// Initialize camera
bool initCamera() {
  camera_config_t config;
  fillCameraConfig(config);
  
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed: 0x%x\n", err);
    return false;
  }
  
  Serial.println("✓ Camera initialized");
  return true;
}

// Initialize TensorFlow
bool initTFLite() {
  Serial.println("\n=== Initializing TensorFlow Lite ===");
  Serial.printf("Free heap at start: %d bytes\n", ESP.getFreeHeap());
  
  model = tflite::GetModel(g_model);
  if (!model) {
    Serial.println("✗ Failed to load model");
    return false;
  }

  static tflite::AllOpsResolver resolver;
  static tflite::MicroErrorReporter error_reporter;
  
  Serial.printf("Free heap before interpreter: %d bytes\n", ESP.getFreeHeap());
  
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize, &error_reporter);
  interpreter = &static_interpreter;

  Serial.printf("Free heap after interpreter: %d bytes\n", ESP.getFreeHeap());

  if (interpreter->AllocateTensors() != kTfLiteOk) {
    Serial.println("✗ AllocateTensors() failed");
    return false;
  }

  Serial.printf("Free heap after tensor allocation: %d bytes\n", ESP.getFreeHeap());

  input = interpreter->input(0);
  output = interpreter->output(0);
  
  Serial.println("✓ TensorFlow ready");
  return true;
}

// RGB buffer (allocated dynamically during first inference)
constexpr int RGB_SIZE = 160 * 120 * 3;
static uint8_t* rgb_buffer = nullptr;

// Preprocess: Convert JPEG to 64x64 grayscale
bool preprocessImage(camera_fb_t *fb) {
  if (fb->format != PIXFORMAT_JPEG) {
    Serial.println("Expected JPEG format");
    return false;
  }
  
  // Allocate RGB buffer on first use
  if (!rgb_buffer) {
    Serial.printf("Allocating %d bytes for RGB buffer (heap: %d)...\n", RGB_SIZE, ESP.getFreeHeap());
    rgb_buffer = (uint8_t*)malloc(RGB_SIZE);
    if (!rgb_buffer) {
      Serial.printf("✗ Failed! Free heap: %d\n", ESP.getFreeHeap());
      return false;
    }
    Serial.printf("✓ RGB buffer allocated\n");
  }
  
  // Decode JPEG to RGB
  bool ok = fmt2rgb888(fb->buf, fb->len, fb->format, rgb_buffer);
  if (!ok) {
    Serial.println("JPEG decode failed");
    return false;
  }
  
  // Downsample to 64x64 grayscale
  for (int y = 0; y < MODEL_H; y++) {
    int srcY = (y * 120) / MODEL_H;
    for (int x = 0; x < MODEL_W; x++) {
      int srcX = (x * 160) / MODEL_W;
      int idx = (srcY * 160 + srcX) * 3;
      uint8_t r = rgb_buffer[idx];
      uint8_t g = rgb_buffer[idx + 1];
      uint8_t b = rgb_buffer[idx + 2];
      float gray = (r + g + b) / (3.0f * 255.0f);
      input_buffer[y * MODEL_W + x] = gray;
    }
  }
  
  return true;
}

// Fill model input tensor
void fillInputTensor() {
  if (input->type == kTfLiteFloat32) {
    memcpy(input->data.f, input_buffer, MODEL_W * MODEL_H * sizeof(float));
  } else if (input->type == kTfLiteInt8) {
    int8_t *dst = input->data.int8;
    float scale = input->params.scale;
    int zero_point = input->params.zero_point;
    
    for (int i = 0; i < MODEL_W * MODEL_H; i++) {
      int32_t q = lroundf(input_buffer[i] / scale) + zero_point;
      q = constrain(q, -128, 127);
      dst[i] = (int8_t)q;
    }
  }
}

// Get output scores
void getOutputScores(float* scores, int max_classes) {
  int num_classes = 1;
  for (int i = 0; i < output->dims->size; i++) {
    num_classes *= output->dims->data[i];
  }
  num_classes = min(num_classes, max_classes);
  
  if (output->type == kTfLiteFloat32) {
    for (int i = 0; i < num_classes; i++) {
      scores[i] = output->data.f[i];
    }
  } else if (output->type == kTfLiteInt8) {
    float scale = output->params.scale;
    int zero_point = output->params.zero_point;
    for (int i = 0; i < num_classes; i++) {
      scores[i] = (output->data.int8[i] - zero_point) * scale;
    }
  }
}

// Read HTTP request line
String readRequestLine(WiFiClient &client) {
  String line;
  unsigned long timeout = millis();
  while (client.connected() && millis() - timeout < 1000) {
    if (!client.available()) {
      delay(1);
      continue;
    }
    char c = client.read();
    if (c == '\n') break;
    if (c != '\r') line += c;
  }
  return line;
}

// Serve HTML page
void sendIndexPage(WiFiClient &client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  
  client.println("<!DOCTYPE html><html><head>");
  client.println("<meta charset=\"utf-8\"/>");
  client.println("<title>ESP32-S3 HandNet</title>");
  client.println("<style>");
  client.println("body{font-family:sans-serif;text-align:center;background:#111;color:#eee;margin:20px;}");
  client.println("img{max-width:100%;border:2px solid #444;margin:20px 0;}");
  client.println("button{padding:15px 30px;font-size:18px;margin:10px;cursor:pointer;background:#28a745;color:#fff;border:none;border-radius:5px;}");
  client.println("button:hover{background:#218838;}");
  client.println("#result{margin-top:20px;font-size:20px;padding:20px;background:#222;border-radius:5px;}");
  client.println(".class{margin:5px 0;padding:5px;background:#333;}");
  client.println(".best{background:#28a745;font-weight:bold;}");
  client.println("</style>");
  client.println("<script>");
  client.println("function refresh(){document.getElementById('cam').src='/snapshot?t='+Date.now();}");
  client.println("setInterval(refresh,300);");
  client.println("window.onload=refresh;");
  client.println("async function runInfer(){");
  client.println("  const btn=document.getElementById('btn');");
  client.println("  const res=document.getElementById('result');");
  client.println("  btn.disabled=true;");
  client.println("  btn.textContent='Running...';");
  client.println("  res.innerHTML='Running inference...';");
  client.println("  try{");
  client.println("    const r=await fetch('/infer');");
  client.println("    const t=await r.text();");
  client.println("    res.innerHTML=t;");
  client.println("  }catch(e){");
  client.println("    res.innerHTML='Error: '+e;");
  client.println("  }");
  client.println("  btn.disabled=false;");
  client.println("  btn.textContent='Run Inference';");
  client.println("}");
  client.println("</script>");
  client.println("</head><body>");
  client.println("<h1>ESP32-S3 HandNet</h1>");
  client.println("<p>Live Camera Feed (160x120)</p>");
  client.println("<img id=\"cam\" src=\"/snapshot\"/>");
  client.println("<br/>");
  client.println("<button id=\"btn\" onclick=\"runInfer()\">Run Inference</button>");
  client.printf("<p>Inferences run: %d</p>", inference_count);
  client.println("<div id=\"result\">Click button to run inference</div>");
  client.println("</body></html>");
}

// Send snapshot
void sendSnapshot(WiFiClient &client) {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    client.println("HTTP/1.1 500 Error");
    client.println();
    return;
  }
  
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: image/jpeg");
  client.printf("Content-Length: %u\r\n", fb->len);
  client.println("Connection: close");
  client.println();
  client.write(fb->buf, fb->len);
  
  esp_camera_fb_return(fb);
}

// Run inference and send results
void sendInference(WiFiClient &client) {
  Serial.println("\n=== Running Inference ===");
  
  // Capture
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    client.println("HTTP/1.1 500 Error");
    client.println("Content-Type: text/html");
    client.println();
    client.println("<div>Camera capture failed</div>");
    return;
  }
  
  Serial.printf("Captured: %dx%d, %u bytes\n", fb->width, fb->height, fb->len);
  
  // Preprocess
  if (!preprocessImage(fb)) {
    esp_camera_fb_return(fb);
    client.println("HTTP/1.1 500 Error");
    client.println("Content-Type: text/html");
    client.println();
    client.println("<div>Image preprocessing failed</div>");
    return;
  }
  esp_camera_fb_return(fb);
  
  // Fill input
  fillInputTensor();
  
  // Run inference
  uint32_t start = millis();
  TfLiteStatus status = interpreter->Invoke();
  uint32_t elapsed = millis() - start;
  
  if (status != kTfLiteOk) {
    client.println("HTTP/1.1 500 Error");
    client.println("Content-Type: text/html");
    client.println();
    client.println("<div>Inference failed</div>");
    return;
  }
  
  inference_count++;
  Serial.printf("Inference completed in %d ms\n", elapsed);
  
  // Get results
  float scores[16];
  getOutputScores(scores, 16);
  
  int num_classes = 1;
  for (int i = 0; i < output->dims->size; i++) {
    num_classes *= output->dims->data[i];
  }
  num_classes = min(num_classes, 16);
  
  int best_idx = 0;
  float best_score = scores[0];
  for (int i = 1; i < num_classes; i++) {
    if (scores[i] > best_score) {
      best_score = scores[i];
      best_idx = i;
    }
  }
  
  // Send HTML response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  
  client.println("<div>");
  client.printf("<h2>Inference #%d</h2>", inference_count);
  client.printf("<p><strong>Detected:</strong> Class %d</p>", best_idx);
  client.printf("<p><strong>Confidence:</strong> %.4f</p>", best_score);
  client.printf("<p><strong>Time:</strong> %d ms</p>", elapsed);
  client.println("<h3>All Classes:</h3>");
  
  for (int i = 0; i < num_classes; i++) {
    String cls = (i == best_idx) ? "best" : "";
    client.printf("<div class=\"class %s\">Class %d: %.4f", cls.c_str(), i, scores[i]);
    if (i == best_idx) client.print(" ← BEST");
    client.println("</div>");
  }
  
  client.println("</div>");
  
  // Print to serial
  Serial.printf("Best: Class %d = %.4f\n", best_idx, best_score);
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n================================");
  Serial.println("ESP32-S3 HandNet Web Interface");
  Serial.println("================================\n");
  
  if (!initCamera()) {
    Serial.println("HALT: Camera failed");
    while(1) delay(1000);
  }
  
  if (!initTFLite()) {
    Serial.println("HALT: TensorFlow failed");
    while(1) delay(1000);
  }
  
  // Connect WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.printf("Connecting to %s", ssid);
  
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 20) {
    delay(500);
    Serial.print(".");
    tries++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi connected");
    Serial.print("✓ IP: ");
    Serial.println(WiFi.localIP());
    
    server.begin();
    Serial.println("✓ Server started");
    Serial.print("\nOpen http://");
    Serial.print(WiFi.localIP());
    Serial.println("/ in your browser\n");
  } else {
    Serial.println("\n✗ WiFi failed - continuing without network");
  }
}

void loop() {
  WiFiClient client = server.available();
  if (!client) {
    delay(10);
    return;
  }
  
  // Wait for data
  unsigned long timeout = millis();
  while (!client.available() && millis() - timeout < 1000) {
    delay(1);
  }
  
  if (!client.available()) {
    client.stop();
    return;
  }
  
  String req = readRequestLine(client);
  Serial.printf("Request: %s\n", req.c_str());
  
  // Skip headers
  while (client.available()) {
    String line = readRequestLine(client);
    if (line.length() == 0) break;
  }
  
  // Route request
  if (req.indexOf("GET /snapshot") >= 0) {
    sendSnapshot(client);
  } else if (req.indexOf("GET /infer") >= 0) {
    sendInference(client);
  } else {
    sendIndexPage(client);
  }
  
  delay(1);
  client.stop();
}
