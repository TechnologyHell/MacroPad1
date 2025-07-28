// ------------------------- LIBRARIES -------------------------

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Encoder.h>
#include <HID-Project.h>
#include <Adafruit_NeoPixel.h>

// ------------------------- SWITCHES -------------------------
const uint8_t switchPins[8] = {A1, A0, 7, 8, 15, 14, 16, 9};
bool switchState[8] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};

// ------------------------- DISPLAY -------------------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
String lines[4];
int currentLine = 0;

// ------------------------- ENCODER -------------------------
#define CLK_PIN 4
#define DT_PIN 5
#define SW_PIN 6
Encoder knob(CLK_PIN, DT_PIN);
long lastPos = 0;
bool lastBtn = HIGH;

// ------------------------- LIGHTING -------------------------
#define LED_PIN A2
#define LED_COUNT 8
#define NUM_LEDS 8
#define TOUCH_PIN A3
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
uint8_t currentMode = 0;
const uint8_t totalModes = 20;
bool lastTouchState = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;
unsigned long lastEffectUpdate = 0;
uint16_t effectStep = 0;
float brightnessScale[LED_COUNT] = {0.2, 0.2, 0.2, 0.2, 0.45, 0.45, 0.45, 0.45};

// ------------------------- SETUP -------------------------
void setup() {
  // Switches
  for (int i = 0; i < 8; i++) pinMode(switchPins[i], INPUT_PULLUP);
  Keyboard.begin();

  // Display
  Serial.begin(9600);
  Wire.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) while (1);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Waiting for data...");
  display.display();

  // Encoder
  pinMode(SW_PIN, INPUT_PULLUP);
  Consumer.begin();
  knob.write(0);

  // Lighting
  strip.begin();
  strip.setBrightness(255);
  strip.show();
  pinMode(TOUCH_PIN, INPUT);
}

// ------------------------- LOOP -------------------------
void loop() {
  handleSwitches();
  handleDisplaySerial();
  handleEncoder();
  handleTouchSensor();
  runCurrentMode();
}

// ------------------------- SWITCH HANDLING -------------------------
void handleSwitches() {
  for (int i = 0; i < 8; i++) {
    bool currentState = digitalRead(switchPins[i]);
    if (currentState == LOW && switchState[i] == HIGH) {
      handleKey(i);
      delay(200);
    }
    switchState[i] = currentState;
  }
}

void handleKey(int index) {
  switch (index) {
    case 0: Keyboard.press(KEY_LEFT_CTRL); Keyboard.press(KEY_LEFT_SHIFT); Keyboard.press(KEY_ESC); break;             // Task Manager
    case 1: Keyboard.press(KEY_LEFT_GUI); Keyboard.press(KEY_LEFT_SHIFT); Keyboard.press('s'); break;                 // Snipping Tool
    case 2: Keyboard.press(KEY_LEFT_GUI); Keyboard.press('i'); break;                                                 // Settings
    case 3: Keyboard.press(KEY_LEFT_GUI); Keyboard.press('p'); break;                                                 // Project Screen
    case 4: Keyboard.press(KEY_LEFT_GUI); Keyboard.press('l'); break;                                                 // Lock PC
    case 5: Keyboard.press(KEY_LEFT_GUI); Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('v'); break;                  // Switch Audio
    case 6: Keyboard.press(KEY_LEFT_GUI); Keyboard.press('d'); break;                                                 // Minimize All
    case 7: Keyboard.press(KEY_LEFT_GUI); Keyboard.press('g'); break;                                                 // Game Bar
  }
  delay(100);
  Keyboard.releaseAll();
}

// ------------------------- DISPLAY HANDLING -------------------------
void handleDisplaySerial() {
  static String input = "";
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      if (currentLine < 4) lines[currentLine++] = input;
      input = "";
      if (currentLine == 4) {
        updateDisplay();
        currentLine = 0;
      }
    } else {
      input += c;
    }
  }
}

void updateDisplay() {
  display.clearDisplay();
  for (int i = 0; i < 4; i++) {
    display.setCursor(0, i * 8);
    display.print(lines[i]);
  }
  display.display();
}

// ------------------------- ENCODER HANDLING -------------------------
void handleEncoder() {
  long pos = knob.read();
  long delta = pos - lastPos;

  if (abs(delta) >= 4) {
    Consumer.write(delta > 0 ? MEDIA_VOLUME_DOWN : MEDIA_VOLUME_UP);
    lastPos = pos;
    delay(50);
  }

  bool btn = digitalRead(SW_PIN);
  if (btn == LOW && lastBtn == HIGH) {
    Consumer.write(MEDIA_VOLUME_MUTE);
    delay(200);
  }
  lastBtn = btn;
}

// ------------------------- TOUCH + LIGHTING HANDLING -------------------------
void handleTouchSensor() {
  bool touchState = digitalRead(TOUCH_PIN);
  if (touchState && !lastTouchState && (millis() - lastDebounceTime > debounceDelay)) {
    currentMode = (currentMode + 1) % totalModes;
    strip.clear(); strip.show();
    lastDebounceTime = millis();
  }
  lastTouchState = touchState;
}


void runCurrentMode() {
  switch (currentMode) {
    case 0: solidColor(strip.Color(255, 0, 0)); break;
    case 1: solidColor(strip.Color(0, 255, 0)); break;
    case 2: solidColor(strip.Color(0, 0, 255)); break;
    case 3: solidColor(strip.Color(255, 255, 0)); break;
    case 4: solidColor(strip.Color(0, 255, 255)); break;
    case 5: solidColor(strip.Color(255, 0, 255)); break;
    case 6: solidColor(strip.Color(255, 255, 255)); break;
    case 7: rainbowShift(); break;
    case 8: rgbBreathing(); break;
    case 9: colorFadeLoop(); break;
    case 10: fadeColors(); break;
    case 11: rainbowChaseFade(); break;
    case 12: fireFlicker(); break;
    case 13: oceanWave(); break;
    case 14: glitchRainbow(); break;
    case 15: knightRider(); break;
    case 16: pastelBreath(); break;
    case 17: policeLights(); break;
    case 18: sparklePop(); break;
    case 19: glitchBlink(); break;
  }
}

// ------------------------- LIGHTING EFFECTS -------------------------
uint32_t applyDimming(uint32_t color, int index) {
  float factor = brightnessScale[index];
  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;
  return strip.Color(r * factor, g * factor, b * factor);
}

void setPixelScaled(int i, uint8_t r, uint8_t g, uint8_t b) {
  float scale = brightnessScale[i];
  strip.setPixelColor(i, strip.Color(r * scale, g * scale, b * scale));
}

void solidColor(uint32_t c) {
  for (int i = 0; i < LED_COUNT; i++) strip.setPixelColor(i, applyDimming(c, i));
  strip.show();
}

void rainbowShift() {
  if (millis() - lastEffectUpdate > 20) {
    for (int i = 0; i < LED_COUNT; i++) {
      uint32_t color = strip.ColorHSV((effectStep + i * 100) % 65536);
      strip.setPixelColor(i, applyDimming(color, i));
    }
    strip.show();
    effectStep += 256;
    lastEffectUpdate = millis();
  }
}

void rgbBreathing() {
  uint8_t brightness = abs(sin(millis() / 1000.0) * 255);
  for (int i = 0; i < LED_COUNT; i++) {
    uint32_t color = strip.Color(brightness, 0, 255 - brightness);
    strip.setPixelColor(i, applyDimming(color, i));
  }
  strip.show();
}

void colorFadeLoop() {
  if (millis() - lastEffectUpdate > 30) {
    for (int i = 0; i < LED_COUNT; i++) {
      uint32_t color = strip.ColorHSV((effectStep + i * 500) % 65536);
      strip.setPixelColor(i, applyDimming(color, i));
    }
    strip.show();
    effectStep += 128;
    lastEffectUpdate = millis();
  }
}

void rainbowChaseFade() {
  static uint8_t startHue = 0;
  startHue += 2;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t hue = startHue + i * 20;
    uint32_t color = strip.ColorHSV(hue * 256, 255, 255);
    setPixelScaled(i, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
  }
  strip.show();
  delay(40);
}

void fadeColors() {
  static uint8_t hue = 0;
  hue++;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint32_t color = strip.ColorHSV(hue * 256, 255, 255);
    setPixelScaled(i, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
  }
  strip.show();
  delay(30);
}

void fireFlicker() {
  if (millis() - lastEffectUpdate > 70) {
    for (int i = 0; i < LED_COUNT; i++) {
      uint32_t color = strip.Color(random(180, 255), random(45, 64), 0);
      strip.setPixelColor(i, applyDimming(color, i));
    }
    strip.show();
    lastEffectUpdate = millis();
  }
}

void oceanWave() {
  if (millis() - lastEffectUpdate > 60) {
    for (int i = 0; i < LED_COUNT; i++) {
      uint8_t blue = 128 + sin((i + effectStep) * 0.3) * 127;
      uint32_t color = strip.Color(0, blue / 4, blue);
      strip.setPixelColor(i, applyDimming(color, i));
    }
    strip.show();
    effectStep++;
    lastEffectUpdate = millis();
  }
}

void glitchBlink() {
  if (millis() - lastEffectUpdate > 80) {
    for (int i = 0; i < LED_COUNT; i++) {
      uint32_t color = random(2) ? strip.Color(255, 255, 255) : 0;
      strip.setPixelColor(i, applyDimming(color, i));
    }
    strip.show();
    lastEffectUpdate = millis();
  }
}

void sparklePop() {
  if (millis() - lastEffectUpdate > 50) {
    int idx = random(LED_COUNT);
    strip.setPixelColor(idx, applyDimming(strip.Color(random(255), random(255), random(255)), idx));
    strip.show();
    delay(10);
    strip.setPixelColor(idx, 0);
    strip.show();
    lastEffectUpdate = millis();
  }
}

void pastelBreath() {
  float t = millis() / 2000.0;
  float s = (sin(t * 2 * PI) + 1.0) / 2.0;
  uint32_t color = strip.Color(127 + s * 128, 180 + s * 75, 255);
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, applyDimming(color, i));
  }
  strip.show();
}

void policeLights() {
  static bool flash = false;
  if (millis() - lastEffectUpdate > 200) {
    for (int i = 0; i < LED_COUNT; i++) {
      uint32_t color = (i < LED_COUNT / 2) ? (flash ? strip.Color(255, 0, 0) : 0) : (flash ? 0 : strip.Color(0, 0, 255));
      strip.setPixelColor(i, applyDimming(color, i));
    }
    strip.show();
    flash = !flash;
    lastEffectUpdate = millis();
  }
}

void knightRider() {
  static int pos = 0;
  static int dir = 1;
  if (millis() - lastEffectUpdate > 50) {
    strip.clear();
    strip.setPixelColor(pos, applyDimming(strip.Color(255, 0, 0), pos));
    strip.show();
    pos += dir;
    if (pos <= 0 || pos >= LED_COUNT - 1) dir = -dir;
    lastEffectUpdate = millis();
  }
}

void glitchRainbow() {
  if (millis() - lastEffectUpdate > 100) {
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, applyDimming(strip.ColorHSV(random(65536)), i));
    }
    strip.show();
    lastEffectUpdate = millis();
  }
}
