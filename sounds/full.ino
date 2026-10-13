
void song(int buzzerPin) {

  tone(buzzerPin, 349);
  delay(23);
  noTone(buzzerPin);

  tone(buzzerPin, 1319);
  delay(151);
  noTone(buzzerPin);

  tone(buzzerPin, 330);
  delay(1058);
  noTone(buzzerPin);

  tone(buzzerPin, 494);
  delay(965);
  noTone(buzzerPin);

  tone(buzzerPin, 1976);
  delay(116);
  noTone(buzzerPin);

}

void setup() {
  song(11);  // Change the pin number as needed
}

void loop() {
  // Empty loop
}
