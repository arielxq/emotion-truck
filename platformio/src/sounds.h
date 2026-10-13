#ifndef EMOTION_TRUCK_SOUNDS_H
#define EMOTION_TRUCK_SOUNDS_H

#include <Arduino.h>

constexpr uint8_t SOUND_BUZZER_PIN = 25;

namespace EmotionTruckSounds {

struct Note {
  uint16_t frequency;
  uint16_t durationMs;
};

constexpr uint8_t MAX_VOLUME = 80;
constexpr uint16_t TRUCK_MUSIC_DURATION_MS = 10000;

// ===== 其他音效用映射 =====
constexpr uint16_t mapToSafeRange(uint16_t freq) {
  if (freq == 0) return 0;
  uint16_t f = freq;
  while (f > 800) f /= 2;
  while (f < 400) f *= 2;
  return f;
}

// ===== Fur Elise 整體降八度（不選擇性映射）=====
constexpr uint16_t lowerOctave(uint16_t freq) {
  if (freq == 0) return 0;
  return freq / 2;
}

void setVolume(uint8_t volume) {
  ledcWrite(SOUND_BUZZER_PIN, volume);
}

void stop() {
  ledcWriteTone(SOUND_BUZZER_PIN, 0);
  setVolume(0);
}

// 其他音效：用 mapToSafeRange
void playNotes(const Note* notes, size_t count) {
  for (size_t i = 0; i < count; ++i) {
    if (notes[i].frequency == 0) {
      ledcWriteTone(SOUND_BUZZER_PIN, 0);
      setVolume(0);
    } else {
      ledcWriteTone(SOUND_BUZZER_PIN, mapToSafeRange(notes[i].frequency));
      setVolume(MAX_VOLUME);
    }
    delay(notes[i].durationMs);
  }
  stop();
}

// Fur Elise：用 lowerOctave（整體降八度，不破壞音程）
void playNotesFurElise(const Note* notes, size_t count) {
  for (size_t i = 0; i < count; ++i) {
    if (notes[i].frequency == 0) {
      ledcWriteTone(SOUND_BUZZER_PIN, 0);
      setVolume(0);
    } else {
      ledcWriteTone(SOUND_BUZZER_PIN, lowerOctave(notes[i].frequency));
      setVolume(MAX_VOLUME);
    }
    delay(notes[i].durationMs);
  }
  stop();
}

// ===== ACCUMULATING =====
void playAccumulating() {
  static const Note melody[] = {
      {784, 109}, {1175, 196}, {0, 40},
  };
  playNotes(melody, sizeof(melody) / sizeof(melody[0]));
}

// ===== FULL =====
void playFull() {
  static const Note melody[] = {
      {349, 20}, {1319, 100}, {330, 200}, {494, 200}, {1976, 80},
  };
  playNotes(melody, sizeof(melody) / sizeof(melody[0]));
}

// ===== THROW =====
void playThrow() {
  static const Note melody[] = {
      {698, 400}, {2794, 70}, {0, 30}, {880, 400}, {1397, 300},
  };
  playNotes(melody, sizeof(melody) / sizeof(melody[0]));
}

// ===== 工作人員：小心 =====
void playWorkerCareful() {
  static const Note melody[] = {
      {523, 120}, {659, 120}, {0, 50},
  };
  playNotes(melody, sizeof(melody) / sizeof(melody[0]));
}

// ===== 工作人員：謝謝 =====
void playWorkerThanks() {
  static const Note melody[] = {
      {659, 150}, {659, 150}, {0, 50},
  };
  playNotes(melody, sizeof(melody) / sizeof(melody[0]));
}

// ===== 工作人員：回收 =====
void playWorkerRecycle() {
  static const Note melody[] = {
      {659, 150}, {523, 150}, {440, 300}, {0, 50},
  };
  playNotes(melody, sizeof(melody) / sizeof(melody[0]));
}

// ===== Fur Elise 完整版（原始音高，不映射）=====
constexpr Note TRUCK_PHRASE[] = {
    {659, 200}, {622, 200}, {659, 200}, {622, 200}, {659, 200},
    {494, 200}, {587, 200}, {523, 200}, {440, 400},
    {262, 200}, {330, 200}, {440, 200}, {494, 400},
    {330, 200}, {415, 200}, {494, 200}, {523, 400},
    {330, 200}, {659, 200}, {622, 200}, {659, 200},
    {622, 200}, {659, 200}, {494, 200}, {587, 200},
    {523, 200}, {440, 400},
    {262, 200}, {330, 200}, {440, 200}, {494, 600},
    {294, 600}, {523, 200}, {494, 200}, {440, 400},
    {262, 400},
};

uint8_t fadeVolume(uint32_t elapsedMs, bool fadeIn) {
  const uint32_t elapsed = min(
      elapsedMs, static_cast<uint32_t>(TRUCK_MUSIC_DURATION_MS));
  const uint32_t progress = fadeIn
                                 ? elapsed
                                 : TRUCK_MUSIC_DURATION_MS - elapsed;
  return static_cast<uint8_t>(10 +
                              ((MAX_VOLUME - 10) * progress) /
                                  TRUCK_MUSIC_DURATION_MS);
}

void playTruckMusic(bool fadeIn) {
  const uint32_t startedAt = millis();
  size_t noteIndex = 0;
  constexpr size_t PHRASE_LENGTH =
      sizeof(TRUCK_PHRASE) / sizeof(TRUCK_PHRASE[0]);

  while (millis() - startedAt < TRUCK_MUSIC_DURATION_MS) {
    const Note& note = TRUCK_PHRASE[noteIndex];
    const uint32_t noteStartedAt = millis();

    // Fur Elise 用 lowerOctave（整體降八度，不破壞音程）
    ledcWriteTone(SOUND_BUZZER_PIN, lowerOctave(note.frequency));
    while (millis() - noteStartedAt < note.durationMs &&
           millis() - startedAt < TRUCK_MUSIC_DURATION_MS) {
      setVolume(fadeVolume(millis() - startedAt, fadeIn));
      delay(10);
    }

    noteIndex = (noteIndex + 1) % PHRASE_LENGTH;
  }
  stop();
}

void playApproaching() {
  playTruckMusic(true);
}

void playCollected() {
  playTruckMusic(false);
}

// Non-blocking Fur Elise player used by the state machine. The older
// playTruckMusic() functions remain available for isolated sound tests.
struct TruckMusicPlayer {
  bool active = false;
  bool fadeIn = true;
  uint32_t startedAt = 0;
  uint32_t noteStartedAt = 0;
  size_t noteIndex = 0;
};

TruckMusicPlayer truckMusic;

void startTruckMusic(bool fadeIn) {
  truckMusic.active = true;
  truckMusic.fadeIn = fadeIn;
  truckMusic.startedAt = millis();
  truckMusic.noteStartedAt = truckMusic.startedAt;
  truckMusic.noteIndex = 0;
  const Note& note = TRUCK_PHRASE[0];
  ledcWriteTone(SOUND_BUZZER_PIN, lowerOctave(note.frequency));
  setVolume(fadeVolume(0, fadeIn));
}

bool updateTruckMusic() {
  if (!truckMusic.active) {
    return true;
  }

  const uint32_t now = millis();
  const uint32_t elapsed = now - truckMusic.startedAt;
  constexpr size_t phraseLength = sizeof(TRUCK_PHRASE) / sizeof(TRUCK_PHRASE[0]);

  if (elapsed >= TRUCK_MUSIC_DURATION_MS) {
    truckMusic.active = false;
    stop();
    return true;
  }

  const Note& note = TRUCK_PHRASE[truckMusic.noteIndex];
  if (now - truckMusic.noteStartedAt >= note.durationMs) {
    truckMusic.noteIndex = (truckMusic.noteIndex + 1) % phraseLength;
    truckMusic.noteStartedAt = now;
    ledcWriteTone(SOUND_BUZZER_PIN,
                  lowerOctave(TRUCK_PHRASE[truckMusic.noteIndex].frequency));
  }

  setVolume(fadeVolume(elapsed, truckMusic.fadeIn));
  return false;
}

void stopTruckMusic() {
  truckMusic.active = false;
  stop();
}

}  // namespace EmotionTruckSounds

#endif
