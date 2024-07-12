// Nama: Sandhi Karunia Sugihartana
// NIM: 2107411007
// Kelas: TI-6A
// Mata Kuliah: Internet of Things
// Praktik Pilihan 'A'

#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6-2cViFoq"
#define BLYNK_TEMPLATE_NAME "UTS IoT 2107411007 Sandhi Karunia Sugihartana"
#define BLYNK_AUTH_TOKEN ""

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <DHT.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Blynk Auth Token
char auth[] = "";

// WiFi credentials
char ssid[] = "POCO M3 Pro 5G";
char pass[] = "61616161";

// DHT11
#define DHTPIN 19
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// MQ2
#define MQ2PIN 34

// LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Keypad
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 18, 5, 17, 16 };
byte colPins[COLS] = { 4, 0, 2, 15 };
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Variables
int gas;
int unit = 10;
float temperature;
float humidity;
String gasInformation;
String airInformation;
BlynkTimer blynkTimer;

void setup() {
  // Initialize serial monitor
  Serial.begin(115200);

  // Initialize DHT sensor
  dht.begin();

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Initialize WiFi and Blynk
  Blynk.begin(auth, ssid, pass);

  // Display initial message
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Press A/B/C");

  blynkTimer.setInterval(1000, interval);
}

void loop() {
  // Blynk
  Blynk.run();

  // Read sensor data
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  gas = analogRead(MQ2PIN);

  // Check keypad input
  char key = keypad.getKey();
  if (key) {
    switch (key) {
      case 'A': unit = 0; break;
      case 'B': unit = 1; break;
      case 'C': unit = 2; break;
    }
  }

  blynkTimer.run();
}

void interval() {
  if (gas >= 2600) {
    gasInformation = "Detected";
  } else {
    gasInformation = "Not Detected";
  }

  if (temperature > 36 || humidity > 80 || gas >= 2600) {
    airInformation = "Bad";
  } else {
    airInformation = "Good";
  }

  // Update LCD
  lcd.setCursor(0, 0);
  switch (unit) {
    case 0:
      lcd.clear();
      lcd.print("Gas:");
      if (gas >= 2600) {
        lcd.print("Detected");
      } else {
        lcd.print("Not Detected");
      }
      break;
    case 1:
      lcd.clear();
      lcd.print("Temp:");
      lcd.print(temperature);
      lcd.print("C");
      break;
    case 2:
      lcd.clear();
      lcd.print("Humidity:");
      lcd.print(humidity);
      lcd.print("%");
      break;
  }

  lcd.setCursor(0, 1);
  lcd.print("Air Quality:");
  if (temperature > 36 || humidity > 80 || gas >= 2600) {
    lcd.print("Bad");
  } else {
    lcd.print("Good");
  }

  // Update Blynk
  Blynk.virtualWrite(V5, humidity);
  Blynk.virtualWrite(V6, temperature);
  Blynk.virtualWrite(V7, gasInformation);
  Blynk.virtualWrite(V8, airInformation);
}