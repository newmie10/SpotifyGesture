#include <Arduino.h>
#include <WiFi.h>
#include "esp_camera.h"
#include "camera_config.h"   // <== new header with pins + tuning
#include <string.h>
// 5-second delayed log
unsigned long startTime = 0;
bool logged = false;

// WiFi (edit these)
const char *ssid     = "NETGEAR69";
const char *password = "deeptrail850";

WiFiServer server(80);

// Initialize the camera using the helper from camera_config.h
bool initCamera() {
    camera_config_t config;
    fillCameraConfig(config);   // fills all fields based on the header settings

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed: 0x%x\n", err);
        return false;
    }

    Serial.println("Camera init OK");
    return true;
}

// Read "GET /path HTTP/1.1" line from client
String readRequestLine(WiFiClient &client) {
    String line;
    while (client.connected()) {
        if (!client.available()) continue;
        char c = client.read();
        if (c == '\r') continue;
        if (c == '\n') break;
        line += c;
    }
    return line;
}

// One still JPEG
void sendSnapshot(WiFiClient &client) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Snapshot: fb == nullptr");
        client.println("HTTP/1.1 500 Internal Server Error");
        client.println("Content-Type: text/plain");
        client.println("Connection: close");
        client.println();
        client.println("Camera capture failed");
        return;
    }

    Serial.printf("Snapshot size: %u bytes\n", fb->len);

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: image/jpeg");
    client.printf("Content-Length: %u\r\n", fb->len);
    client.println("Connection: close");
    client.println();
    client.write(fb->buf, fb->len);

    esp_camera_fb_return(fb);
}

// MJPEG stream
void sendStream(WiFiClient &client) {
    Serial.println("Start stream");

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: multipart/x-mixed-replace; boundary=frame");
    client.println("Connection: close");
    client.println();

    while (client.connected()) {
        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) {
            Serial.println("Stream: fb == nullptr");
            break;
        }

        client.println("--frame");
        client.println("Content-Type: image/jpeg");
        client.printf("Content-Length: %u\r\n\r\n", fb->len);
        client.write(fb->buf, fb->len);
        client.println();

        esp_camera_fb_return(fb);

        // Uses the tunable delay from the header.
        delay(CAM_STREAM_DELAY_MS);
    }

    Serial.println("Stream end");
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    startTime = millis();
    Serial.println("Booting...");

    if (!initCamera()) {
        Serial.println("Camera init failed, halting");
        while (true) delay(1000);
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.printf("Connecting to WiFi: %s\n", ssid);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("WiFi connected");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    server.begin();
    Serial.println("HTTP server started");
    Serial.println("Use http://<IP>/ for snapshot, http://<IP>/stream for video");
}

void loop() {
    // 5 second delayed log
    if (!logged && millis() - startTime >= 5000) {
        Serial.println(">>> 5 seconds since program start <<<");
        logged = true;
    }

    WiFiClient client = server.available();
    if (!client) {
        delay(5);
        return;
    }

    Serial.println("Client connected");

    // Wait up to 2 seconds for request data
    unsigned long t0 = millis();
    while (!client.available() && millis() - t0 < 2000) {
        delay(1);
    }
    if (!client.available()) {
        Serial.println("Client timeout");
        client.stop();
        return;
    }

    String reqLine = readRequestLine(client);
    Serial.print("Request: ");
    Serial.println(reqLine);

    if (reqLine.indexOf("GET /stream") >= 0) {
        sendStream(client);
    } else {
        sendSnapshot(client);
    }

    delay(10);
    client.stop();
    Serial.println("Client disconnected");
}
