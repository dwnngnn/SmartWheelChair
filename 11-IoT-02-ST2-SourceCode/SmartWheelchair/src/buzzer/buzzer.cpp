#include "buzzer.h"
#include "../def.h"
#include <Arduino.h>

// Melody Happy Birthday: (Frequency, Duration in ms)
const int melody[][2] = {
  {NOTE_G4, 200}, {NOTE_G4, 200}, {NOTE_A4, 400}, {NOTE_G4, 400}, {NOTE_C5, 400}, {NOTE_B4, 800},
  {NOTE_G4, 200}, {NOTE_G4, 200}, {NOTE_A4, 400}, {NOTE_G4, 400}, {NOTE_D5, 400}, {NOTE_C5, 800},
  {NOTE_G4, 200}, {NOTE_G4, 200}, {NOTE_G5, 400}, {NOTE_E5, 400}, {NOTE_C5, 400}, {NOTE_B4, 400}, {NOTE_A4, 800},
  {NOTE_F5, 200}, {NOTE_F5, 200}, {NOTE_E5, 400}, {NOTE_C5, 400}, {NOTE_D5, 400}, {NOTE_C5, 800}
};
const int melodyLength = 25;

unsigned long lastNoteTime = 0;
int currentNote = 0;
bool isPlaying = false;

void handleBuzzer() {
  if (distance < 15.0) {  
    if (!isPlaying) {
      isPlaying = true;
      currentNote = 0;
      lastNoteTime = millis();
      tone(BUZZER_PIN, melody[currentNote][0]);
    }

    if (millis() - lastNoteTime >= melody[currentNote][1]) {
      currentNote = (currentNote + 1) % melodyLength;
      lastNoteTime = millis();
      tone(BUZZER_PIN, melody[currentNote][0]);
    }
  } else {
    if (isPlaying) {
      noTone(BUZZER_PIN);
      isPlaying = false;
    }
  }
}
