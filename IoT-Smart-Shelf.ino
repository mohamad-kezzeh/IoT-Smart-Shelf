// Include necessary libraries
#include <HX711.h>
#include <WiFi.h>
#include <DHT.h>
#include <HTTPClient.h>

// Define constants for DHT11
#define DHTPIN 4           // GPIO pin connected to the DHT11
#define DHTTYPE DHT11      // DHT11 sensor type
DHT dht(DHTPIN, DHTTYPE);

// Define HX711 pins
#define LOADCELL_DOUT_PIN 5 // HX711 data pin
#define LOADCELL_SCK_PIN 18 // HX711 clock pin
HX711 scale;

// WiFi credentials
const char* ssid = "SKYFAAJM";           
const char* password = "dzuk64g7jenm";   

// ThingSpeak settings
const char* server = "http://api.thingspeak.com/update";
String apiKey = "1LYX1M8Q04NFKJ9Q";      
void setup() {
  // Start Serial Monitor
  Serial.begin(115200);

  // Initialize DHT sensor
  dht.begin();

  // Initialize HX711
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale();    // Use calibration factor here (update after calibration)
  scale.tare();         // Reset the scale to zero

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

void loop() {
  // Read Temperature and Humidity
  float temperature = dht.readTemperature(); // in °C
  float humidity = dht.readHumidity();       // in %

  // Check if DHT data is valid
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  // Read Weight from Load Cell
  float weight = scale.get_units(5);         // Average over 5 readings

  // Print sensor data to Serial Monitor
  Serial.println("------ Sensor Data ------");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.print("Weight: ");
  Serial.print(weight);
  Serial.println(" kg");
  Serial.println("-------------------------");

  // Send data to ThingSpeak
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(server) + "?api_key=" + apiKey + "&field1=" + String(temperature) + "&field2=" + String(humidity) + "&field3=" + String(weight);
    http.begin(url);

    int httpCode = http.GET(); // Send HTTP GET request

    if (httpCode > 0) {
      Serial.print("Data sent successfully! HTTP code: ");
      Serial.println(httpCode);
    } else {
      Serial.print("Error sending data. HTTP code: ");
      Serial.println(httpCode);
    }
    http.end();
  } else {
    Serial.println("WiFi not connected. Retrying...");
  }

  // Delay 15 seconds between updates (as per ThingSpeak limit)
  delay(15000);
}
