// ===== PARAMETRES =====
const float RATIO = 10.0;           // Pont diviseur 9k + 1k
const float V_MAX_PANNEAU = 33.0;   // Tension max PV 33V
const float V_ARDUINO = 5.0;        // Standard Arduino

void setup() {
  Serial.begin(9600);
}

void loop() {
  // 1. MESURE DE LA TENSION
  int lecture = analogRead(A0);
  float voltsPin = lecture * (V_ARDUINO / 1023.0);
  float tension = voltsPin * RATIO;

  // 2. CALCUL DU COURANT I = U/R
  float courant = tension / 10000.0;

  // 3. CALCUL PUISSANCE P = U*I
  float puissance = tension * courant;

  // 4. ESTIMATION Rayonement solaire avec la Tension
  // On compare la tension actuelle à la tension max possible
  float soleil = (tension / V_MAX_PANNEAU) * 1000.0;

  // Sécurité pour ne pas dépasser 1000 si la tension est très haute
  if (soleil > 1000) soleil = 1000;

  // 5. AFFICHAGE
  Serial.print("Tension: "); Serial.print(tension);     Serial.print(" V | ");
  Serial.print("Soleil: ");  Serial.print(soleil, 0);   Serial.println(" W/m2");

  delay(1000);
}