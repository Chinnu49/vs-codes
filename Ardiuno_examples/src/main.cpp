#include <WiFiS3.h>

char ssid[] = "Telezer_J";
char pass[] = "Telezer12";

WiFiServer server(80);

int ledPin = 13;
bool ledState = false;

void setup() {
  Serial.begin(115200);
  delay(2000);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.println("Connecting to WiFi...");

  int status = WL_IDLE_STATUS;

  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    Serial.println("Trying WiFi...");
    delay(3000);
  }

  Serial.println("WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();

    Serial.println(request);

    // -----------------------
    // LED CONTROL
    // -----------------------
    if (request.indexOf("/LED=ON") != -1) {
      digitalWrite(ledPin, HIGH);
      ledState = true;
      Serial.println("LED ON");
    }

    if (request.indexOf("/LED=OFF") != -1) {
      digitalWrite(ledPin, LOW);
      ledState = false;
      Serial.println("LED OFF");
    }

    // -----------------------
    // JSON API (REAL TIME STATUS)
    // -----------------------
    if (request.indexOf("/status") != -1) {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-type: application/json");
      client.println("Connection: close");
      client.println();

      client.print("{\"led\":\"");
      client.print(ledState ? "ON" : "OFF");
      client.println("\"}");

      client.stop();
      return;
    }

    // -----------------------
    // WEB PAGE
    // -----------------------
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type: text/html");
    client.println("Connection: close");
    client.println();

    client.println("<!DOCTYPE html>");
    client.println("<html>");
    client.println("<head>");
    client.println("<title>UNO R4 Smart Control</title>");

    client.println("<style>");
    client.println("body { font-family: Arial; text-align:center; transition:0.5s; }");
    client.println(".on { background-color: #c8f7c5; }");
    client.println(".off { background-color: #f7c5c5; }");
    client.println("button { padding:15px; margin:10px; font-size:18px; cursor:pointer; }");
    client.println("</style>");

    client.println("<script>");

    client.println("function setLED(state){");
    client.println("fetch('/LED=' + state);");
    client.println("setTimeout(updateStatus, 200);");
    client.println("}");

    client.println("function updateStatus(){");
    client.println("fetch('/status')");
    client.println(".then(res => res.json())");
    client.println(".then(data => {");

    client.println("document.getElementById('status').innerHTML = data.led;");

    client.println("if(data.led == 'ON'){");
    client.println("document.body.className = 'on';");
    client.println("} else {");
    client.println("document.body.className = 'off';");
    client.println("}");

    client.println("});");
    client.println("}");

    client.println("setInterval(updateStatus, 2000);");

    client.println("</script>");

    client.println("</head>");

    client.println("<body onload='updateStatus()'>");

    client.println("<h1>UNO R4 WiFi Smart Control</h1>");
    client.println("<h2>LED Status: <span id='status'>Loading...</span></h2>");

    client.println("<button onclick=\"setLED('ON')\">TURN ON</button>");
    client.println("<button onclick=\"setLED('OFF')\">TURN OFF</button>");

    client.println("</body>");
    client.println("</html>");

    client.stop();
  }
}