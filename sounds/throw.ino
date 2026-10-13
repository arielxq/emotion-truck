
void song(int buzzerPin) {

  tone(buzzerPin, 698);
  delay(919);
  noTone(buzzerPin);

  tone(buzzerPin, 2794);
  delay(70);
  noTone(buzzerPin);

  delay(46);

  tone(buzzerPin, 880);
  delay(826);
  noTone(buzzerPin);

  tone(buzzerPin, 1397);
  delay(605);
  noTone(buzzerPin);

}

void setup() {
  song(11);  // Change the pin number as needed
}

void loop() {
  // Empty loop
}
