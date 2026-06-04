void setup() {
  // put your setup code here, to run once:
  serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int valeur_brute = analogRead(A0);  // recuperer la valeur sur A0
  float tension = (valeur_brute * (5.0 / 1023.0))*10; // valeur brute en volt qui vaut 1/10 de la valleur tire r1 9k r2 1k

  serial.print("Tension : ");
  serial.print(tension);
  serial.println(" V");
  delay(1000);
}
