#include <Adafruit_NeoPixel.h>

#define LED_PIN A2     // LED data pin
#define LED_COUNT 8    // Number of LEDs
#define NUM_LEDS 8
#define TOUCH_PIN A3   // TTP223 sensor pin

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

uint8_t currentMode = 0;
const uint8_t totalModes = 20;

bool lastTouchState = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

unsigned long lastEffectUpdate = 0;
uint16_t effectStep = 0;

// Brightness scalars (0.0 to 1.0)
float brightnessScale[NUM_LEDS] = {
  0.2, 0.2, 0.2, 0.2,  // Top 4 LEDs (row 1)
  0.3, 0.3, 0.3, 0.3   // Bottom 4 LEDs (row 2)
};

void setPixelScaled(int i, uint8_t r, uint8_t g, uint8_t b) {
  float scale = brightnessScale[i];
  strip.setPixelColor(i, strip.Color(r * scale, g * scale, b * scale));
}

// Apply dimming profile to color depending on LED index
uint32_t applyDimming(uint32_t color, int index) {
  float factor = (index < 4) ? 0.2 : 0.3;  // 20% for top row, 30% for bottom
  uint8_t r = (uint8_t)(color >> 16);
  uint8_t g = (uint8_t)(color >> 8);
  uint8_t b = (uint8_t)color;
  return strip.Color(r * factor, g * factor, b * factor);
}

void setup() {
  strip.begin();
  strip.setBrightness(255); // Keep full scale; we're manually controlling per-LED brightness
  strip.show();
  pinMode(TOUCH_PIN, INPUT);
}

void loop() {
  handleTouch();
  runCurrentMode();
}

void handleTouch() {
  bool touchState = digitalRead(TOUCH_PIN);
  if (touchState && !lastTouchState && (millis() - lastDebounceTime > debounceDelay)) {
    currentMode = (currentMode + 1) % totalModes;
    strip.clear();
    strip.show();
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
    case 14: glitchBlink(); break;
    case 15: sparklePop(); break;
    case 16: pastelBreath(); break;
    case 17: policeLights(); break;
    case 18: knightRider(); break;
    case 19: glitchRainbow(); break;
  }
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
