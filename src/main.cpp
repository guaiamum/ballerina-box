#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>

#include <AudioFileSourceSPIFFS.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>

#define BUTTON_PIN 0  // Adjust based on your wiring

AudioGeneratorMP3 *mp3;
AudioFileSourceSPIFFS *file;
AudioOutputI2S *out;

bool isPlaying = false;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS failed");
    return;
  }

  out = new AudioOutputI2S();
  out->SetPinout(26, 25, 22);  // BCLK, LRC, DIN
  out->SetGain(1);           // Optional volume control

  mp3 = new AudioGeneratorMP3();
}

void loop() {
  static bool lastButtonState = HIGH;
  bool currentState = digitalRead(BUTTON_PIN);

  // Detect button press
  if (lastButtonState == HIGH && currentState == LOW && !isPlaying) {
    Serial.println("Button pressed, playing track");
    file = new AudioFileSourceSPIFFS("/track.mp3");
    mp3->begin(file, out);
    isPlaying = true;
  }
  lastButtonState = currentState;

  // Play if ongoing
  if (isPlaying) {
    if (!mp3->loop()) {
      Serial.println("Playback done");
      mp3->stop();
      delete file;
      isPlaying = false;
    }
  }
}
