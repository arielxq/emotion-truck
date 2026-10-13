
void song(int buzzerPin) {

  tone(buzzerPin, 523);
  delay(348);
  noTone(buzzerPin);

  tone(buzzerPin, 784);
  delay(467);
  noTone(buzzerPin);

  tone(buzzerPin, 659);
  delay(424);
  noTone(buzzerPin);

  tone(buzzerPin, 2349);
  delay(33);
  noTone(buzzerPin);

  tone(buzzerPin, 1047);
  delay(554);
  noTone(buzzerPin);

  tone(buzzerPin, 3136);
  delay(120);
  noTone(buzzerPin);

  tone(buzzerPin, 1319);
  delay(620);
  noTone(buzzerPin);

  tone(buzzerPin, 1568);
  delay(587);
  noTone(buzzerPin);

  tone(buzzerPin, 1976);
  delay(33);
  noTone(buzzerPin);

  tone(buzzerPin, 2093);
  delay(500);
  noTone(buzzerPin);

}

void setup() {
  song(11);  // Change the pin number as needed
}

void loop() {
  // Empty loop
}
