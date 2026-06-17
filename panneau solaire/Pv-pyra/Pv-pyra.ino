#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>

// --- Configuration WiFi ---
char ssid[] = "iPhone";        
char pass[] = "mathias974";     

// --- Configuration HiveMQ Cloud ---
const char broker[] = "4a82b76d33f6404ca54e83f408c06636.s1.eu.hivemq.cloud";
int        port     = 8883; // Port sécurisé SSL

const char mqtt_user[] = "acquisition-uno_wifi";     
const char mqtt_pass[] = "VWL37e2026"; 

// Topic pour envoyer les données
const char topicUnique[] = "Smacp/ac-données";

WiFiSSLClient wifiClient;
MqttClient mqttClient(wifiClient);

const float courantMax = 50.0;  // 50A max pour le JXK7
unsigned long previousMillis = 0;
const long interval = 5000;     // Envoi toutes les 5 secondes

void setup() {
  Serial.begin(9600);
  while (!Serial) { ; }

  connectWiFi();
  mqttClient.setId("ArduinoUnoWiFi"); 
  mqttClient.setUsernamePassword(mqtt_user, mqtt_pass);
  connectMQTT();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) { connectWiFi(); }
  if (!mqttClient.connected()) { connectMQTT(); }
  
  mqttClient.poll();

  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // --- 1. MESURE DE L'IRRADIANCE (Avec moyenne sur 10 mesures) ---
    int somme_I = 0;
    for (int i = 0; i < 10; i++) {
      somme_I += analogRead(A0);
      delay(2);
    }
    float valeurBrute_I = somme_I / 10.0; 
    
    
    float tension_mv = valeurBrute_I * (5000.0 / 1023.0); // mise en mv 
    float irradiance = tension_mv * 5.0; // 1 mv = 5w/m²
    
    // --- 2. MESURE DE LA TENSION (Pont diviseur ratio de 10 avec la vrais valeur) ---
    int somme_P = 0;
    for (int i = 0; i < 10; i++) {
      somme_P += analogRead(A1);
      delay(2);
    }
    float lecture_P = somme_P / 10.0 ;  
    float tension = (lecture_P * (5.0 / 1023.0)) * 10.0; 
    
    // --- 3. MESURE DU COURANT (Transmetteur JXK7 avec moyenne) ---
    int somme = 0;
    for (int i = 0; i < 10; i++) {
      somme += analogRead(A3);
      delay(2); 
    }
    float valeurBrute = somme / 10.0; 
    float tension_jxk7 = valeurBrute * (5.0 / 1023.0);    
    float courant = (tension_jxk7 / 5.0) * courantMax;   

    // --- AFFICHAGE SÉRIE ---
    Serial.print("Irradiance: "); Serial.print(irradiance, 1); Serial.println(" W/m²");
    Serial.print("Tension   : "); Serial.print(tension, 1);    Serial.println(" V");
    Serial.print("Courant   : "); Serial.print(courant, 2);    Serial.println(" A");

    // --- ENVOI EN FORMAT JSON ---
    unsigned long epochTime = WiFi.getTime(); 

    // Sécurité : Si l'heure n'est pas encore synchronisée, on n'envoie pas un timestamp à 0
    if (epochTime == 0) {
      Serial.println("En attente de la synchronisation de l'heure WiFi...");
      epochTime = millis(); // Solution de secours temporaire
    }

    String payload = "{\"irradiance\":" + String(irradiance, 1) + 
                     ",\"tension\":" + String(tension, 1) + 
                     ",\"courant\":" + String(courant, 2) + 
                     ",\"timestamp\":" + String(epochTime) + "}";

    mqttClient.beginMessage(topicUnique);
    mqttClient.print(payload);
    mqttClient.endMessage();

    Serial.println("Payload JSON envoyé à HiveMQ !\n-----------------------");
  }
}

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  Serial.print("Tentative de connexion au WiFi...");
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(2000);
  }
  Serial.println("\nConnecté au WiFi !");
}

void connectMQTT() {
  Serial.print("Tentative de connexion au broker HiveMQ Cloud...");
  while (!mqttClient.connect(broker, port)) {
    Serial.print("Échec. Code d'erreur = ");
    Serial.println(mqttClient.connectError());
    delay(5000);
  }
  Serial.println("\nConnecté à HiveMQ !");
}