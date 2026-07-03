#include <Arduino.h>
#include <WiFiS3.h>
#include <DHT.h>

// ---------------- WIFI ----------------
char ssid[] = "Telezer_J";
char pass[] = "Telezer12";

WiFiServer server(80);

// ---------------- PINS ----------------
#define PIR 3
#define LED 5
#define BUZZER 6
#define TRIG 9
#define ECHO 10
#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

long duration;

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);

  pinMode(PIR, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  dht.begin();

  Serial.println("Connecting WiFi...");

  int status = WL_IDLE_STATUS;

  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    delay(3000);
    Serial.println("Trying WiFi...");
  }

  Serial.println("WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

// ---------------- ULTRASONIC ----------------
int getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH);
  return duration * 0.034 / 2;
}

// ---------------- LOOP ----------------
void loop() {
  WiFiClient client = server.available();

  if (client) {
    client.readStringUntil('\r');
    client.flush();

    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    int dist = getDistance();
    int motion = digitalRead(PIR);

    bool alert = (motion || dist < 15 || temp > 35);

    digitalWrite(LED, alert);
    digitalWrite(BUZZER, alert);

    unsigned long t = millis() / 1000;

    // ---------------- HTML ----------------
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();

    client.println("<!DOCTYPE html><html>");

    client.println("<head>");
    client.println("<meta http-equiv='refresh' content='1'>");

    client.println("<style>");
    client.println("body{font-family:Arial;text-align:center;color:white;background:url('https://images.unsplash.com/photo-1501785888041-af3ef285b470') no-repeat center center fixed;background-size:cover;}");

    client.println("table{margin:auto;width:90%;border-collapse:collapse;background:rgba(0,0,0,0.7);}");
    client.println("th,td{padding:10px;border:1px solid white;text-align:center;font-size:14px;}");
    client.println("th{background:rgba(255,255,255,0.2);}");    

    client.println("</style>");

    client.println("</head>");

    client.println("<body>");

    client.println("<h1> Live IoT Monitoring Dashboard</h1>");

    client.println("<table>");

    client.println("<tr>");
    client.println("<th>Time (s)</th>");
    client.println("<th>Temp</th>");
    client.println("<th>Humidity</th>");
    client.println("<th>Distance</th>");
    client.println("<th>Motion</th>");
    client.println("<th>LED</th>");
    client.println("<th>Buzzer</th>");
    client.println("</tr>");

    // LOOP-LIKE MULTIPLE ROWS (SIMULATED HISTORY)
    for (int i = 0; i < 10; i++) {
      client.println("<tr>");

      client.println("<td>" + String(t - i) + "</td>");
      client.println("<td>" + String(temp) + "°C</td>");
      client.println("<td>" + String(hum) + "%</td>");
      client.println("<td>" + String(dist) + "cm</td>");
      client.println("<td>" + String(motion) + "</td>");
      client.println("<td>" + String(alert ? "ON" : "OFF") + "</td>");
      client.println("<td>" + String(alert ? "ON" : "OFF") + "</td>");

      client.println("</tr>");
    }

    client.println("</table>");

    client.println("</body></html>");

    client.stop();
  }
}