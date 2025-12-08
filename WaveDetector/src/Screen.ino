#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();

#define BLACK 0x0000
#define WHITE 0xFFFF
#define SPOTIFY_GREEN 0x1EA9
#define GRAY 0x4208
#define BLK_PIN 13

bool handDetected = false;
bool isPlaying = false;
String currentSong = "No Song Playing";
String currentArtist = "";

unsigned long handRemovedTime = 0;
const int dimDelay = 3000;
bool backlightBright = false;

int scrollPosition = 0;
unsigned long lastScrollTime = 0;
const int scrollDelay = 100;
bool needsScrolling = false;

int animatingButton = -1;
unsigned long animationStartTime = 0;
const int animationDuration = 200;

void drawSpotifyLogo() {
  int centerX = 120, centerY = 100, radius = 40;
  tft.fillCircle(centerX, centerY, radius, SPOTIFY_GREEN);
  
  for (int i = 0; i < 3; i++) {
    int y = centerY - 5 + (i * 12);
    int arcWidth = 50 - (i * 8);
    
    for (int x = 0; x < arcWidth; x++) {
      int px = centerX - arcWidth/2 + x;
      int curvature = (x - arcWidth/2) * (x - arcWidth/2) / 80;
      tft.drawPixel(px, y + curvature, BLACK);
      tft.drawPixel(px, y + curvature + 1, BLACK);
      tft.drawPixel(px, y + curvature + 2, BLACK);
      tft.drawPixel(px, y + curvature + 3, BLACK);
      tft.drawPixel(px, y + curvature + 4, BLACK);
    }
  }
  
  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(2);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("Spotify", 120, 160);
}

void drawPreviousIcon(int x, int y, uint16_t color, float scale = 1.0) {
  int w = 14 * scale, h = 14 * scale, offsetY = 12 - (h / 2);
  tft.fillRect(x, y + offsetY, 2 * scale, h, color);
  tft.fillTriangle(x + 4 * scale, y + 12, x + 4 * scale + w, y + offsetY, x + 4 * scale + w, y + offsetY + h, color);
}

void drawPlayIcon(int x, int y, uint16_t color, float scale = 1.0) {
  int w = 13 * scale, h = 14 * scale, offsetY = 12 - (h / 2);
  tft.fillTriangle(x + 3 * scale, y + offsetY, x + 3 * scale, y + offsetY + h, x + 3 * scale + w, y + 12, color);
}

void drawPauseIcon(int x, int y, uint16_t color, float scale = 1.0) {
  int w = 3 * scale, h = 14 * scale, offsetY = 12 - (h / 2);
  tft.fillRect(x + 4 * scale, y + offsetY, w, h, color);
  tft.fillRect(x + 11 * scale, y + offsetY, w, h, color);
}

void drawNextIcon(int x, int y, uint16_t color, float scale = 1.0) {
  int w = 10 * scale, h = 14 * scale, offsetY = 12 - (h / 2);
  tft.fillTriangle(x, y + offsetY, x, y + offsetY + h, x + w, y + 12, color);
  tft.fillRect(x + 12 * scale, y + offsetY, 2 * scale, h, color);
}

void drawControlButton(int buttonIndex, bool highlight, float scale = 1.0) {
  int x, y;
  uint16_t color;
  
  switch(buttonIndex) {
    case 0:
      x = 45; y = 140;
      color = highlight ? SPOTIFY_GREEN : WHITE;
      drawPreviousIcon(x - (scale > 1.0 ? 2 : 0), y, color, scale);
      break;
    case 1:
      x = 105; y = 140;
      color = highlight ? WHITE : SPOTIFY_GREEN;
      if (isPlaying) drawPauseIcon(x - (scale > 1.0 ? 2 : 0), y, color, scale);
      else drawPlayIcon(x - (scale > 1.0 ? 2 : 0), y, color, scale);
      break;
    case 2:
      x = 165; y = 140;
      color = highlight ? SPOTIFY_GREEN : WHITE;
      drawNextIcon(x - (scale > 1.0 ? 2 : 0), y, color, scale);
      break;
  }
}

void showStartupScreen() {
  tft.fillScreen(BLACK);
  drawSpotifyLogo();
  delay(2000);
}

void drawMainInterface() {
  tft.fillScreen(BLACK);
  tft.fillCircle(220, 20, 8, handDetected ? SPOTIFY_GREEN : GRAY);
  drawControlButton(0, false);
  drawControlButton(1, false);
  drawControlButton(2, false);
  scrollPosition = 0;
  lastScrollTime = millis();
  updateSongDisplay();
}

void updateSongDisplay() {
  tft.fillRect(0, 40, 240, 70, BLACK);
  tft.setTextSize(2);
  int textWidth = tft.textWidth(currentSong);
  needsScrolling = (textWidth > 220);
  
  tft.setTextColor(WHITE, BLACK);
  tft.setTextSize(2);
  
  if (needsScrolling) {
    String displayText = currentSong + "   " + currentSong;
    tft.setTextDatum(TL_DATUM);
    tft.drawString(displayText, 10 - scrollPosition, 50);
  } else {
    tft.setTextDatum(MC_DATUM);
    tft.drawString(currentSong, 120, 60);
  }
  
  tft.setTextColor(GRAY, BLACK);
  tft.setTextSize(1);
  tft.setTextDatum(MC_DATUM);
  String artist = currentArtist.length() > 30 ? currentArtist.substring(0, 27) + "..." : currentArtist;
  tft.drawString(artist, 120, 90);
}

void updateHandIndicator() {
  tft.fillCircle(220, 20, 8, handDetected ? SPOTIFY_GREEN : GRAY);
  if (handDetected) {
    analogWrite(BLK_PIN, 255);
    backlightBright = true;
  } else if (backlightBright) {
    handRemovedTime = millis();
  }
}

void updateBacklightDimming() {
  if (!handDetected && backlightBright && millis() - handRemovedTime > dimDelay) {
    analogWrite(BLK_PIN, 179);
    backlightBright = false;
    Serial.println("💡 Display dimmed");
  }
}

void updateSongInfo(String song, String artist) {
  currentSong = song;
  currentArtist = artist;
  scrollPosition = 0;
  lastScrollTime = millis();
  updateSongDisplay();
}

void updatePlaybackState(bool playing) {
  isPlaying = playing;
  tft.fillRect(100, 135, 30, 30, BLACK);
  drawControlButton(1, false);
}

void animateButton(int buttonIndex) {
  animatingButton = buttonIndex;
  animationStartTime = millis();
}

void updateButtonAnimation() {
  if (animatingButton >= 0) {
    unsigned long elapsed = millis() - animationStartTime;
    
    if (elapsed < animationDuration) {
      float progress = (float)elapsed / animationDuration;
      float scale = 1.0 + (sin(progress * 3.14159) * 0.3);
      int clearX = (animatingButton == 0) ? 40 : (animatingButton == 1) ? 100 : 160;
      tft.fillRect(clearX, 135, 35, 30, BLACK);
      drawControlButton(animatingButton, animatingButton != 1, scale);
    } else {
      int clearX = (animatingButton == 0) ? 40 : (animatingButton == 1) ? 100 : 160;
      tft.fillRect(clearX, 135, 35, 30, BLACK);
      drawControlButton(animatingButton, false);
      animatingButton = -1;
    }
  }
}

void updateScrolling() {
  if (needsScrolling && millis() - lastScrollTime > scrollDelay) {
    scrollPosition += 2;
    if (scrollPosition > tft.textWidth(currentSong) + 20) scrollPosition = 0;
    lastScrollTime = millis();
    updateSongDisplay();
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(BLK_PIN, OUTPUT);
  analogWrite(BLK_PIN, 179);
  
  Serial.println("\nSpotify Display v2.0");
  
  tft.init();
  tft.setRotation(2);
  tft.fillScreen(WHITE);
  delay(1000);
  tft.fillScreen(BLACK);
  
  showStartupScreen();
  drawMainInterface();
  
  Serial.println("Ready");
  Serial.println("Commands: SONG:title|artist, PLAY, PAUSE, PREV, NEXT, HAND:0/1\n");
}

void loop() {
  updateButtonAnimation();
  updateScrolling();
  updateBacklightDimming();
  
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    
    if (cmd.startsWith("SONG:")) {
      int sep = cmd.indexOf('|', 5);
      if (sep > 0) {
        updateSongInfo(cmd.substring(5, sep), cmd.substring(sep + 1));
        Serial.println("♫ " + currentSong + " - " + currentArtist);
      }
    }
    else if (cmd == "PLAY") { updatePlaybackState(true); animateButton(1); Serial.println("▶ Playing"); }
    else if (cmd == "PAUSE") { updatePlaybackState(false); animateButton(1); Serial.println("⏸ Paused"); }
    else if (cmd == "PREV") { animateButton(0); Serial.println("⏮ Previous"); }
    else if (cmd == "NEXT") { animateButton(2); Serial.println("⏭ Next"); }
    else if (cmd == "HAND:1") { handDetected = true; updateHandIndicator(); Serial.println("👋 Hand"); }
    else if (cmd == "HAND:0") { handDetected = false; updateHandIndicator(); Serial.println("Hand removed"); }
    else if (cmd.length() > 0) Serial.println("❌ Unknown: " + cmd);
  }
  
  delay(10);
}