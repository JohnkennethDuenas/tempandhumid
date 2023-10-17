#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h> // Make sure to include this library

// Pin Configuration
#define DHTPIN D4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Wi-Fi Configuration
const char* ssid = "Anon2001"; // Replace with your Wi-Fi network name
const char* password = "123456789"; // Replace with your Wi-Fi password
const char* host = "https://climate-api-bsu.vercel.app/climate"; // Replace with your website's domain

// Variables for storing humidity and temperature values
int h;
int t;

// JSON configuration for temperature and humidity ranges
const char* jsonConfig = "{\"temperature\":{\"min\":15,\"max\":25},\"humidity\":{\"min\":30,\"max\":60}}";

// Setup function
void setup() {
    // Set pin modes for LEDs
    pinMode(D8, OUTPUT); // Blue LED
    pinMode(D0, OUTPUT); // Green LED
    pinMode(D1, OUTPUT); // Red LED

    // Initialize serial communication
    Serial.begin(9600);

    // Initialize the DHT sensor
    dht.begin();

    // Connect to Wi-Fi
    connectToWiFi();
}

// Loop function
void loop() {
    // Read humidity and temperature values from the sensor
    h = dht.readHumidity();
    t = dht.readTemperature();

    // Check if Wi-Fi is connected
  if (WiFi.status() == WL_CONNECTED) {
      // Send data to the web server
      HTTPClient http;
      WiFiClient client; // create a WiFiClient object
      String data = "temperature=" + String(t) + "&humidity=" + String(h);
      http.begin(client, "https://climate-api-bsu.vercel.app/climate"); // use the begin method with the client and URL
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      int httpResponseCode = http.POST(data);
      http.end();
      delay(10000); // Delay of 10 seconds before sending data again
  }

    // Parse the JSON configuration
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, jsonConfig);

    // Extract temperature and humidity ranges from JSON
    int minTemp = doc["temperature"]["min"];
    int maxTemp = doc["temperature"]["max"];
    int minHumidity = doc["humidity"]["min"];
    int maxHumidity = doc["humidity"]["max"];

    // Control LEDs based on temperature ranges
    if (t >= minTemp && t <= maxTemp) {
        digitalWrite(D0, HIGH); // Green LED behavior
    } else if (t < minTemp && t >= 0) {
        digitalWrite(D8, HIGH); // Blue LED behavior
    } else if (t > maxTemp && t <= 100) {
        digitalWrite(D1, HIGH); // Red LED behavior
    }
    delay(5000); // Delay for 5 seconds before turning off the LEDs
    digitalWrite(D1, LOW);
    digitalWrite(D8, LOW);
    digitalWrite(D0, LOW);
    delay(5000); // Delay for 5 seconds before starting the loop again
}

// Function to connect to Wi-Fi
void connectToWiFi() {
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}
