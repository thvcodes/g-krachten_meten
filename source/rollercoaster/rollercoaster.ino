#include <WiFi.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// Access Point instellingen
const char* ssid = "ESP32_AccessPoint";
const char* password = "12345678"; // Minimaal 8 tekens

// MPU6050 sensor
Adafruit_MPU6050 mpu;

// Webserver poort
WiFiServer server(80);

// Variabelen voor het bijhouden van de max, min en gemiddelde waarden
float maxX = -1000.0, minX = 1000.0;
float maxY = -1000.0, minY = 1000.0;
float maxZ = -1000.0, minZ = 1000.0;
float sumX = 0, sumY = 0, sumZ = 0;
int count = 0;

//setup, dus doet hij 1 keer
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
        while (1);
    }

    // Stel het versnellingsbereik in op ±16g
    mpu.setAccelerometerRange(MPU6050_RANGE_16_G);  // Instellen op ±16g
}

//loop, dus doet het programma keer na keer door totdat het programma zelf stopt
void loop() {
    // Wacht op een client
    WiFiClient client = server.available();
    if (client) {
        String request = client.readStringUntil('\r');
        client.flush();

        // Controleer het aangevraagde pad
        if (request.indexOf("/getData") >= 0) {
            // Lees sensorgegevens
            sensors_event_t a, g, temp;
            mpu.getEvent(&a, &g, &temp);

            // Bereken g-krachten
            float ax_g = a.acceleration.x / 9.81;
            float ay_g = a.acceleration.y / 9.81;
            float az_g = a.acceleration.z / 9.81;

            // Update de maximale en minimale waarden
            maxX = max(maxX, ax_g);
            minX = min(minX, ax_g);

            maxY = max(maxY, ay_g);
            minY = min(minY, ay_g);

            maxZ = max(maxZ, az_g);
            minZ = min(minZ, az_g);

            // Bereken de gemiddelde waarden
            sumX += ax_g;
            sumY += ay_g;
            sumZ += az_g;
            count++;

            float avgX = sumX / count;
            float avgY = sumY / count;
            float avgZ = sumZ / count;

            // Verstuur de data in JSON-formaat
            String json = "{";
            json += "\"x\":" + String(ax_g, 2) + ",";
            json += "\"y\":" + String(ay_g, 2) + ",";
            json += "\"z\":" + String(az_g, 2) + ",";
            json += "\"maxX\":" + String(maxX, 2) + ",";
            json += "\"minX\":" + String(minX, 2) + ",";
            json += "\"maxY\":" + String(maxY, 2) + ",";
            json += "\"minY\":" + String(minY, 2) + ",";
            json += "\"maxZ\":" + String(maxZ, 2) + ",";
            json += "\"minZ\":" + String(minZ, 2) + ",";
            json += "\"avgX\":" + String(avgX, 2) + ",";
            json += "\"avgY\":" + String(avgY, 2) + ",";
            json += "\"avgZ\":" + String(avgZ, 2);
            json += "}";

            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: application/json");
            client.println("Connection: close");
            client.println();
            client.println(json);
        } else {
            // HTML-pagina versturen
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<head>");
            client.println("<title>ESP32 G-Krachten</title>");
            client.println("<script>");
            client.println("function fetchData() {");
            client.println("  fetch('/getData').then(response => response.json()).then(data => {");
            client.println("    document.getElementById('xValue').innerText = data.x + ' g';");
            client.println("    document.getElementById('yValue').innerText = data.y + ' g';");
            client.println("    document.getElementById('zValue').innerText = data.z + ' g';");
            client.println("    document.getElementById('maxX').innerText = data.maxX + ' g';");
            client.println("    document.getElementById('minX').innerText = data.minX + ' g';");
            client.println("    document.getElementById('maxY').innerText = data.maxY + ' g';");
            client.println("    document.getElementById('minY').innerText = data.minY + ' g';");
            client.println("    document.getElementById('maxZ').innerText = data.maxZ + ' g';");
            client.println("    document.getElementById('minZ').innerText = data.minZ + ' g';");
            client.println("    document.getElementById('avgX').innerText = data.avgX + ' g';");
            client.println("    document.getElementById('avgY').innerText = data.avgY + ' g';");
            client.println("    document.getElementById('avgZ').innerText = data.avgZ + ' g';");
            client.println("  });");
            client.println("}");
            client.println("setInterval(fetchData, 500);"); // Elke 500 ms data ophalen
            client.println("</script>");
            client.println("</head>");
            client.println("<body>");
            client.println("<h1>Real-time G-Krachten</h1>");
            client.println("<p><b>Huidige G-krachten:</b></p>");
            client.println("<p>X: <span id='xValue'>Laden...</span></p>");
            client.println("<p>Y: <span id='yValue'>Laden...</span></p>");
            client.println("<p>Z: <span id='zValue'>Laden...</span></p>");
            client.println("<p><b>Max en Min G-krachten:</b></p>");
            client.println("<p>Max X: <span id='maxX'>Laden...</span> | Min X: <span id='minX'>Laden...</span></p>");
            client.println("<p>Max Y: <span id='maxY'>Laden...</span> | Min Y: <span id='minY'>Laden...</span></p>");
            client.println("<p>Max Z: <span id='maxZ'>Laden...</span> | Min Z: <span id='minZ'>Laden...</span></p>");
            client.println("<p><b>Gemiddelde G-krachten:</b></p>");
            client.println("<p>Gemiddelde X: <span id='avgX'>Laden...</span></p>");
            client.println("<p>Gemiddelde Y: <span id='avgY'>Laden...</span></p>");
            client.println("<p>Gemiddelde Z: <span id='avgZ'>Laden...</span></p>");
            client.println("</body>");
            client.println("</html>");
        }
        client.stop();
    }
}
