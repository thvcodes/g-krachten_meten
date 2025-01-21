#include <WiFi.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "index_html.h"

// Board: ESP32 Dev module

// Access Point instellingen
const char* ssid = "ESP32_AccessPoint";
const char* password = "12345678"; // Minimaal 8 tekens

// MPU6050 sensor
Adafruit_MPU6050 mpu;

// Webserver poort
WiFiServer server(80);

// Define a struct for the object
struct DataPoint {
  unsigned long timestamp; // millis() value
  float x;
  float y;
  float z;
};

// Define the size of the array
const int arraySize = 1600;
const int interval = 5;
int currentIndex = 0;

// Create an array of DataPoint objects
DataPoint dataArray[arraySize];
bool started = false;

// setup, dus doet hij 1 keer
void setup() {
    Serial.begin(115200);

    // Configureren als access point
    Serial.println("Instellen van ESP32 als Access Point...");
    WiFi.softAP(ssid, password);

    // IP-adres van het AP
    IPAddress IP = WiFi.softAPIP();
    Serial.print("Access Point gestart. IP-adres: ");
    Serial.println(IP);

    // Start webserver
    server.begin();

    // Initialiseer MPU6050
    if (!mpu.begin()) {
        Serial.println("Kan MPU6050 niet detecteren. Controleer verbindingen!");
        while (true);
    }

    // Stel het versnellingsbereik in op ±16g
    mpu.setAccelerometerRange(MPU6050_RANGE_16_G);  // Instellen op ±16g
}

//loop, dus doet het programma keer na keer door totdat het programma zelf stopt
void loop() {
  
  // Doe meting
  if(started) {
    recordDatapoints();
  }

  // Wacht op een client
  handleClient();
}

void handleClient(){
  WiFiClient client = server.available();

  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();

    // Controleer het aangevraagde pad
    if (request.indexOf("startRecording") >= 0) {
      started = true;
      // HTML-pagina versturen
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/plain");
      client.println("Connection: close");
      client.println();
      client.println("Started");
    }
    else if (request.indexOf("getData") >= 0) {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: application/json");
      client.println("Connection: close");
      client.println();
      
      // Verstuur de data in JSON-formaat
      client.println("{");
      client.println("\"datapoints\":[");
      client.println();
      for (int i = 0; i < arraySize; i++) {
        if (i > 0) {
          client.print(",");
        }
        client.println(" {\"millis\":" + String(dataArray[i].timestamp) + ", \"x\":" + String(dataArray[i].x, 2) + ", \"y\":" + String(dataArray[i].y, 2) + ", \"z\":" + String(dataArray[i].z, 2) + "}");
      }
      client.println("]}");
    } else {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println("Connection: close");
      client.println();
      client.print(s1);
    }
    client.stop();
  }
}

void recordDatapoints() {
  unsigned long startTimestamp = millis();

  for (int i = 0; i < arraySize; i++) {
    // Haal sensor meting
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // Bereken g-krachten
    float ax_g = a.acceleration.x / 9.81;
    float ay_g = a.acceleration.y / 9.81;
    float az_g = a.acceleration.z / 9.81;

    dataArray[i].timestamp = (i * interval);
    dataArray[i].x = ax_g;
    dataArray[i].y = ay_g;
    dataArray[i].z = az_g;

    // Wacht voor volgende meting
    unsigned long waitUntil = startTimestamp + ( (i+1) * interval);
    signed long waitFor = waitUntil - millis();

    Serial.print(String(i));
    Serial.print(", wait for: ");
    Serial.println(String(waitFor));

    if(waitFor>0) {
      delay(waitFor);
    }
  }

  started = false;
}