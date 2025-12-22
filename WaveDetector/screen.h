/*
 * Spotify Rotating Logo Display
 *
 * Simple ESP32 project displaying a continuously rotating Spotify logo
 * on a TFT display.
 *
 * Hardware:
 * - ESP32 Dev Board
 * - ST7789 240x240 TFT Display
 */

#include <TFT_eSPI.h>
#include <SPI.h>

// Display Colors (RGB565)
#define BLACK 0x0000
#define SPOTIFY_GREEN 0x1EA9

// Display object
TFT_eSPI tft = TFT_eSPI();

// Logo rotation state
float logoRotation = 0;
unsigned long lastRotationUpdate = 0;
const int rotationSpeed = 0; // ms between rotation updates

// ============================================================================
// DISPLAY FUNCTIONS
// ============================================================================

void drawSpotifyLogo(float rotation) {
  int centerX = 120, centerY = 120, radius = 80;

  tft.fillScreen(BLACK);

  // Draw circle
  tft.fillCircle(centerX, centerY, radius, SPOTIFY_GREEN);

  // Convert rotation to radians
  float angleRad = rotation * PI / 180.0;
  float cosA = cos(angleRad);
  float sinA = sin(angleRad);

  // Draw three curved lines (Spotify arcs)
  for (int i = 0; i < 3; i++) {
    int yOffset = -10 + (i * 24);
    int arcWidth = 100 - (i * 16);

    for (int x = 0; x < arcWidth; x++) {
      int localX = x - arcWidth/2;
      int localY = yOffset;
      int curvature = (localX * localX) / 160;
      localY += curvature;

      // Apply rotation
      int rotatedX = centerX + (localX * cosA - localY * sinA);
      int rotatedY = centerY + (localX * sinA + localY * cosA);

      // Draw thick line
      for (int thickness = 0; thickness < 8; thickness++) {
        int ty = rotatedY + thickness;
        if (rotatedX >= 0 && rotatedX < 240 && ty >= 0 && ty < 240) {
          tft.drawPixel(rotatedX, ty, BLACK);
        }
      }
    }
  }
}

void updateRotatingLogo() {
  if (millis() - lastRotationUpdate > rotationSpeed) {
    logoRotation += 2; // Rotate 2 degrees per update
    if (logoRotation >= 360) logoRotation -= 360;

    drawSpotifyLogo(logoRotation);

    lastRotationUpdate = millis();
  }
}

// ============================================================================
// SETUP
// ============================================================================

// void setup() {
//   Serial.begin(115200);
//   delay(1000);
//   Serial.println("\n=== Spotify Rotating Logo ===");

//   // Initialize TFT display
//   tft.init();
//   tft.setRotation(2);
//   tft.fillScreen(BLACK);
//   Serial.println("Display initialized");

//   // Draw initial logo
//   drawSpotifyLogo(0);

//   Serial.println("Ready\n");
// }

// ============================================================================
// MAIN LOOP
// ============================================================================

// void loop() {
//   // Update rotating logo
//   updateRotatingLogo();

//   delay(10);
// }