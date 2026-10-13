
void song(int buzzerPin) {

  tone(buzzerPin, 262);
  delay(116);
  noTone(buzzerPin);

  tone(buzzerPin, 659);
  delay(105);
  noTone(buzzerPin);

  tone(buzzerPin, 784);
  delay(105);
  noTone(buzzerPin);

  tone(buzzerPin, 932);
  delay(105);
  noTone(buzzerPin);

  tone(buzzerPin, 392);
  delay(93);
  noTone(buzzerPin);

  tone(buzzerPin, 1175);
  delay(81);
  noTone(buzzerPin);

  tone(buzzerPin, 1480);
  delay(81);
  noTone(buzzerPin);

  tone(buzzerPin, 1661);
  delay(81);
  noTone(buzzerPin);

  tone(buzzerPin, 131);
  delay(70);
  noTone(buzzerPin);

  tone(buzzerPin, 1976);
  delay(58);
  noTone(buzzerPin);

  delay(70);

  tone(buzzerPin, 262);
  delay(116);
  noTone(buzzerPin);

  tone(buzzerPin, 659);
  delay(105);
  noTone(buzzerPin);

  tone(buzzerPin, 784);
  delay(105);
  noTone(buzzerPin);

  tone(buzzerPin, 932);
  delay(93);
  noTone(buzzerPin);

  tone(buzzerPin, 392);
  delay(93);
  noTone(buzzerPin);

  tone(buzzerPin, 1175);
  delay(81);
  noTone(buzzerPin);

  tone(buzzerPin, 1480);
  delay(81);
  noTone(buzzerPin);

  tone(buzzerPin, 1661);
  delay(81);
  noTone(buzzerPin);

  tone(buzzerPin, 131);
  delay(70);
  noTone(buzzerPin);

  tone(buzzerPin, 1976);
  delay(58);
  noTone(buzzerPin);

}

void setup() {
  song(11);  // Change the pin number as needed
}

void loop() {
  // Empty loop
}
