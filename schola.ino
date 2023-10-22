#include <Arduino.h>
#include <WiFi.h>
#include <NewPing.h>

#define TRIGGER_PIN  27  // Broche Arduino connectée à la broche de déclenchement du capteur ultrasonique.
#define ECHO_PIN     26  // Broche Arduino connectée à la broche d'écho du capteur ultrasonique.
#define MAX_DISTANCE 400 // Distance maximale de détection en centimètres.

const char* ssid = "PMS";        // Nom du réseau WiFi
const char* password = "";     // Mot de passe du réseau WiFi
const char* ipAddress = "192.168.1.10";  // Adresse IP statique que vous souhaitez utiliser

WiFiServer server(80);
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  IPAddress staticIP;
  staticIP.fromString(ipAddress);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(staticIP, gateway, subnet);

  server.begin();
}

void loop() {
  WiFiClient client = server.available(); // Écoute des clients entrants

  if (client) {
    String request = client.readStringUntil('\r');
   
    if (request.indexOf("GET /getval") != -1) {
      // Gestion de la requête GET pour /getval
      int distance = sonar.ping_cm(); // Mesure de la distance en cm

      if (distance == 0) {
        // La mesure est hors de la plage du capteur, cela peut être dû à une absence d'objet détecté
        Serial.println("Hors de portée du capteur");
      } else {
        // La mesure est valide, envoi de la distance en cm
        Serial.print("Distance : ");
        Serial.print(distance);
        Serial.println(" cm");

        String jsonResponse = "{\"level\": " + String(distance) + "}";
        client.println("HTTP/1.1 200 OK");
        client.println("Content-type: application/json");
        client.println("Access-Control-Allow-Origin: *"); // Définir l'en-tête CORS
        client.println("Connection: close");
        client.println();
        client.print(jsonResponse);
      }
    }

    delay(1);
    client.stop();
  }
}
