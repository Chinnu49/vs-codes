#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Telezer_J";
const char* password = "Telezer12";

WebServer server(80);

const int ledPin = 2; // built-in LED on many ESP32 boards

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 LED Control</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">

  <style>
    body {
      margin: 0;
      font-family: Arial, sans-serif;
      background: linear-gradient(135deg, #1e3c72, #2a5298);
      color: white;
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
    }

    .card {
      background: rgba(255, 255, 255, 0.1);
      backdrop-filter: blur(10px);
      padding: 30px;
      border-radius: 20px;
      text-align: center;
      box-shadow: 0 8px 20px rgba(0,0,0,0.3);
      width: 300px;
    }

    h2 {
      margin-bottom: 20px;
    }

    .btn {
      display: inline-block;
      width: 100px;
      margin: 10px;
      padding: 12px;
      font-size: 18px;
      border: none;
      border-radius: 12px;
      cursor: pointer;
      text-decoration: none;
      color: white;
      transition: 0.3s;
    }

    .on {
      background: #00c853;
      box-shadow: 0 4px 15px rgba(0,200,83,0.4);
    }

    .on:hover {
      background: #00e676;
      transform: scale(1.05);
    }

    .off {
      background: #d50000;
      box-shadow: 0 4px 15px rgba(213,0,0,0.4);
    }

    .off:hover {
      background: #ff1744;
      transform: scale(1.05);
    }

    .status {
      margin-top: 15px;
      font-size: 14px;
      opacity: 0.8;
    }
  </style>
</head>

<body>
  <div class="card">
    <h2> ESP32 LED Control</h2>

    <a href="/on" class="btn on">ON</a>
    <a href="/off" class="btn off">OFF</a>

    <div class="status">Control your LED over WiFi</div>
  </div>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

void handleOn() {
  digitalWrite(ledPin, HIGH);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleOff() {
  digitalWrite(ledPin, LOW);
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);

  server.begin();
}

void loop() {
  server.handleClient();
}