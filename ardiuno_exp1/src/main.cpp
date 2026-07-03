#include <Arduino.h>
#include <WiFiS3.h>

// WiFi credentials
char ssid[] = "Telezer_J";
char pass[] = "Telezer12";

WiFiServer server(80);

// Ultrasonic sensor pins
#define TRIG 9
#define ECHO 10

long duration;
int distance;

void setup() {
  Serial.begin(115200);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

Serial.println("Connecting to WiFi...");

int status = WL_IDLE_STATUS;
int attempts = 0;

while (status != WL_CONNECTED && attempts < 10) {
  Serial.print("Attempt: ");
  Serial.println(attempts + 1);

  status = WiFi.begin(ssid, pass);

  delay(3000);
  attempts++;
}

if (status == WL_CONNECTED) {
  Serial.println("WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
} else {
  Serial.println("WiFi FAILED - Check SSID/Password or 2.4GHz network");
}

  server.begin();
}

// Function to get distance
int getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH);

  distance = duration * 0.034 / 2;

  return distance;
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();

    // ---------------- API ----------------
    if (request.indexOf("/distance") != -1) {
      int d = getDistance();

      client.println("HTTP/1.1 200 OK");
      client.println("Content-type: application/json");
      client.println("Connection: close");
      client.println();

      client.print("{\"distance\":");
      client.print(d);
      client.println("}");

      client.stop();
      return;
    }

    // ---------------- WEB PAGE ----------------
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println();

    client.println("<!DOCTYPE html>");
    client.println("<html>");
    client.println("<head>");
    client.println("<title>Ultrasonic Dashboard</title>");

    // ---------------- CSS ----------------
    client.println("<style>");
    client.println("body { font-family: Arial; text-align:center; transition:0.5s; }");
    client.println(".safe { background:#b6fcb6; }");
    client.println(".mid { background:#fff3a6; }");
    client.println(".danger { background:#ffb3b3; }");
    client.println("table { margin:auto; border-collapse:collapse; width:60%; }");
    client.println("th,td { border:1px solid #333; padding:10px; }");
    client.println("th { background:#444; color:white; }");
    client.println("</style>");

    // ---------------- JS ----------------
    client.println("<script>");

    client.println("let rows = [];");

    client.println("function update(){");
    client.println("fetch('/distance')");
    client.println(".then(r=>r.json())");
    client.println(".then(data=>{");

    client.println("let now = new Date().toLocaleTimeString();");

    client.println("document.getElementById('d').innerHTML = data.distance + ' cm';");

    client.println("rows.unshift(`<tr><td>${now}</td><td>${data.distance}</td></tr>`);");
    client.println("if(rows.length > 10) rows.pop();");

    client.println("document.getElementById('table').innerHTML = rows.join('');");

    client.println("if(data.distance < 10){");
    client.println("document.body.className='danger';");
    client.println("} else if(data.distance < 30){");
    client.println("document.body.className='mid';");
    client.println("} else {");
    client.println("document.body.className='safe';");
    client.println("}");

    client.println("});");
    client.println("}");

    client.println("setInterval(update,1000);");

    client.println("</script>");

    client.println("</head>");

    // ---------------- BODY ----------------
    client.println("<body onload='update()'>");

    client.println("<h1>Ultrasonic Sensor Dashboard</h1>");
    client.println("<h2>Live Distance: <span id='d'>Loading...</span></h2>");

    client.println("<table>");
    client.println("<tr><th>Time</th><th>Distance (cm)</th></tr>");
    client.println("<tbody id='table'></tbody>");
    client.println("</table>");

    client.println("</body>");
    client.println("</html>");

    client.stop();
  }
}