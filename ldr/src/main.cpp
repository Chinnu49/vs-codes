const int ldrPin = 34;

void setup() {
  Serial.begin(115200);
}

void loop() {
  int value = analogRead(ldrPin);

  Serial.print("LDR Value: ");
  Serial.println(value);

  delay(500);
}