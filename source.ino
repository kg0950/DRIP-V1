#define BLYNK_TEMPLATE_ID "TMPL3WsN8UStR"
#define BLYNK_TEMPLATE_NAME "DRIP"
#define BLYNK_AUTH_TOKEN "odgVssJE8071xYwOedbgsXs7p-PJPqdl"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

// DHT sensor pin on ESP8266
#define DHT_PIN D4  // Change this to the pin connected to your DHT sensor

// Digital pin for moisture sensor
#define MOISTURE_PIN A0

// Digital pins for relay control
#define RELAY_PIN_1 D5
#define RELAY_PIN_2 D6

// Blynk setup
char ssid[] = "Shubh's Galaxy A52";
char pass[] = "eidm7560";
char auth[] = BLYNK_AUTH_TOKEN;

// Interval for sending DHT data to Blynk (in milliseconds)
const long dhtInterval = 2000;

// Interval for sending data to Blynk (in milliseconds)
const long blynkInterval = 2000;

// Define DHT sensor type (DHT11, DHT21, DHT22)
#define DHT_TYPE DHT11

// Initialize DHT sensor
DHT dht(DHT_PIN, DHT_TYPE);

unsigned long previousMillisDHT = 0;
unsigned long previousMillisBlynk = 0;

// Relay states
int relayState1 = LOW;
int relayState2 = LOW;

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, pass);
  Blynk.begin(auth, ssid, pass);

  // Initialize DHT sensor
  dht.begin();

  // Initialize relay pins
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);

  // Initially turn off relays
  digitalWrite(RELAY_PIN_1, relayState1);
  digitalWrite(RELAY_PIN_2, relayState2);

  // Setup Blynk virtual pin handlers
  Blynk.virtualWrite(V4, relayState1);
  Blynk.virtualWrite(V5, relayState2);
}

void loop() {
  Blynk.run();

  // Read DHT sensor data
  unsigned long currentMillisDHT = millis();
  if (currentMillisDHT - previousMillisDHT >= dhtInterval) {
    previousMillisDHT = currentMillisDHT;
    sendDHTData();
  }

  // Read and send moisture sensor data to Blynk
  unsigned long currentMillisBlynk = millis();
  if (currentMillisBlynk - previousMillisBlynk >= blynkInterval) {
    previousMillisBlynk = currentMillisBlynk;
    sendMoistureData();
  }
}

void sendDHTData() {
  // Read DHT sensor data
  float temperature = dht.readTemperature(); // Read temperature as Celsius
  float humidity = dht.readHumidity(); // Read humidity

  // Check if any reads failed and exit early (to try again)
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Send DHT sensor data to Blynk
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
}

void sendMoistureData() {
  // Read moisture sensor data
  int moisture = analogRead(MOISTURE_PIN);

  // Adjust the mapping based on your current condition
  int soilMoisturePercentage = map(moisture, 0, 1023, 100, 0); // Invert the mapping

  // Print moisture sensor data to serial monitor
  Serial.print("Moisture: ");
  Serial.println(soilMoisturePercentage);

  // Send moisture sensor data to Blynk
  Blynk.virtualWrite(V2, soilMoisturePercentage);
}

BLYNK_WRITE(V4) {
  // Control relay 1
  relayState1 = param.asInt();
  digitalWrite(RELAY_PIN_1, relayState1);
}

BLYNK_WRITE(V5) {
  // Control relay 2
  relayState2 = param.asInt();
  digitalWrite(RELAY_PIN_2, relayState2);
}
