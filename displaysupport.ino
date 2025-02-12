#include <U8glib.h>  // Include U8glib library
#include <DHT11.h>   // Include DHT11 library

U8GLIB_PCD8544 u8g(13, 11, 10, 9, 8);
DHT11 dht11(2);  // DHT11 sensor connected to digital pin 2
int soilMoisturePin = A0;  // Analog pin for soil moisture sensor
int Variable1;

void setup(void)
{
  u8g.setFont(u8g_font_6x10);
  Serial.begin(9600);  // Initialize serial communication for debugging
}

void loop(void)
{
  Variable1++;

  if (Variable1 > 150)
  {
    Variable1 = 0;
  }

  char string[10];
  dtostrf(Variable1, 3, 0, string);

  int temperature = dht11.readTemperature();
  int humidity = dht11.readHumidity();
  
  // Read soil moisture level
  int soilMoisture = analogRead(soilMoisturePin);
  // Map the analog sensor value (0-1023) to a percentage (0-100)
  int soilMoisturePercentage = map(soilMoisture, 0, 1023, 0, 100);

  u8g.firstPage();
  do
  {
    u8g.drawStr(0, 7, "Climate Status");  // (x,y,"Text")
    u8g.drawStr(0, 20, "Temp: ");
    u8g.drawStr(40, 20, dtostrf(temperature, 4, 1, string));
    u8g.drawStr(80, 20, " C");
    u8g.drawStr(0, 32, "Humidity: ");
    u8g.drawStr(60, 32, dtostrf(humidity, 4, 1, string));
    u8g.drawStr(100, 32, " %");
    u8g.drawStr(0, 44, "Moisture: ");
    u8g.drawStr(70, 44, dtostrf(soilMoisturePercentage, 4, 1, string));
    u8g.drawStr(110, 44, " %");

    // Print temperature, humidity, and soil moisture to serial for debugging
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    Serial.print("Soil Moisture: ");
    Serial.print(soilMoisturePercentage);
    Serial.println(" %");

  } while (u8g.nextPage());

  delay(1000); // Adjust the delay according to your needs
}
