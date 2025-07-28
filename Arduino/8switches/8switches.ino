#include <Keyboard.h>

const uint8_t switchPins[8] = {A1, A0, 7, 8, 15, 14, 16, 9};
bool switchState[8] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};

void setup() {
  for (int i = 0; i < 8; i++) {
    pinMode(switchPins[i], INPUT_PULLUP);
  }
  Keyboard.begin();
}

void loop() {
  for (int i = 0; i < 8; i++) {
    bool currentState = digitalRead(switchPins[i]);

    if (currentState == LOW && switchState[i] == HIGH) {
      // Button just pressed
      handleKey(i);
      delay(200); // Debounce
    }

    switchState[i] = currentState;
  }
}

void handleKey(int index) {
  switch (index) {
    case 0: // Task Manager - Ctrl + Shift + Esc
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press(KEY_ESC);
      delay(100);
      Keyboard.releaseAll();
      break;

    case 1: // Snipping Tool - Win + Shift + S
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press('s');
      delay(100);
      Keyboard.releaseAll();
      break;

    case 2: // Settings - Win + I
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press('i');
      delay(100);
      Keyboard.releaseAll();
      break;

    case 3: // Project Screen - Win + P
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press('p');
      delay(100);
      Keyboard.releaseAll();
      break;

    case 4: // Lock PC - Win + L
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press('l');
      delay(100);
      Keyboard.releaseAll();
      break;

    case 5: // Switch Audio Output - Win + Ctrl + V
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press('v');
      delay(100);
      Keyboard.releaseAll();
      break;

    case 6: // Minimize All - Win + D
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press('d');
      delay(100);
      Keyboard.releaseAll();
      break;

    case 7: // Game Bar - Win + G
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press('g');
      delay(100);
      Keyboard.releaseAll();
      break;
  }
}
