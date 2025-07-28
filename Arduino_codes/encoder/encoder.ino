#include <Encoder.h>
#include <HID-Project.h>
#include <HID-Settings.h>

#define CLK_PIN 4
#define DT_PIN 5
#define SW_PIN 6

Encoder knob(CLK_PIN, DT_PIN);
long lastPos = 0;
bool lastBtn = HIGH;

void setup() {
  pinMode(SW_PIN, INPUT_PULLUP);
  Consumer.begin();
  knob.write(0);
}

void loop() {
  long pos = knob.read();
  long delta = pos - lastPos;

  if (abs(delta) >= 4) {  // Only act on significant movement
    if (delta > 0) {
      Consumer.write(MEDIA_VOLUME_DOWN);  // <- was UP
    } else {
      Consumer.write(MEDIA_VOLUME_UP);    // <- was DOWN
    }
    lastPos = pos;
    delay(50); // Optional: reduce jitter at very fast speeds
  }

  bool btn = digitalRead(SW_PIN);
  if (btn == LOW && lastBtn == HIGH) {
    Consumer.write(MEDIA_VOLUME_MUTE);
    delay(200);  // Debounce
  }
  lastBtn = btn;

  delay(5);
}
