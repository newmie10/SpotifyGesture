#include <Arduino.h>
#include <WiFi.h>
#include "esp_camera.h"
#include "camera_config.h"
#include <string.h>

// 5-second delayed log
unsigned long startTime = 0;
bool logged = false;

// Simple capture counter
int counter = 0;

// WiFi Access Point settings (ESP32 creates its own network)
const char *ap_ssid     = "ESP32-HandData";  // Network name you'll connect to
const char *ap_password = "collect123";      // Password (min 8 chars)

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

// Serve main HTML page: auto-refreshing snapshot + capture button + filename + counter
void sendIndexPage(WiFiClient &client) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println("<!DOCTYPE html>");
    client.println("<html>");
    client.println("<head>");
    client.println("<meta charset=\"utf-8\" />");
    client.println("<title>Hand Gesture Data Collection</title>");
    client.println("<style>");
    client.println("body { font-family: sans-serif; text-align: center; background:#111; color:#eee; }");
    client.println("img { max-width: 100%; height: auto; border: 2px solid #444; }");
    client.println("button { margin-top: 10px; padding: 10px 20px; font-size: 16px; }");
    client.println("input { padding: 8px; margin-top: 10px; font-size: 16px; }");
    client.println("</style>");
    client.println("<script>");
    client.println("function refreshImage(){");
    client.println("  const img = document.getElementById('cam');");
    client.println("  img.src = '/snapshot?ts=' + Date.now();");
    client.println("}");
    client.println("setInterval(refreshImage, 200);");  // ~5 fps
    client.println("window.onload = refreshImage;");
    client.println();
    client.println("function captureWithName(){");
    client.println("  const name = document.getElementById('fname').value || 'capture';");
    client.println("  window.location.href = '/capture?name=' + encodeURIComponent(name);");
    client.println("}");
    client.println("</script>");
    client.println("</head>");
    client.println("<body>");
    client.println("<h2>🤖 Hand Gesture Data Collection</h2>");
    
    client.println("<p style=\"background:#222;padding:10px;border-radius:5px;\">");
    client.println("<strong>Instructions:</strong><br/>");
    client.println("1. Enter gesture name (e.g., 'fist', 'palm', 'peace')<br/>");
    client.println("2. Position your hand<br/>");
    client.println("3. Click Capture - saves as gesture_0.jpg, gesture_1.jpg, etc.<br/>");
    client.println("4. Collect 200-300 images per gesture!");
    client.println("</p>");

    client.print("<p style=\"font-size:20px;color:#28a745;\">📸 Images captured: <strong>");
    client.print(counter);
    client.println("</strong></p>");

    client.println("<img id=\"cam\" src=\"/snapshot\" />");
    client.println("<br/><br/>");

    client.println("<input id=\"fname\" placeholder=\"e.g., fist, palm, peace\" style=\"width:250px;\" />");
    client.println("<br/>");
    client.println("<button onclick=\"captureWithName()\" style=\"background:#28a745;color:#fff;border:none;padding:12px 24px;font-size:18px;cursor:pointer;border-radius:5px;margin-top:10px;\">📸 Capture Image</button>");

    client.println("</body>");
    client.println("</html>");
}

// One still JPEG for live view
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

    // Serial.printf("Snapshot size: %u bytes\n", fb->len);

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: image/jpeg");
    client.printf("Content-Length: %u\r\n", fb->len);
    client.println("Connection: close");
    client.println();
    client.write(fb->buf, fb->len);

    esp_camera_fb_return(fb);
}

// Sanitize filename: keep only letters, digits, _ and -
String sanitizeFileName(const String &raw) {
    if (raw.length() == 0) return "capture";
    String out;
    for (size_t k = 0; k < raw.length(); k++) {
        char c = raw[k];
        if ((c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            c == '_' || c == '-') {
            out += c;
        }
    }
    if (out.length() == 0) return "capture";
    return out;
}

// One still JPEG, returned as a download with custom filename
void sendCapture(WiFiClient &client, const String &fileName) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Capture: fb == nullptr");
        client.println("HTTP/1.1 500 Internal Server Error");
        client.println("Content-Type: text/plain");
        client.println("Connection: close");
        client.println();
        client.println("Camera capture failed");
        return;
    }

    Serial.printf("Capture size: %u bytes\n", fb->len);

    // increment counter on each successful capture
    counter++;

    String safeName = sanitizeFileName(fileName);

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: image/jpeg");
    client.print("Content-Disposition: attachment; filename=\"");
    client.print(safeName);
    client.println(".jpg\"");
    client.printf("Content-Length: %u\r\n", fb->len);
    client.println("Connection: close");
    client.println();
    client.write(fb->buf, fb->len);

    esp_camera_fb_return(fb);
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

    // Start Access Point mode (ESP32 creates its own WiFi network)
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ap_ssid, ap_password);
    
    IPAddress IP = WiFi.softAPIP();
    
    Serial.println("\n================================");
    Serial.println("WiFi Access Point Started!");
    Serial.println("================================");
    Serial.printf("Network Name: %s\n", ap_ssid);
    Serial.printf("Password: %s\n", ap_password);
    Serial.print("IP Address: ");
    Serial.println(IP);
    Serial.println("\nSteps to connect:");
    Serial.println("1. Connect your phone/laptop to WiFi: ESP32-HandData");
    Serial.println("2. Enter password: collect123");
    Serial.print("3. Open browser to: http://");
    Serial.println(IP);
    Serial.println("================================\n");

    server.begin();
    Serial.println("HTTP server started - waiting for connections...");
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

    if (reqLine.indexOf("GET /snapshot") >= 0) {
        sendSnapshot(client);
    } else if (reqLine.indexOf("GET /capture") >= 0) {
        // default name
        String fileName = "capture";

        // parse ?name=...
        int namePos = reqLine.indexOf("name=");
        if (namePos >= 0) {
            int start = namePos + 5;
            int end = reqLine.indexOf(' ', start);
            if (end < 0) end = reqLine.length();
            fileName = reqLine.substring(start, end);
        }

        fileName += "_" + String(counter);  // append counter to filename

        sendCapture(client, fileName);
    } else {
        // default: serve HTML page with auto-refresh snapshot + capture button
        sendIndexPage(client);
    }

    delay(10);
    client.stop();
    Serial.println("Client disconnected");
}
