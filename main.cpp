#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <Audio.h>  // From ESP8266Audio (works on ESP32 too)

// SD
#define SD_CS 5

// I2S Audio
#include <AudioFileSourceSD.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>

AudioGeneratorMP3 *mp3;
AudioFileSourceSD *file;
AudioOutputI2S *out;

const int buttonPin = 21;
bool lastButtonState = HIGH;

void setup() {
  Serial.begin(115200);

  pinMode(buttonPin, INPUT_PULLUP);

  // SD card setup
  if (!SD.begin(SD_CS)) {
    Serial.println("SD Card mount failed!");
    return;
  }

  // I2S config
  out = new AudioOutputI2S();
  out->SetPinout(26, 25, 22);  // BCLK, LRC, DIN
}

void loop() {
  bool buttonState = digitalRead(buttonPin);

  if (lastButtonState == HIGH && buttonState == LOW) {
    Serial.println("Button pressed");

    if (mp3) {
      mp3->stop();
      delete mp3;
    }

    file = new AudioFileSourceSD("/track.mp3");  // must exist on SD
    mp3 = new AudioGeneratorMP3();
    mp3->begin(file, out);
  }

  if (mp3 && mp3->isRunning()) {
    mp3->loop();
  }

  lastButtonState = buttonState;
}
