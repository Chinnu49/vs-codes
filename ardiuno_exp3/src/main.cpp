#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// -------------------------
// Pin Definitions
// -------------------------
#define DHTPIN 2
#define DHTTYPE DHT11
#define FLAME_PIN 3

// Create DHT object
DHT dht(DHTPIN, DHTTYPE);

// LCD Address (Try 0x3F if 0x27 doesn't work)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
    Serial.begin(9600);

    dht.begin();

    pinMode(FLAME_PIN, INPUT);

    // Initialize LCD
    lcd.init();
    lcd.backlight();

    lcd.setCursor(0, 0);
    lcd.print("Fire Monitoring");
    lcd.setCursor(0, 1);
    lcd.print("System Ready");
    delay(2000);

    lcd.clear();
}

void loop()
{
    // Read DHT11 values
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Read Flame Sensor
    int flameStatus = digitalRead(FLAME_PIN);

    // ==========================
    // Serial Monitor Output
    // ==========================
    Serial.println("--------------------------------");

    if (isnan(temperature) || isnan(humidity))
    {
        Serial.println("DHT11 Sensor Error");
    }
    else
    {
        Serial.print("Temperature : ");
        Serial.print(temperature);
        Serial.println(" C");

        Serial.print("Humidity    : ");
        Serial.print(humidity);
        Serial.println(" %");
    }

    if (flameStatus == LOW)
    {
        Serial.println("Flame Status: DETECTED");
    }
    else
    {
        Serial.println("Flame Status: SAFE");
    }

    // ==========================
    // LCD Display
    // ==========================
    lcd.clear();

    // First Row
    lcd.setCursor(0, 0);

    if (!isnan(temperature))
    {
        lcd.print("T:");
        lcd.print(temperature, 1);
        lcd.print((char)223);   // Degree symbol
        lcd.print("C ");
    }
    else
    {
        lcd.print("T:Err ");
    }

    lcd.print("H:");

    if (!isnan(humidity))
    {
        lcd.print(humidity, 0);
        lcd.print("%");
    }
    else
    {
        lcd.print("Err");
    }

    // Second Row
    lcd.setCursor(0, 1);

    if (flameStatus == LOW)
    {
        lcd.print("Flame: YES");
    }
    else
    {
        lcd.print("Flame: NO ");
    }

    delay(2000);
}