#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#include "sounds.h"

constexpr uint8_t JOYSTICK_X_PIN = 34;
constexpr uint8_t JOYSTICK_Y_PIN = 35;
constexpr uint8_t JOYSTICK_BUTTON_PIN = 32;
constexpr uint8_t BUZZER_PIN = 25;
constexpr uint8_t LED_PIN = 27;

constexpr uint8_t I2C_SDA_PIN = 21;
constexpr uint8_t I2C_SCL_PIN = 22;
constexpr uint8_t LCD_ADDRESS = 0x27;

constexpr uint16_t LED_PWM_FREQUENCY = 5000;
constexpr uint8_t LED_PWM_RESOLUTION = 8;
constexpr uint8_t MAX_EMOTION_BAGS = 5;
constexpr uint8_t DIRECTION_THRESHOLD = 3;
constexpr uint16_t BUTTON_DEBOUNCE_MS = 50;
constexpr uint16_t THROWING_HOLD_MS = 1000;
constexpr uint16_t RESPONSE_DISPLAY_MS = 1500;
constexpr uint16_t COLLECTED_DISPLAY_MS = 1000;
constexpr uint8_t DAILY_COLLECTION_LIMIT = 2;
constexpr float THROW_TARGET_DEGREES = 1080.0f;
constexpr float ROTATION_MIN_STEP_DEGREES = 2.0f;

constexpr uint16_t ADC_CENTER = 2048;
constexpr uint16_t ADC_DEADZONE = 650;

enum LcdFace : uint8_t {
  FACE_SLEEP = 0,
  FACE_SMILE = 1,
  FACE_ANGRY = 2,
  FACE_FULL = 3,
  FACE_SURPRISE = 4,
  FACE_HAPPY = 5,
  FACE_TRASH = 6,
  FACE_RELAX = 7,
};

uint8_t sleepFace[8] = {
    0b00000, 0b00100, 0b01010, 0b00000,
    0b11111, 0b00000, 0b00000, 0b00000};
uint8_t smileFace[8] = {
    0b00000, 0b01010, 0b01010, 0b00000,
    0b10001, 0b01110, 0b00000, 0b00000};
uint8_t angryFace[8] = {
    0b00000, 0b10001, 0b01010, 0b00000,
    0b11111, 0b10001, 0b00000, 0b00000};
uint8_t fullFace[8] = {
    0b00000, 0b01010, 0b01010, 0b00000,
    0b11111, 0b10101, 0b00000, 0b00000};
uint8_t surpriseFace[8] = {
    0b00000, 0b01110, 0b10001, 0b10001,
    0b01110, 0b00000, 0b00000, 0b00000};
uint8_t happyFace[8] = {
    0b00000, 0b01010, 0b01010, 0b00000,
    0b10001, 0b01110, 0b00000, 0b00000};
uint8_t trashBag[8] = {
    0b00000, 0b01110, 0b11111, 0b11111,
    0b11111, 0b01110, 0b00000, 0b00000};
uint8_t relaxFace[8] = {
    0b00000, 0b00000, 0b01010, 0b00000,
    0b11111, 0b00000, 0b00000, 0b00000};

LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);

enum class State {
  REST_CLOSED,
  IDLE,
  ACCUMULATING,
  FULL,
  APPROACHING,
  ARRIVED,
  THROW_READY,
  THROWING,
  RESPONDING_CAREFUL,
  RESPONDING_THANKS,
  RESPONDING_RECYCLE,
  COLLECTED,
  REST_DAILY_LIMIT,
};

enum class Direction {
  CENTER,
  UP,
  DOWN,
  LEFT,
  RIGHT,
};

enum class Emotion {
  NONE,
  GAN,  // LEFT
  KAO,  // RIGHT
  CAO,  // UP
};

enum class RotationDirection {
  NONE,
  CLOCKWISE,
  COUNTERCLOCKWISE,
};

enum class LedPattern {
  OFF,
  SOLID,
  BLINK,
  REPEAT,
};

struct LedAnimation {
  LedPattern pattern = LedPattern::OFF;
  uint16_t onMs = 0;
  uint16_t offMs = 0;
  uint8_t totalCycles = 0;  // 0 means repeat forever
  uint8_t completedCycles = 0;
  bool on = false;
  uint32_t changedAt = 0;
};

State currentState = State::REST_CLOSED;
Emotion selectedEmotion = Emotion::NONE;
Direction selectedDirection = Direction::CENTER;
Direction lastDirection = Direction::CENTER;
uint8_t directionCount = 0;
uint8_t emotionBagCount = 0;
uint8_t returnedBagCount = 0;
RotationDirection throwRotationDirection = RotationDirection::NONE;
float throwAccumulatedDegrees = 0.0f;
float lastThrowAngle = 0.0f;
bool throwAngleStarted = false;
uint8_t dailyCollectCount = 0;
uint32_t stateEnteredAt = 0;
LedAnimation ledAnimation;
String serialInput;

const char* stateName(State state) {
  switch (state) {
    case State::REST_CLOSED: return "REST_CLOSED";
    case State::IDLE: return "IDLE";
    case State::ACCUMULATING: return "ACCUMULATING";
    case State::FULL: return "FULL";
    case State::APPROACHING: return "APPROACHING";
    case State::ARRIVED: return "ARRIVED";
    case State::THROW_READY: return "THROW_READY";
    case State::THROWING: return "THROWING";
    case State::RESPONDING_CAREFUL: return "RESPONDING_CAREFUL";
    case State::RESPONDING_THANKS: return "RESPONDING_THANKS";
    case State::RESPONDING_RECYCLE: return "RESPONDING_RECYCLE";
    case State::COLLECTED: return "COLLECTED";
    case State::REST_DAILY_LIMIT: return "REST_DAILY_LIMIT";
    default: return "UNKNOWN";
  }
}

const char* emotionName(Emotion emotion) {
  switch (emotion) {
    case Emotion::GAN: return "GAN";
    case Emotion::KAO: return "KAO";
    case Emotion::CAO: return "CAO";
    default: return "NONE";
  }
}

const char* directionName(Direction direction) {
  switch (direction) {
    case Direction::UP: return "UP";
    case Direction::DOWN: return "DOWN";
    case Direction::LEFT: return "LEFT";
    case Direction::RIGHT: return "RIGHT";
    default: return "CENTER";
  }
}

void setLed(uint8_t brightness) {
  ledcWrite(LED_PIN, brightness);
}

void stopLedAnimation() {
  ledAnimation = {};
  setLed(0);
}

void setLedSolid(bool on) {
  ledAnimation = {};
  ledAnimation.pattern = on ? LedPattern::SOLID : LedPattern::OFF;
  ledAnimation.on = on;
  setLed(on ? 255 : 0);
}

void startLedBlink(uint16_t onMs, uint16_t offMs, uint8_t cycles = 0) {
  ledAnimation.pattern = cycles == 0 ? LedPattern::BLINK : LedPattern::REPEAT;
  ledAnimation.onMs = onMs;
  ledAnimation.offMs = offMs;
  ledAnimation.totalCycles = cycles;
  ledAnimation.completedCycles = 0;
  ledAnimation.on = true;
  ledAnimation.changedAt = millis();
  setLed(255);
}

void updateLedAnimation() {
  if (ledAnimation.pattern == LedPattern::OFF ||
      ledAnimation.pattern == LedPattern::SOLID) {
    return;
  }

  const uint32_t now = millis();
  const uint16_t interval = ledAnimation.on ? ledAnimation.onMs
                                            : ledAnimation.offMs;
  if (now - ledAnimation.changedAt < interval) {
    return;
  }

  ledAnimation.changedAt = now;
  ledAnimation.on = !ledAnimation.on;

  if (ledAnimation.on) {
    setLed(255);
  } else {
    setLed(0);
    if (ledAnimation.pattern == LedPattern::REPEAT) {
      ++ledAnimation.completedCycles;
      if (ledAnimation.completedCycles >= ledAnimation.totalCycles) {
        stopLedAnimation();
      }
    }
  }
}

void clearLcdLine(uint8_t row) {
  lcd.setCursor(0, row);
  lcd.print("                ");
}

void showLcd(const char* line1, const char* line2) {
  clearLcdLine(0);
  clearLcdLine(1);
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void showLcdFace(const char* line1, uint8_t face) {
  clearLcdLine(0);
  clearLcdLine(1);
  lcd.setCursor(0, 0);
  lcd.print(line1);
  // A single custom character at column 7 is centered on a 16-column LCD.
  lcd.setCursor(7, 1);
  lcd.write(face);
}

void resetDirectionSelection() {
  directionCount = 0;
  selectedEmotion = Emotion::NONE;
  selectedDirection = Direction::CENTER;
}

void resetThrowSelection() {
  throwRotationDirection = RotationDirection::NONE;
  throwAccumulatedDegrees = 0.0f;
  lastThrowAngle = 0.0f;
  throwAngleStarted = false;
}

void showThrowProgress() {
  char line1[17];
  const uint8_t rounds = static_cast<uint8_t>(throwAccumulatedDegrees / 360.0f);
  snprintf(line1, sizeof(line1), "Rotate %u/3",
           static_cast<unsigned>(rounds > 3 ? 3 : rounds));
  showLcdFace(line1, FACE_TRASH);
}

float readJoystickAngle() {
  const float x = static_cast<float>(analogRead(JOYSTICK_X_PIN)) - ADC_CENTER;
  const float y = ADC_CENTER - static_cast<float>(analogRead(JOYSTICK_Y_PIN));
  return atan2f(y, x) * 180.0f / PI;
}

void showIdle() {
  char line1[17];
  snprintf(line1, sizeof(line1), "Bags: %u/5", emotionBagCount);
  showLcdFace(line1, FACE_SMILE);
}

void showDirectionProgress(Direction direction) {
  char line1[17];
  snprintf(line1, sizeof(line1), "%s %u/3",
           directionName(direction), directionCount);
  showLcdFace(line1, FACE_ANGRY);
}

void showSelectedEmotion() {
  char line1[17];
  snprintf(line1, sizeof(line1), "%s - %s", emotionName(selectedEmotion),
           selectedEmotion == Emotion::GAN   ? "anger"
           : selectedEmotion == Emotion::KAO ? "annoyed"
                                             : "irritated");
  showLcdFace(line1, FACE_ANGRY);
}

Direction readDirection(int x, int y) {
  const int xOffset = x - static_cast<int>(ADC_CENTER);
  const int yOffset = y - static_cast<int>(ADC_CENTER);

  if (abs(yOffset) > ADC_DEADZONE && abs(yOffset) >= abs(xOffset)) {
    return yOffset < 0 ? Direction::UP : Direction::DOWN;
  }
  if (abs(xOffset) > ADC_DEADZONE) {
    return xOffset < 0 ? Direction::LEFT : Direction::RIGHT;
  }
  return Direction::CENTER;
}

void enterState(State newState) {
  currentState = newState;
  Serial.printf("=== State: %s | bags=%u/5 ===\n",
                stateName(currentState), emotionBagCount);

  switch (currentState) {
    case State::REST_CLOSED:
      resetDirectionSelection();
      resetThrowSelection();
      EmotionTruckSounds::stop();
      showLcdFace("Truck resting", FACE_SLEEP);
      stopLedAnimation();
      break;

    case State::IDLE:
      showIdle();
      stopLedAnimation();
      break;

    case State::ACCUMULATING:
      showSelectedEmotion();
      startLedBlink(100, 100);
      EmotionTruckSounds::playAccumulating();
      break;

    case State::FULL:
      resetDirectionSelection();
      showLcdFace("Emotion is full", FACE_FULL);
      startLedBlink(200, 200);
      EmotionTruckSounds::playFull();
      break;

    case State::APPROACHING:
      showLcdFace("Truck coming", FACE_SURPRISE);
      startLedBlink(300, 300);
      EmotionTruckSounds::startTruckMusic(true);
      break;

    case State::ARRIVED:
      EmotionTruckSounds::stop();
      resetThrowSelection();
      showLcdFace("Truck is here", FACE_HAPPY);
      setLedSolid(true);
      break;

    case State::THROW_READY:
      showLcdFace("Press to throw", FACE_TRASH);
      startLedBlink(100, 100, 2);
      EmotionTruckSounds::playThrow();
      break;

    case State::THROWING:
      showLcdFace("Throwing...", FACE_TRASH);
      startLedBlink(100, 100, 2);
      break;

    case State::RESPONDING_CAREFUL:
      emotionBagCount = emotionBagCount >= MAX_EMOTION_BAGS
                            ? emotionBagCount - MAX_EMOTION_BAGS
                            : 0;
      showLcdFace("Be careful", FACE_HAPPY);
      startLedBlink(100, 100, 2);
      EmotionTruckSounds::playWorkerCareful();
      break;

    case State::RESPONDING_THANKS:
      emotionBagCount = emotionBagCount >= MAX_EMOTION_BAGS
                            ? emotionBagCount - MAX_EMOTION_BAGS
                            : 0;
      showLcdFace("Thank you", FACE_HAPPY);
      startLedBlink(100, 100, 2);
      EmotionTruckSounds::playWorkerThanks();
      break;

    case State::RESPONDING_RECYCLE: {
      char line1[17];
      char line2[17];

      // 根據 emotionBagCount 產生隨機數
      if (emotionBagCount == 0) {
        returnedBagCount = 0;
      } else {
        returnedBagCount = static_cast<uint8_t>(random(1, emotionBagCount + 1));
      }

      // bags 變成退回的數量
      emotionBagCount = returnedBagCount;

      // LCD 顯示
      snprintf(line1, sizeof(line1), "Returned %u bags", returnedBagCount);
      snprintf(line2, sizeof(line2), "Keep: %u bags", returnedBagCount);
      showLcd(line1, line2);

      startLedBlink(200, 200, 5);
      EmotionTruckSounds::playWorkerRecycle();

      // 序列埠與 LCD 一致
      Serial.printf("Returned %u bags, keep=%u, bags=%u\n",
                    returnedBagCount, returnedBagCount, emotionBagCount);
      break;
    }

    case State::COLLECTED:
      showLcdFace("Emotion cleared", FACE_RELAX);
      setLedSolid(true);
      EmotionTruckSounds::startTruckMusic(false);
      break;

    case State::REST_DAILY_LIMIT:
      resetDirectionSelection();
      resetThrowSelection();
      EmotionTruckSounds::stop();
      showLcdFace("2 trips done", FACE_SLEEP);
      stopLedAnimation();
      break;
  }
  stateEnteredAt = millis();
}

void confirmEmotion() {
  if (selectedEmotion == Emotion::NONE || currentState != State::ACCUMULATING) {
    return;
  }

  if (emotionBagCount < MAX_EMOTION_BAGS) {
    ++emotionBagCount;
  }

  Serial.printf("Confirmed %s, bags=%u/5\n",
                emotionName(selectedEmotion), emotionBagCount);
  resetDirectionSelection();

  if (emotionBagCount >= MAX_EMOTION_BAGS) {
    enterState(State::FULL);
  } else {
    enterState(State::IDLE);
  }
}

void handleDirection(Direction direction) {
  if (currentState != State::IDLE && currentState != State::ACCUMULATING) {
    return;
  }

  if (direction == Direction::CENTER) {
    // A return to center arms the next push, including the same direction.
    lastDirection = Direction::CENTER;
    return;
  }

  if (direction == lastDirection) {
    return;
  }
  lastDirection = direction;

  if (direction == Direction::DOWN) {
    resetDirectionSelection();
    Serial.println("Selection cancelled.");
    enterState(State::IDLE);
    return;
  }

  // Switching direction starts a new 1/3 sequence. This avoids keeping
  // ambiguous simultaneous counters while still requiring three pushes.
  if (selectedDirection != Direction::CENTER &&
      direction != selectedDirection) {
    directionCount = 0;
    selectedEmotion = Emotion::NONE;
  }
  selectedDirection = direction;
  ++directionCount;

  if (directionCount >= DIRECTION_THRESHOLD) {
    selectedEmotion = direction == Direction::LEFT  ? Emotion::GAN
                     : direction == Direction::RIGHT ? Emotion::KAO
                                                      : Emotion::CAO;
    enterState(State::ACCUMULATING);
    Serial.printf("Emotion option ready: %s\n", emotionName(selectedEmotion));
  } else {
    currentState = State::ACCUMULATING;
    showDirectionProgress(direction);
    startLedBlink(100, 100);
    Serial.printf("%s %u/3\n", directionName(direction), directionCount);
  }
}

void handleThrowDirection(Direction direction, bool pressedEdge) {
  if (currentState == State::THROW_READY) {
    if (pressedEdge) {
      Serial.println("Throw confirmed -> THROWING");
      enterState(State::THROWING);
    }
    return;
  }

  if (currentState != State::ARRIVED || pressedEdge) {
    return;
  }

  if (direction == Direction::CENTER) {
    return;
  }

  const int x = analogRead(JOYSTICK_X_PIN);
  const int y = analogRead(JOYSTICK_Y_PIN);
  const int xOffset = x - static_cast<int>(ADC_CENTER);
  const int yOffset = y - static_cast<int>(ADC_CENTER);
  if (abs(xOffset) <= ADC_DEADZONE && abs(yOffset) <= ADC_DEADZONE) {
    return;
  }

  const float currentAngle = readJoystickAngle();
  if (!throwAngleStarted) {
    lastThrowAngle = currentAngle;
    throwAngleStarted = true;
    Serial.printf("Rotation started at angle %.1f\n", currentAngle);
    return;
  }

  float delta = currentAngle - lastThrowAngle;
  if (delta > 180.0f) delta -= 360.0f;
  if (delta < -180.0f) delta += 360.0f;
  if (fabsf(delta) < ROTATION_MIN_STEP_DEGREES) {
    return;
  }

  const RotationDirection stepDirection =
      delta > 0.0f ? RotationDirection::CLOCKWISE
                   : RotationDirection::COUNTERCLOCKWISE;
  if (throwRotationDirection == RotationDirection::NONE) {
    throwRotationDirection = stepDirection;
  } else if (throwRotationDirection != stepDirection) {
    Serial.println("Rotation reversed; rotation progress reset.");
    resetThrowSelection();
    return;
  }

  throwAccumulatedDegrees += fabsf(delta);
  lastThrowAngle = currentAngle;
  showThrowProgress();
  Serial.printf("Rotation %.1f/1080 degrees (%s)\n", throwAccumulatedDegrees,
                throwRotationDirection == RotationDirection::CLOCKWISE
                    ? "CW"
                    : "CCW");

  if (throwAccumulatedDegrees >= THROW_TARGET_DEGREES) {
    Serial.println("Three rounds complete -> THROW_READY");
    enterState(State::THROW_READY);
  }
}

void updateWorkflow() {
  const uint32_t elapsed = millis() - stateEnteredAt;

  // ===== APPROACHING：等音樂播完 =====
  if (currentState == State::APPROACHING) {
    if (EmotionTruckSounds::updateTruckMusic()) {
      enterState(State::ARRIVED);
    }
    return;
  }

  // ===== COLLECTED：等音樂播完才進入下一個狀態 =====
  if (currentState == State::COLLECTED) {
    const bool musicDone = EmotionTruckSounds::updateTruckMusic();
    if (musicDone) {
      // 音樂播完，增加清運次數
      if (dailyCollectCount < DAILY_COLLECTION_LIMIT) {
        ++dailyCollectCount;
      }
      Serial.printf("Daily collection count: %u/%u\n",
                    dailyCollectCount, DAILY_COLLECTION_LIMIT);

      if (dailyCollectCount >= DAILY_COLLECTION_LIMIT) {
        enterState(State::REST_DAILY_LIMIT);
      } else {
        enterState(State::IDLE);
      }
    }
    return;
  }

  // ===== THROWING：等 1 秒後隨機回應 =====
  if (currentState == State::THROWING && elapsed >= THROWING_HOLD_MS) {
    const uint8_t response = random(100);
    if (response < 50) {
      enterState(State::RESPONDING_CAREFUL);
    } else if (response < 75) {
      enterState(State::RESPONDING_THANKS);
    } else {
      enterState(State::RESPONDING_RECYCLE);
    }
    return;
  }

  // ===== RESPONDING：等 1.5 秒後進入 COLLECTED =====
  if (currentState == State::RESPONDING_CAREFUL ||
      currentState == State::RESPONDING_THANKS ||
      currentState == State::RESPONDING_RECYCLE) {
    if (elapsed >= RESPONSE_DISPLAY_MS) {
      enterState(State::COLLECTED);
      return;
    }
  }
}

void printHelp();

void processCommand(String command) {
  command.trim();
  command.toLowerCase();

  if (command == "state rest" || command == "state closed") {
    enterState(State::REST_CLOSED);
  } else if (command == "state daily limit") {
    enterState(State::REST_DAILY_LIMIT);
  } else if (command == "state idle") {
    enterState(State::IDLE);
  } else if (command == "state approaching") {
    enterState(State::APPROACHING);
  } else if (command == "state arrived") {
    enterState(State::ARRIVED);
  } else if (command == "state throw ready") {
    enterState(State::THROW_READY);
  } else if (command == "state throwing") {
    enterState(State::THROWING);
  } else if (command == "state careful") {
    if (currentState != State::REST_DAILY_LIMIT) {
      enterState(State::RESPONDING_CAREFUL);
    }
  } else if (command == "state thanks") {
    if (currentState != State::REST_DAILY_LIMIT) {
      enterState(State::RESPONDING_THANKS);
    }
  } else if (command == "state recycle") {
    if (currentState != State::REST_DAILY_LIMIT) {
      enterState(State::RESPONDING_RECYCLE);
    }
  } else if (command == "state collected") {
    if (currentState != State::REST_DAILY_LIMIT) {
      enterState(State::COLLECTED);
    }
  } else if (command == "test careful") {
    enterState(State::RESPONDING_CAREFUL);
  } else if (command == "test thanks") {
    enterState(State::RESPONDING_THANKS);
  } else if (command == "test recycle") {
    enterState(State::RESPONDING_RECYCLE);
  } else if (command == "test collected") {
    enterState(State::COLLECTED);
  } else if (command == "throw reset") {
    resetThrowSelection();
    if (currentState == State::ARRIVED) {
      showLcdFace("Truck is here", FACE_HAPPY);
    }
    Serial.println("Throw gesture counters reset.");
  } else if (command == "reset") {
    emotionBagCount = 0;
    returnedBagCount = 0;
    dailyCollectCount = 0;
    resetDirectionSelection();
    enterState(State::IDLE);
  } else if (command == "reset day") {
    dailyCollectCount = 0;
    Serial.println("Daily collection count reset; bags preserved.");
    enterState(State::IDLE);
  } else if (command == "status") {
    Serial.printf("State=%s, bags=%u/5, daily=%u/%u, selected=%s, direction=%s, count=%u/3, rotation=%.1f/1080 (%s)\n",
                  stateName(currentState), emotionBagCount,
                  dailyCollectCount, DAILY_COLLECTION_LIMIT,
                  emotionName(selectedEmotion), directionName(selectedDirection),
                  directionCount, throwAccumulatedDegrees,
                  throwRotationDirection == RotationDirection::CLOCKWISE
                      ? "CW"
                      : throwRotationDirection == RotationDirection::COUNTERCLOCKWISE
                            ? "CCW"
                            : "NONE");
  } else if (command == "help" || command == "m0") {
    printHelp();
  } else if (command.length() > 0) {
    Serial.printf("Unknown command: %s\n", command.c_str());
  }
}

void printHelp() {
  Serial.println();
  Serial.println("=== JOYSTICK + SINGLE LED TEST ===");
  Serial.println("ARRIVED: rotate CW or CCW for 3 rounds, then press SW");
  Serial.println("DOWN      -> cancel emotion selection only");
  Serial.println("state closed/daily limit/idle");
  Serial.println("state approaching -> Fur Elise, then ARRIVED");
  Serial.println("state arrived -> skip music and test throwing gesture");
  Serial.println("state throwing");
  Serial.println("state throw ready");
  Serial.println("state careful/thanks/recycle/collected");
  Serial.println("test careful/thanks/recycle/collected -> bypass daily-limit guard");
  Serial.println("throw reset -> reset rotation counters");
  Serial.println("reset     -> clear bags and return to IDLE");
  Serial.println("reset day -> clear daily count, preserve bags, enter IDLE");
  Serial.println("status    -> show current state");
  Serial.println("help      -> show commands");
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(JOYSTICK_BUTTON_PIN, INPUT_PULLUP);
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  lcd.init();
  lcd.backlight();
  lcd.createChar(FACE_SLEEP, sleepFace);
  lcd.createChar(FACE_SMILE, smileFace);
  lcd.createChar(FACE_ANGRY, angryFace);
  lcd.createChar(FACE_FULL, fullFace);
  lcd.createChar(FACE_SURPRISE, surpriseFace);
  lcd.createChar(FACE_HAPPY, happyFace);
  lcd.createChar(FACE_TRASH, trashBag);
  lcd.createChar(FACE_RELAX, relaxFace);

  ledcAttach(LED_PIN, LED_PWM_FREQUENCY, LED_PWM_RESOLUTION);
  ledcAttach(BUZZER_PIN, 2000, 8);
  EmotionTruckSounds::stop();

  Serial.println("=== EMOTION TRUCK: JOYSTICK + SINGLE LED ===");
  Serial.println("LCD: SDA=21 SCL=22 address=0x27");
  Serial.println("Single LED: GPIO 27 through 220 ohm resistor");
  Serial.println("Buzzer: GPIO 25");
  printHelp();
  enterState(State::REST_CLOSED);
}

void loop() {
  static bool previousButtonPressed = false;
  static uint32_t buttonChangedAt = 0;
  static bool stableButtonPressed = false;

  while (Serial.available() > 0) {
    const char incoming = static_cast<char>(Serial.read());
    if (incoming == '\n' || incoming == '\r') {
      if (serialInput.length() > 0) {
        processCommand(serialInput);
        serialInput = "";
      }
    } else {
      serialInput += incoming;
    }
  }

  const uint32_t now = millis();
  const bool rawButtonPressed = digitalRead(JOYSTICK_BUTTON_PIN) == LOW;
  if (rawButtonPressed != previousButtonPressed) {
    previousButtonPressed = rawButtonPressed;
    buttonChangedAt = now;
  }

  bool pressedEdge = false;
  if (now - buttonChangedAt >= BUTTON_DEBOUNCE_MS &&
      stableButtonPressed != rawButtonPressed) {
    stableButtonPressed = rawButtonPressed;
    if (stableButtonPressed) {
      pressedEdge = true;
    }
  }

  const Direction direction = readDirection(
      analogRead(JOYSTICK_X_PIN), analogRead(JOYSTICK_Y_PIN));
  if (currentState == State::ARRIVED || currentState == State::THROW_READY) {
    handleThrowDirection(direction, pressedEdge);
  } else {
    if (pressedEdge) {
      confirmEmotion();
    }
    handleDirection(direction);
  }
  updateWorkflow();
  updateLedAnimation();
  delay(5);
}
