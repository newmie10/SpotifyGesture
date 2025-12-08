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

// WiFi Access Point settings
const char *ap_ssid     = "ESP32-HandData";
const char *ap_password = "collect123";
WiFiServer server(80);

int inference_count = 0;

// Class names (matches your training labels: closed_hand, open_hand, two_hand)
const char* class_names[] = {"✊ Closed Hand", "None", "✋ Open Hand"};
const int num_classes = 3;

// TensorFlow globals
namespace {
  const tflite::Model* model = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  TfLiteTensor* input = nullptr;
  TfLiteTensor* output = nullptr;

  constexpr int kTensorArenaSize = 85 * 1024;
  alignas(16) uint8_t tensor_arena[kTensorArenaSize];
  
  constexpr int MODEL_W = 48;
  constexpr int MODEL_H = 48;
  float input_buffer[MODEL_W * MODEL_H];
}

// RGB buffer (allocated dynamically)
constexpr int RGB_SIZE = 160 * 120 * 3;
static uint8_t* rgb_buffer = nullptr;

// Cropped buffer (106x94x3)
constexpr int CROP_W = 106;
constexpr int CROP_H = 94;
static uint8_t* crop_buffer = nullptr;

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

bool initTFLite() {
  Serial.println("\n=== Initializing TensorFlow Lite ===");
  Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
  
  model = tflite::GetModel(g_model);
  if (!model) {
    Serial.println("✗ Failed to load model");
    return false;
  }

  static tflite::AllOpsResolver resolver;
  static tflite::MicroErrorReporter error_reporter;
  
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize, &error_reporter);
  interpreter = &static_interpreter;

  if (interpreter->AllocateTensors() != kTfLiteOk) {
    Serial.println("✗ AllocateTensors() failed");
    return false;
  }

  input = interpreter->input(0);
  output = interpreter->output(0);
  
  Serial.println("✓ TensorFlow ready");
  Serial.printf("Free heap after init: %d bytes\n", ESP.getFreeHeap());
  return true;
}

// Softmax function
void softmax(float* scores, int n) {
  float max_score = scores[0];
  for (int i = 1; i < n; i++) {
    if (scores[i] > max_score) max_score = scores[i];
  }
  
  float sum = 0.0f;
  for (int i = 0; i < n; i++) {
    scores[i] = expf(scores[i] - max_score);
    sum += scores[i];
  }
  
  for (int i = 0; i < n; i++) {
    scores[i] /= sum;
  }
}

// Preprocess with center crop matching Python training
bool preprocessImage(camera_fb_t *fb) {
  if (fb->format != PIXFORMAT_JPEG) {
    Serial.println("Expected JPEG format");
    return false;
  }
  
  // Allocate buffers on first use
  if (!rgb_buffer) {
    rgb_buffer = (uint8_t*)malloc(RGB_SIZE);
    if (!rgb_buffer) {
      Serial.printf("✗ Failed to allocate RGB buffer (heap: %d)\n", ESP.getFreeHeap());
      return false;
    }
    Serial.println("✓ RGB buffer allocated");
  }
  
  if (!crop_buffer) {
    crop_buffer = (uint8_t*)malloc(CROP_W * CROP_H * 3);
    if (!crop_buffer) {
      Serial.println("✗ Failed to allocate crop buffer");
      return false;
    }
    Serial.println("✓ Crop buffer allocated");
  }
  
  // Decode JPEG to RGB (160x120)
  if (!fmt2rgb888(fb->buf, fb->len, fb->format, rgb_buffer)) {
    Serial.println("JPEG decode failed");
    return false;
  }
  
  // Crop to center region (matches Python preprocessing)
  // For 160x120: crop to (27,13)-(133,107) = 106x94
  const int src_w = 160;
  const int src_h = 120;
  const int crop_left = 27;
  const int crop_top = 13;
  
  // Copy cropped region
  for (int y = 0; y < CROP_H; y++) {
    for (int x = 0; x < CROP_W; x++) {
      int src_idx = ((crop_top + y) * src_w + (crop_left + x)) * 3;
      int dst_idx = (y * CROP_W + x) * 3;
      crop_buffer[dst_idx + 0] = rgb_buffer[src_idx + 0];
      crop_buffer[dst_idx + 1] = rgb_buffer[src_idx + 1];
      crop_buffer[dst_idx + 2] = rgb_buffer[src_idx + 2];
    }
  }
  
  // Resize from 106x94 to 48x48 and convert to grayscale [0,1]
  for (int y = 0; y < MODEL_H; y++) {
    float srcY = (y * (float)CROP_H) / MODEL_H;
    int srcY_int = (int)srcY;
    srcY_int = min(srcY_int, CROP_H - 1);
    
    for (int x = 0; x < MODEL_W; x++) {
      float srcX = (x * (float)CROP_W) / MODEL_W;
      int srcX_int = (int)srcX;
      srcX_int = min(srcX_int, CROP_W - 1);
      
      int idx = (srcY_int * CROP_W + srcX_int) * 3;
      uint8_t r = crop_buffer[idx + 0];
      uint8_t g = crop_buffer[idx + 1];
      uint8_t b = crop_buffer[idx + 2];
      
      // Convert to grayscale [0,1]
      float gray = (r + g + b) / (3.0f * 255.0f);
      input_buffer[y * MODEL_W + x] = gray;
    }
  }
  
  return true;
}


 bool is_None() {
  
  float sum = 0.0f;
  float min_val = 1.0f;
  float max_val = 0.0f;
  
  // Pass 1: Calculate mean, min, max
  for (int i = 0; i < MODEL_W * MODEL_H; i++) {
    float pixel = input_buffer[i];
    sum += pixel;
    if (pixel < min_val) min_val = pixel;
    if (pixel > max_val) max_val = pixel;
  }
  
  float mean = sum / (MODEL_W * MODEL_H);
float contrast = max_val - min_val;

  if (mean > 0.15f) {
    return false;
  }

  Serial.print("Mean:");
  Serial.println(mean);

  return true;
}

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

void getOutputScores(float* scores) {
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

void sendIndexPage(WiFiClient &client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  
  client.println("<!DOCTYPE html><html><head>");
  client.println("<meta charset=\"utf-8\"/>");
  client.println("<title>🤖 HandNet Inference</title>");
  client.println("<style>");
  client.println("body{font-family:sans-serif;text-align:center;background:#111;color:#eee;margin:20px;}");
  client.println("img{max-width:100%;border:2px solid #444;margin:20px 0;}");
  client.println("button{padding:15px 30px;font-size:18px;margin:10px;cursor:pointer;background:#28a745;color:#fff;border:none;border-radius:5px;}");
  client.println("button:hover{background:#218838;}");
  client.println("#result{margin-top:20px;font-size:18px;padding:20px;background:#222;border-radius:10px;}");
  client.println(".prob-bar{margin:10px 0;padding:10px;background:#333;border-radius:5px;position:relative;overflow:hidden;}");
  client.println(".prob-fill{position:absolute;left:0;top:0;height:100%;background:#28a745;z-index:0;}");
  client.println(".prob-text{position:relative;z-index:1;}");
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
  client.println("  btn.textContent='🔄 Processing...';");
  client.println("  res.innerHTML='<p>Running inference on camera frame...</p>';");
  client.println("  try{");
  client.println("    const r=await fetch('/infer');");
  client.println("    const t=await r.text();");
  client.println("    res.innerHTML=t;");
  client.println("  }catch(e){");
  client.println("    res.innerHTML='<p style=\"color:#dc3545;\">Error: '+e+'</p>';");
  client.println("  }");
  client.println("  btn.disabled=false;");
  client.println("  btn.textContent='🚀 Run Inference';");
  client.println("}");
  client.println("</script>");
  client.println("</head><body>");
  client.println("<h1>🤖 Hand Gesture Recognition</h1>");
  client.println("<p style=\"color:#888;\">Live Camera (160x120 → cropped to 106x94 → resized to 48x48)</p>");
  client.println("<img id=\"cam\" src=\"/snapshot\"/>");
  client.println("<br/>");
  client.println("<button id=\"btn\" onclick=\"runInfer()\">🚀 Run Inference</button>");
  client.printf("<p style=\"color:#888;font-size:14px;\">Total inferences: %d</p>", inference_count);
  client.println("<div id=\"result\">");
  client.println("<p>👆 Click button to classify hand gesture</p>");
  client.println("<p style=\"font-size:14px;color:#888;\">Model recognizes: ✊ Closed Hand, None, ✋ Open Hand, </p>");
  client.println("</div>");
  client.println("</body></html>");
}

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

void sendInference(WiFiClient &client) {
  Serial.println("\n╔══════════════════════════════════════════╗");
  Serial.println("║      INFERENCE PROFILING STARTED         ║");
  Serial.println("╚══════════════════════════════════════════╝");
  
  uint32_t total_start = millis();
  uint32_t step_start, step_time;
  
  // Step 1: Camera Capture
  Serial.println("\n[1/6] 📷 Camera Capture");
  Serial.printf("      Free heap: %d bytes\n", ESP.getFreeHeap());
  step_start = millis();
  
  camera_fb_t *fb = esp_camera_fb_get();
  step_time = millis() - step_start;
  
  if (!fb) {
    Serial.println("      ❌ FAILED - No frame buffer");
    client.println("HTTP/1.1 500 Error");
    client.println("Content-Type: text/html");
    client.println();
    client.println("<div><p style=\"color:#dc3545;\">❌ Camera capture failed</p></div>");
    return;
  }
  
  Serial.printf("      ✓ Captured: %dx%d, %u bytes\n", fb->width, fb->height, fb->len);
  Serial.printf("      ⏱️  Time: %d ms\n", step_time);
  
  // Step 2: Image Preprocessing
  Serial.println("\n[2/6] 🖼️  Image Preprocessing");
  Serial.printf("      Format: %d (JPEG=%d)\n", fb->format, PIXFORMAT_JPEG);
  step_start = millis();
  
  bool preprocess_ok = preprocessImage(fb);
  step_time = millis() - step_start;
  
  esp_camera_fb_return(fb);
  
  if (!preprocess_ok) {
    Serial.println("      ❌ FAILED - Preprocessing error");
    client.println("HTTP/1.1 500 Error");
    client.println("Content-Type: text/html");
    client.println();
    client.println("<div><p style=\"color:#dc3545;\">❌ Preprocessing failed</p></div>");
    return;
  }
  
  Serial.printf("      ✓ JPEG decoded → RGB\n");
  Serial.printf("      ✓ Cropped: 160x120 → %dx%d\n", CROP_W, CROP_H);
  Serial.printf("      ✓ Resized: %dx%d → %dx%d grayscale\n", CROP_W, CROP_H, MODEL_W, MODEL_H);
  Serial.printf("      ✓ Sample pixels (0-3): %.3f, %.3f, %.3f, %.3f\n", 
                input_buffer[0], input_buffer[1], input_buffer[2], input_buffer[3]);
  Serial.printf("      ⏱️  Time: %d ms\n", step_time);
  
  // Step 2.5: Pre-Inference None Check
  Serial.println("\n[2.5/7] 🔍 Pre-Inference None Check");
  step_start = millis();
  
  if (is_None()) {
    step_time = millis() - step_start;
    uint32_t total_time = millis() - total_start;
    inference_count++;
    
    Serial.println("      ✓ Detected NONE class (skipping ML inference)");
    Serial.printf("      ⏱️  Time: %d ms\n", step_time);
    Serial.printf("\n⚡ Fast-path complete: %d ms total\n", total_time);
    
    // Send HTTP response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    
    client.println("<div>");
    client.printf("<h2 style=\"font-size:48px;margin:20px 0;\">%s</h2>", class_names[1]); // "None"
    client.printf("<p style=\"font-size:24px;color:#ffc107;\"><strong>Pre-check</strong> (100.0%% confident)</p>");
    client.printf("<p style=\"color:#888;font-size:14px;\">Inference #%d | %d ms (⚡ fast-path)</p>", inference_count, total_time);
    client.println("<hr style=\"border-color:#444;margin:20px 0;\"/>");
    client.println("<h3>Detection Method:</h3>");
    client.println("<p style=\"font-size:14px;color:#888;\">✓ Pixel-level preprocessing detected empty/background</p>");
    client.println("<p style=\"font-size:14px;color:#888;\">✓ Skipped expensive ML inference</p>");
    client.println("</div>");
    
    return;  // Exit early
  }
  
  step_time = millis() - step_start;
  Serial.println("      ✓ Hand likely present - proceeding with ML inference");
  Serial.printf("      ⏱️  Time: %d ms\n", step_time);
  
  // Step 3: Fill Input Tensor
  Serial.println("\n[3/7] 📥 Fill Input Tensor");
  Serial.printf("      Input type: %s\n", (input->type == kTfLiteFloat32) ? "Float32" : "Int8");
  Serial.printf("      Input dims: ");
  for (int i = 0; i < input->dims->size; i++) {
    Serial.printf("%d ", input->dims->data[i]);
  }
  Serial.println();
  step_start = millis();
  
  fillInputTensor();
  step_time = millis() - step_start;
  
  Serial.printf("      ✓ Tensor filled (%d values)\n", MODEL_W * MODEL_H);
  Serial.printf("      ⏱️  Time: %d ms\n", step_time);
  
  // Step 4: Run Inference
  Serial.println("\n[4/7] 🧠 Run Model Inference");
  Serial.printf("      Model size: %d bytes\n", g_model_len);
  Serial.printf("      Tensor arena: %d KB\n", kTensorArenaSize / 1024);
  step_start = millis();
  
  TfLiteStatus status = interpreter->Invoke();
  step_time = millis() - step_start;
  
  if (status != kTfLiteOk) {
    Serial.printf("      ❌ FAILED - Status: %d\n", status);
    client.println("HTTP/1.1 500 Error");
    client.println("Content-Type: text/html");
    client.println();
    client.println("<div><p style=\"color:#dc3545;\">❌ Inference failed</p></div>");
    return;
  }
  
  inference_count++;
  Serial.printf("      ✓ Inference successful\n");
  Serial.printf("      ⏱️  Time: %d ms\n", step_time);
  
  // Step 5: Process Output
  Serial.println("\n[5/7] 📊 Process Output");
  Serial.printf("      Output type: %s\n", (output->type == kTfLiteFloat32) ? "Float32" : "Int8");
  Serial.printf("      Num classes: %d\n", num_classes);
  step_start = millis();
  
  // Get raw scores and apply softmax
  float scores[3];
  getOutputScores(scores);
  
  Serial.printf("      Raw logits: %.3f, %.3f, %.3f\n", scores[0], scores[1], scores[2]);
  
  softmax(scores, num_classes);
  step_time = millis() - step_start;
  
  Serial.printf("      After softmax: %.3f, %.3f, %.3f\n", scores[0], scores[1], scores[2]);
  Serial.printf("      ⏱️  Time: %d ms\n", step_time);
  
  // Find best class
  int best_idx = 0;
  float best_prob = scores[0];
  for (int i = 1; i < num_classes; i++) {
    if (scores[i] > best_prob) {
      best_prob = scores[i];
      best_idx = i;
    }
  }
  
  uint32_t total_time = millis() - total_start;
  
  // Step 6: Results Summary
  Serial.println("\n[6/7] 🎯 Results Summary");
  Serial.println("      ╔═══════════════════════════════════════╗");
  Serial.printf("      ║ 🏆 Winner: %-25s║\n", class_names[best_idx]);
  Serial.printf("      ║ 📈 Confidence: %.1f%%                   ║\n", best_prob * 100);
  Serial.println("      ╠═══════════════════════════════════════╣");
  for (int i = 0; i < num_classes; i++) {
    char bar[21];
    int bar_len = (int)(scores[i] * 20);
    for (int j = 0; j < 20; j++) {
      bar[j] = (j < bar_len) ? '#' : '.';
    }
    bar[20] = '\0';
    Serial.printf("      ║ %s %s %.1f%% %s║\n", 
                  (i == best_idx) ? ">" : " ",
                  class_names[i],
                  scores[i] * 100,
                  bar);
  }
  Serial.println("      ╚═══════════════════════════════════════╝");
  
  Serial.println("\n⏱️  TIMING BREAKDOWN:");
  Serial.println("    ┌─────────────────────────────┬──────────┐");
  Serial.println("    │ Stage                       │ Time (ms)│");
  Serial.println("    ├─────────────────────────────┼──────────┤");
  Serial.printf("    │ Total Pipeline              │ %8d │\n", total_time);
  Serial.printf("    │ Inference Count             │ %8d │\n", inference_count);
  Serial.println("    └─────────────────────────────┴──────────┘");
  
  Serial.printf("\n💾 Memory: Free heap = %d bytes\n", ESP.getFreeHeap());
  Serial.println("\n╔══════════════════════════════════════════╗");
  Serial.println("║      INFERENCE PROFILING COMPLETE        ║");
  Serial.println("╚══════════════════════════════════════════╝\n");
  
  // Send HTML response (Step 7)
  Serial.println("[7/7] 📤 Sending HTTP Response...");
  step_start = millis();
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  
  client.println("<div>");
  client.printf("<h2 style=\"font-size:48px;margin:20px 0;\">%s</h2>", class_names[best_idx]);
  client.printf("<p style=\"font-size:24px;color:#28a745;\"><strong>%.1f%%</strong> confident</p>", best_prob * 100);
  client.printf("<p style=\"color:#888;font-size:14px;\">Inference #%d | %d ms</p>", inference_count, total_time);
  client.println("<hr style=\"border-color:#444;margin:20px 0;\"/>");
  client.println("<h3>All Probabilities:</h3>");
  
  for (int i = 0; i < num_classes; i++) {
    int pct = (int)(scores[i] * 100);
    client.printf("<div class=\"prob-bar\" style=\"background:%s;\">", (i == best_idx) ? "#28a745" : "#333");
    client.printf("<div class=\"prob-fill\" style=\"width:%d%%;\"></div>", pct);
    client.printf("<div class=\"prob-text\">%s: <strong>%.1f%%</strong></div>", class_names[i], scores[i] * 100);
    client.println("</div>");
  }
  
  client.println("</div>");
  
  step_time = millis() - step_start;
  Serial.printf("      ✓ Response sent\n");
  Serial.printf("      ⏱️  Time: %d ms\n\n", step_time);
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n================================");
  Serial.println("🤖 Hand Gesture Recognition");
  Serial.println("================================\n");
  
  if (!initCamera()) {
    Serial.println("HALT: Camera failed");
    while(1) delay(1000);
  }
  
  if (!initTFLite()) {
    Serial.println("HALT: TensorFlow failed");
    while(1) delay(1000);
  }
  
  // Start Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);
  
  IPAddress IP = WiFi.softAPIP();
  
  Serial.println("\n================================");
  Serial.println("✓ WiFi Access Point Started!");
  Serial.println("================================");
  Serial.printf("Network: %s\n", ap_ssid);
  Serial.printf("Password: %s\n", ap_password);
  Serial.print("URL: http://");
  Serial.println(IP);
  Serial.println("================================");
  Serial.println("\n📱 Connect your device to ESP32-HandData");
  Serial.println("🌐 Then open http://192.168.4.1 in browser\n");
  
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) {
    delay(10);
    return;
  }
  
  unsigned long timeout = millis();
  while (!client.available() && millis() - timeout < 1000) {
    delay(1);
  }
  
  if (!client.available()) {
    client.stop();
    return;
  }
  
  String req = readRequestLine(client);
  
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
