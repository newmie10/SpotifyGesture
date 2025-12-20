# ESP32-S3 Camera Configuration Guide
A complete reference for tuning performance, quality, and board-specific OV2640 mapping.

This guide explains how to tune the camera for:
- higher FPS
- lower latency
- better quality
- lower RAM usage
- PSRAM vs DRAM behavior
- different ESP32-S3 camera boards

Everything is controlled through a single header file you can edit without touching `main.cpp`.

---

# 1. Camera Configuration Header (`camera_config.h`)

This file centralizes:
- camera pin map  
- performance settings  
- JPEG settings  
- memory usage  
- stream timing  

Paste this into `src/camera_config.h`:

```cpp
#pragma once
#include "esp_camera.h"
#include <string.h>   // memset()

// ============================
// CAMERA PIN MAPPING (BOARD-SPECIFIC)
// ============================
// These pins match the ESP32-S3 N16R8 CAM board diagram.
// Only change these when using a different board.

// Power-down pin (unused on this board)
#define CAM_PIN_PWDN    -1

// Hardware reset pin (unused)
#define CAM_PIN_RESET   -1

// External clock for OV2640
#define CAM_PIN_XCLK    15

// SCCB (camera I2C) pins
#define CAM_PIN_SIOD    4
#define CAM_PIN_SIOC    5

// Frame sync
#define CAM_PIN_VSYNC   6
#define CAM_PIN_HREF    7
#define CAM_PIN_PCLK    13

// OV2640 data bus: Y2..Y9 mapped to D0..D7
#define CAM_PIN_D0      11   // Y2
#define CAM_PIN_D1      9    // Y3
#define CAM_PIN_D2      8    // Y4
#define CAM_PIN_D3      10   // Y5
#define CAM_PIN_D4      12   // Y6
#define CAM_PIN_D5      18   // Y7
#define CAM_PIN_D6      17   // Y8
#define CAM_PIN_D7      16   // Y9

// ============================
// CAMERA TUNING PARAMETERS
// ============================
// Adjust these to change FPS, resolution, RAM usage, etc.

// Resolution:
// Smaller = faster FPS and lower RAM usage.
// Options:
//   FRAMESIZE_QQVGA  (160x120) – fastest
//   FRAMESIZE_QQVGA2 (128x160)
//   FRAMESIZE_QVGA   (320x240) – good default
//   FRAMESIZE_VGA    (640x480) – requires PSRAM
static const framesize_t CAM_FRAME_SIZE = FRAMESIZE_QQVGA;

// JPEG quality (0 = best, 63 = worst).
// Lower = better quality but slower and more RAM.
static const int CAM_JPEG_QUALITY = 12;

// Number of frame buffers.
// 1 = least RAM, safest for DRAM-only setups.
// >1 needs PSRAM.
static const int CAM_FB_COUNT = 1;

// Enable PSRAM frame buffers.
// Set to true only if PSRAM is verified working.
static const bool CAM_USE_PSRAM = false;

// Delay between MJPEG frames in ms.
// Lower = higher FPS.
static const int CAM_STREAM_DELAY_MS = 30;

// ============================
// HELPER: Fill camera_config_t
// ============================
// Call this from main.cpp (simplifies camera setup)
inline void fillCameraConfig(camera_config_t &config) {
    memset(&config, 0, sizeof(config));

    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer   = LEDC_TIMER_0;

    config.pin_d0 = CAM_PIN_D0;
    config.pin_d1 = CAM_PIN_D1;
    config.pin_d2 = CAM_PIN_D2;
    config.pin_d3 = CAM_PIN_D3;
    config.pin_d4 = CAM_PIN_D4;
    config.pin_d5 = CAM_PIN_D5;
    config.pin_d6 = CAM_PIN_D6;
    config.pin_d7 = CAM_PIN_D7;

    config.pin_xclk     = CAM_PIN_XCLK;
    config.pin_pclk     = CAM_PIN_PCLK;
    config.pin_vsync    = CAM_PIN_VSYNC;
    config.pin_href     = CAM_PIN_HREF;
    config.pin_sccb_sda = CAM_PIN_SIOD;
    config.pin_sccb_scl = CAM_PIN_SIOC;

    config.pin_pwdn     = CAM_PIN_PWDN;
    config.pin_reset    = CAM_PIN_RESET;

    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    config.frame_size   = CAM_FRAME_SIZE;
    config.jpeg_quality = CAM_JPEG_QUALITY;
    config.fb_count     = CAM_FB_COUNT;

    config.fb_location  = CAM_USE_PSRAM ? CAMERA_FB_IN_PSRAM : CAMERA_FB_IN_DRAM;
    config.grab_mode    = CAMERA_GRAB_WHEN_EMPTY;
}
```

---

# 2. What Each Section Does

## Camera Pin Mapping
This board connects the OV2640 camera to specific ESP32-S3 GPIO pins.  
You almost never change these unless:

- you switch to a different ESP32-S3 CAM board  
- or wire up your own OV2640 module manually  

Different boards route the OV2640 pins differently, so these define *how* the ESP32 talks to the camera.

If your camera fails to initialize (`ESP_ERR_NOT_FOUND`), or you get `fb == nullptr`,
pins are the first thing to check.

---

## Camera Tuning Parameters

### `CAM_FRAME_SIZE`
Controls the resolution:

| Frame Size | Pixels     | Notes |
|------------|------------|----------------|
| FRAMESIZE_QQVGA | 160x120 | Fastest, smallest |
| FRAMESIZE_QQVGA2 | 128x160 | Good compromise |
| FRAMESIZE_QVGA | 320x240 | Default / balanced |
| FRAMESIZE_VGA | 640x480 | Needs PSRAM |

Use a smaller frame if you want:
- higher FPS
- faster streaming
- lower latency
- less RAM use

---

### `CAM_JPEG_QUALITY`
0 = best quality  
63 = worst quality  

Lower numbers give clearer images but slow down:
- JPEG compression  
- streaming  
- FPS  

Typical good values: **10–30**.

---

### `CAM_FB_COUNT`
Number of frame buffers the camera maintains.

- `1` buffer uses the least memory and is safest on DRAM-only boards.
- `2` or more buffers help smooth streaming but require PSRAM.

If you see `fb == nullptr` at higher resolutions, reduce to 1 buffer.

---

### `CAM_USE_PSRAM`
Set to:

```
true
```

only when:
- your board has PSRAM  
- PSRAM is working (verified by boot log)

Otherwise keep it `false` or the camera will fail.

---

### `CAM_STREAM_DELAY_MS`
Controls the delay between frames during streaming:

- Lower = more FPS  
- Higher = lower CPU usage and network usage  

Example:
```
delay(10);   // fast stream
delay(30);   // moderate stream (default)
delay(60);   // slow stream, low bandwidth
```

---

# 3. Helper Function: `fillCameraConfig()`

This function:

- zeroes the config (important)
- sets all pins for your board
- applies resolution, quality, buffering, PSRAM/DRAM mode
- ensures safe DMA grab mode

Call it from `main.cpp`:

```cpp
camera_config_t config;
fillCameraConfig(config);
esp_camera_init(&config);
```

This keeps `main.cpp` clean and makes camera tuning centralized.

---

# 4. How to Tune the Camera for Any Purpose

## Higher FPS
- reduce resolution  
- reduce stream delay  
- keep `CAM_FB_COUNT = 1`  
- increase JPEG quality number (worse quality = faster)

Recommended:
```
FRAMESIZE_QQVGA
CAM_STREAM_DELAY_MS = 10
CAM_JPEG_QUALITY = 20
```

---

## Better Image Quality
- lower JPEG quality (e.g., 10–12)
- increase frame size if PSRAM is enabled

---

## Reduce CPU Load / Bandwidth
- increase `CAM_STREAM_DELAY_MS`
- use `FRAMESIZE_QQVGA` or `FRAMESIZE_QQVGA2`
- increase JPEG quality number (worse quality = smaller files)

---

## Enable PSRAM (advanced)
Set:

```
CAM_USE_PSRAM = true
CAM_FB_COUNT = 2
CAM_FRAME_SIZE = FRAMESIZE_VGA
```

Requires:
- board with PSRAM
- PSRAM working (boot log shows PSRAM OK)

---

## Fixing Capture Failures (`fb == nullptr`)
Try in order:

1. Lower resolution (e.g., QQVGA)  
2. Set `CAM_FB_COUNT = 1`  
3. Set `CAM_USE_PSRAM = false`  
4. Check your pin mapping  
5. Increase stream delay  

---

## ESP32-S3 Camera Driver Quick Reference (from `esp_camera.h`)

This section summarizes the key camera APIs available after including:

```cpp
#include "esp_camera.h"
```

Use this as a quick guide to access frames, control the sensor, or tune the camera.

---

### Frame Buffer Structure

```cpp
typedef struct {
    uint8_t *buf;    // JPEG bytes
    size_t len;      // size in bytes
    size_t width;    // pixel width
    size_t height;   // pixel height
    pixformat_t format; // usually PIXFORMAT_JPEG
} camera_fb_t;
```

A frame buffer is what you receive when capturing an image.

---

### Initialization

```cpp
camera_config_t config;
fillCameraConfig(config);     // your custom helper
esp_camera_init(&config);
```

Must be called once before capturing frames.

---

### Capturing a Frame

```cpp
camera_fb_t *fb = esp_camera_fb_get();
```

Returns:
- pointer to a frame buffer  
- `nullptr` if capture failed (bad pins, no RAM, PSRAM issues, etc.)

After using the frame:

```cpp
esp_camera_fb_return(fb);
```

Always return the buffer so the driver can reuse it.

---

### Getting the Sensor Object

```cpp
sensor_t *s = esp_camera_sensor_get();
```

From here you can adjust sensor settings:

```cpp
s->set_framesize(s, FRAMESIZE_QVGA);
s->set_brightness(s, 1);     // -2 to +2
s->set_contrast(s, 1);
s->set_saturation(s, 0);
s->set_exposure_ctrl(s, 1);
s->set_agc_gain(s, 30);
s->set_gainceiling(s, GAINCEILING_32X);
s->set_whitebal(s, 1);
s->set_wb_mode(s, 0);        // auto
s->set_aec_value(s, value);  // exposure value
```

Useful for:
- low light tuning  
- color correction  
- adjusting sharpness and noise  

---

### Pixel Formats

```cpp
PIXFORMAT_JPEG
PIXFORMAT_RGB565
PIXFORMAT_YUV422
PIXFORMAT_GRAYSCALE
```

For streaming over HTTP, use:
```
PIXFORMAT_JPEG
```

---

### Frame Sizes (resolutions)

Common options:

```
FRAMESIZE_QQVGA   // 160x120, fastest
FRAMESIZE_QQVGA2  // 128x160
FRAMESIZE_QVGA    // 320x240, balanced
FRAMESIZE_VGA     // 640x480, PSRAM required
```

Changing resolution via sensor:

```cpp
s->set_framesize(s, FRAMESIZE_QQVGA);
```

---

### Deinitializing the Camera

```cpp
esp_camera_deinit();
```

Only needed when switching modes or shutting down the camera driver.

---

### Memory Notes

- DRAM-only mode:  
  - lower resolutions  
  - `fb_count = 1`  
  - `CAMERA_FB_IN_DRAM`

- PSRAM mode:  
  - higher resolutions  
  - multiple frame buffers  
  - `CAMERA_FB_IN_PSRAM`

---

### Common Failure Causes

**`ESP_ERR_NOT_FOUND`**
- SCCB pins wrong (SIOD/SIOC)
- OV2640 not detected

**`fb == nullptr`**
- frame buffer allocation failed
- wrong data pins
- XCLK pin wrong
- PSRAM failure
- resolution too high

---

### Minimal Working Capture Example

```cpp
camera_fb_t *fb = esp_camera_fb_get();
if (fb) {
    // use fb->buf and fb->len
    esp_camera_fb_return(fb);
}
```

---

This reference covers the core functionality of `esp_camera.h` that you will use when tuning performance, adjusting image quality, building ML pipelines, or modifying streaming behavior.

