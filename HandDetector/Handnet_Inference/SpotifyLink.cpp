#include <Arduino.h>
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

// Gesture control settings
unsigned long last_gesture_time = 0;
const unsigned long GESTURE_COOLDOWN_MS = 2000;  // 2 seconds between gestures
int last_gesture_class = -1;

int inference_count = 0;

// Class names (matches your training labels)
const char* class_names[] = {"Closed Hand", "None", "Open Hand"};
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
  
    if (mean > 0.3f) {
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

void sendSpotifyCommand(int gesture_class) {
  // Send simple serial command for Python script to read
  if (gesture_class == 0) {
    // Closed Hand -> Pause
    Serial.println("PAUSE");
  } else if (gesture_class == 2) {
    // Open Hand -> Play
    Serial.println("PLAY");
  }
  // Note: None (class 1) sends nothing
}

int runInference() {
  // Capture image
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    return -1;
  }
  
  // Preprocess
  bool preprocess_ok = preprocessImage(fb);
  esp_camera_fb_return(fb);
  
  if (!preprocess_ok) {
    return -1;
  }
  
  // Check if None
  if (is_None()) {
    return 1;  // None class
  }
  
  // Fill input tensor
  fillInputTensor();
  
  // Run inference
  uint32_t start_time = millis();
  TfLiteStatus status = interpreter->Invoke();
  uint32_t inference_time = millis() - start_time;
  
  if (status != kTfLiteOk) {
    return -1;
  }
  
  // Get results
  float scores[3];
  getOutputScores(scores);
  softmax(scores, num_classes);
  
  // Find best class
  int best_idx = 0;
  float best_prob = scores[0];
  for (int i = 1; i < num_classes; i++) {
    if (scores[i] > best_prob) {
      best_prob = scores[i];
      best_idx = i;
    }
  }
  
  // Print inference result (for debugging/monitoring)
  Serial.printf("[INFERENCE] %s (%.1f%%) | %d ms | [%.2f, %.2f, %.2f]\n", 
                class_names[best_idx], best_prob * 100, inference_time,
                scores[0], scores[1], scores[2]);
  
  inference_count++;
  
  return best_idx;
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n================================");
  Serial.println("🤖 Hand Gesture Spotify Control");
  Serial.println("================================\n");
  
  if (!initCamera()) {
    Serial.println("HALT: Camera failed");
    while(1) delay(1000);
  }
  
  if (!initTFLite()) {
    Serial.println("HALT: TensorFlow failed");
    while(1) delay(1000);
  }
  
  Serial.println("\n================================");
  Serial.println("✓ All Systems Ready!");
  Serial.println("================================");
  Serial.println("Serial commands:");
  Serial.println("  ✊ Closed Hand -> PAUSE");
  Serial.println("  ✋ Open Hand -> PLAY");
  Serial.println("================================\n");
  
  delay(1000);
}

void loop() {
  // Run inference
  int gesture_class = runInference();
  
  // Check cooldown period
  unsigned long current_time = millis();
  bool cooldown_active = (current_time - last_gesture_time) < GESTURE_COOLDOWN_MS;
  
  // Send command if:
  // 1. Valid gesture (not None and not error)
  // 2. Different from last gesture OR cooldown expired
  // 3. Not None class
  if (gesture_class >= 0 && gesture_class != 1) {  // Not error and not None
    if (!cooldown_active || gesture_class != last_gesture_class) {
      sendSpotifyCommand(gesture_class);
      last_gesture_time = current_time;
      last_gesture_class = gesture_class;
    } else {
      Serial.println("⏳ Cooldown active, skipping command");
    }
  }
  
  // Small delay before next check
  delay(500);
}
