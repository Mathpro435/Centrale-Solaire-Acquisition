void setup() {
  Serial.begin(9600);
}

void loop() {
  float Lecture = analogRead(A0); // recuperation donne
  float mesure = Lecture * (400/1023.0); // mise a lechelle en mv
  float iradiance = mesure * 5; // 1 mv = 5 w/m² 
  Serial.print("Puissance ensoleillement =", iradiance , "w/m²");
  delay(5000);
}