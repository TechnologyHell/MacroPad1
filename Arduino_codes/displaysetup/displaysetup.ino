#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String lines[4];  // Store 4 lines
int currentLine = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;); // Halt if OLED not found
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Waiting for data...");
  display.display();
}

void loop() {
  static String input = "";

  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {
      if (currentLine < 4) {
        lines[currentLine] = input;
        currentLine++;
      }
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
    display.setCursor(0, i * 8);  // 8 px height per line
    display.print(lines[i]);
  }
  display.display();
}
