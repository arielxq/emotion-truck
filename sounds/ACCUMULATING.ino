
void song(int buzzerPin) {

  tone(buzzerPin, 784);
  delay(109);
  noTone(buzzerPin);

  tone(buzzerPin, 1175);
  delay(196);
  noTone(buzzerPin);

}

void setup() {
  song(11);  // Change the pin number as needed
}

void loop() {
  // Empty loop
}
