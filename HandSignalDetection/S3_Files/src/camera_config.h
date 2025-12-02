#pragma once
// ^ Prevents this header from being included more than once in a build.

#include "esp_camera.h"
// ^ Needed for framesize_t, PIXFORMAT_JPEG, etc.
#include <string.h>   // for memset()

// ============================
// CAMERA PIN MAPPING (BOARD-SPECIFIC)
// ============================
// All pins below come directly from the board diagram you posted.
// Only change these if you move to a different board or rewire things.

// Camera power-down pin. -1 means "not connected / not used".
#define CAM_PIN_PWDN    -1

// Camera hardware reset pin. -1 means "not connected / not used".
#define CAM_PIN_RESET   -1

// External clock pin that drives the OV2640 (CAM_XCLK in the diagram).
#define CAM_PIN_XCLK    15

// SCCB (I2C-like) data pin (CAM_SIOD in the diagram).
#define CAM_PIN_SIOD    4

// SCCB clock pin (CAM_SIOC in the diagram).
#define CAM_PIN_SIOC    5

// Vertical sync pin, high at the start of a frame (CAM_VSYNC).
#define CAM_PIN_VSYNC   6

// Horizontal reference pin, high during active line data (CAM_HREF).
#define CAM_PIN_HREF    7

// Pixel clock pin, toggles for every pixel (CAM_PCLK).
#define CAM_PIN_PCLK    13

// OV2640 data bus mapping: Y2..Y9 -> D0..D7

// D0 receives Y2 from the sensor (CAM_Y2 in the diagram).
#define CAM_PIN_D0      11

// D1 receives Y3 (CAM_Y3).
#define CAM_PIN_D1      9

// D2 receives Y4 (CAM_Y4).
#define CAM_PIN_D2      8

// D3 receives Y5 (CAM_Y5).
#define CAM_PIN_D3      10

// D4 receives Y6 (CAM_Y6).
#define CAM_PIN_D4      12

// D5 receives Y7 (CAM_Y7).
#define CAM_PIN_D5      18

// D6 receives Y8 (CAM_Y8).
#define CAM_PIN_D6      17

// D7 receives Y9 (CAM_Y9).
#define CAM_PIN_D7      16



// ============================
// CAMERA TUNING PARAMETERS
// ============================
// These are the values you will change often when tuning FPS / quality / memory.


// Frame size controls resolution and speed.
// Smaller frame size = less data = higher FPS and lower RAM usage.
// Common options:
//   FRAMESIZE_QQVGA  -> 160x120  (fastest, lowest resolution)
//   FRAMESIZE_QQVGA2 -> 128x160
//   FRAMESIZE_QVGA   -> 320x240  (good default)
//   FRAMESIZE_VGA    -> 640x480  (needs PSRAM for streaming)
// Change this to experiment with FPS vs quality.
static const framesize_t CAM_FRAME_SIZE = FRAMESIZE_QQVGA;


// JPEG quality: 0 is best quality / largest file, 63 is worst / smallest.
// Lower number = better image but more CPU and bandwidth.
// Typical useful range is 10–30.
static const int CAM_JPEG_QUALITY = 12;


// Number of frame buffers - use 1 for DRAM only
static const int CAM_FB_COUNT = 1;

// Use DRAM for frame buffers
static const bool CAM_USE_PSRAM = false;


// Delay between frames in the MJPEG stream loop, in milliseconds.
// Smaller number = higher FPS but more CPU and network usage.
// If you want to speed up the stream, reduce this value.
static const int CAM_STREAM_DELAY_MS = 30;



// ============================
// HELPER: Fills camera_config_t
// ============================
// Call this from main.cpp instead of manually writing all fields each time.

inline void fillCameraConfig(camera_config_t &config) {
    // Start with all zeros to avoid random garbage in unused fields.
    memset(&config, 0, sizeof(config));

    // Timer / channel for the camera clock (pretty standard).
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer   = LEDC_TIMER_0;

    // Data bus pins D0..D7.
    config.pin_d0 = CAM_PIN_D0;
    config.pin_d1 = CAM_PIN_D1;
    config.pin_d2 = CAM_PIN_D2;
    config.pin_d3 = CAM_PIN_D3;
    config.pin_d4 = CAM_PIN_D4;
    config.pin_d5 = CAM_PIN_D5;
    config.pin_d6 = CAM_PIN_D6;
    config.pin_d7 = CAM_PIN_D7;

    // Sync and clock pins.
    config.pin_xclk     = CAM_PIN_XCLK;
    config.pin_pclk     = CAM_PIN_PCLK;
    config.pin_vsync    = CAM_PIN_VSYNC;
    config.pin_href     = CAM_PIN_HREF;

    // SCCB (camera control bus) pins.
    config.pin_sccb_sda = CAM_PIN_SIOD;
    config.pin_sccb_scl = CAM_PIN_SIOC;

    // Power-down and reset (not used on this board).
    config.pin_pwdn     = CAM_PIN_PWDN;
    config.pin_reset    = CAM_PIN_RESET;

    // Clock frequency for XCLK. 20 MHz is standard for OV2640.
    config.xclk_freq_hz = 20000000;

    // Capture format. JPEG is best for streaming over HTTP.
    config.pixel_format = PIXFORMAT_JPEG;

    // Resolution from the constant above.
    config.frame_size   = CAM_FRAME_SIZE;

    // JPEG quality from the constant above.
    config.jpeg_quality = CAM_JPEG_QUALITY;

    // Frame buffer count from the constant above.
    config.fb_count     = CAM_FB_COUNT;

    // Where to store frame buffers: DRAM or PSRAM.
    if (CAM_USE_PSRAM) {
        // Use external PSRAM for frame buffers (requires PSRAM working and enabled).
        config.fb_location = CAMERA_FB_IN_PSRAM;
    } else {
        // Use internal DRAM for frame buffers (what you are using now).
        config.fb_location = CAMERA_FB_IN_DRAM;
    }

    // Grab mode: WHEN_EMPTY means grab a new frame only when previous buffer is released.
    // This is the safest and simplest mode for streaming.
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
}
