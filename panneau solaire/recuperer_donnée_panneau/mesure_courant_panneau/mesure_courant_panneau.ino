const int pinCapteur = A0;
const float courantMax = 50.0;  // 50A max

void setup() {
  Serial.begin(9600);
  Serial.println("=== Mesure de courant JXK-7 ===");
}

void loop() {
  // Moyenne sur 10 lectures pour stabiliser la mesure
  int somme = 0;
  for (int i = 0; i < 10; i++) {
    somme += analogRead(pinCapteur);
    delay(10);
  }
  int valeurBrute = somme / 10;

  // Conversion en tension (0-5V)
  float tension = valeurBrute * (5.0 / 1023.0);

  // Conversion en courant (0-50A)
  float courant = (tension / 5.0) * courantMax;

  // Affichage
  Serial.print("Courant: ");
  Serial.print(courant, 2);
  Serial.println(" A");

  delay(500);
}