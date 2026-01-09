/*
          ****************************************************************************
******************* Well Come to My Flashlight Data Transfer Sketch 2026 **********************************
          ****************************************************************************
***************************** Created By Piyush Kumar Gupta ***********************************************
          ****************************************************************************
*********************************** JAY SHREE RAM *******************************************************
          ****************************************************************************
*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// SPI OLED Pins
#define OLED_MOSI   11
#define OLED_CLK    13
#define OLED_DC     8
#define OLED_CS     10
#define OLED_RESET  9

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET, OLED_CS);

#define LDR_PIN A0
#define LED_PIN 4        // 🔹 Arduino UNO/Nano pin 4 par LED
#define BUZZER_PIN 3     // 🔹 Arduino UNO/Nano pin 3 par Buzzer

// ---------- Adjustable Settings ----------
int threshold = 430;           
const int dotDuration = 350;   
const int dashDuration = dotDuration * 3; 
const int charGap = dotDuration * 2; 
const int wordGap = dotDuration * 8;

String morseCode = "";
String decodedWord = "";
String fullText = "";
unsigned long signalStart = 0;
bool signalOn = false;
unsigned long lastSignalTime = 0;

void setup() {
  pinMode(LDR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1); 
  display.setCursor(10, 20);
  display.print("DATA RECEIVER READY");
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  int lightValue = analogRead(LDR_PIN);
  delay(100);

  if (lightValue > threshold && !signalOn) {
    signalOn = true;
    signalStart = millis();
  }

  if (lightValue < threshold && signalOn) {
    signalOn = false;
    unsigned long duration = millis() - signalStart;

    if (duration < dashDuration)
      morseCode += ".";
    else
      morseCode += "-";

    lastSignalTime = millis();
  }

  // ✅ जब एक letter पूरा हो जाए
  if (!signalOn && morseCode != "" && millis() - lastSignalTime > charGap) {
    char decodedChar = decodeMorse(morseCode);
    if (decodedChar != '?') {
      decodedWord += decodedChar;

      // 🔔 केवल जब letter receive हो तब beep और LED flash
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);
    }
    morseCode = "";
  }

  // ✅ जब पूरा शब्द (या पूरा message) हो जाए
  if (!signalOn && decodedWord != "" && millis() - lastSignalTime > wordGap) {
    addWordToDisplay(decodedWord);
    decodedWord = "";

    // 🔔 Long beep for full data received
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(350);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }
}

// -------- Display me multiple words ko scrolling karne ke liye --------
void addWordToDisplay(String word) {
  fullText += word + " ";

  if (fullText.length() > 100) {
    fullText.remove(0, 30);
  }

  display.clearDisplay();
  display.setTextSize(1);

  int y = 0;
  int start = 0;

  for (int i = 0; i < fullText.length(); i++) {
    if ((i - start) >= 21 || fullText[i] == '\n') { 
      display.setCursor(0, y);
      display.print(fullText.substring(start, i));
      y += 10; 
      start = i + 1;
    }
  }

  display.setCursor(0, y);
  display.print(fullText.substring(start));
  display.display();
}

// -------- Morse Code Decoder (A–Z, 0–9, symbols) --------
char decodeMorse(String code) {
  // Letters
  if (code == ".-") return 'A'; if (code == "-...") return 'B'; if (code == "-.-.") return 'C';
  if (code == "-..") return 'D'; if (code == ".") return 'E'; if (code == "..-.") return 'F';
  if (code == "--.") return 'G'; if (code == "....") return 'H'; if (code == "..") return 'I';
  if (code == ".---") return 'J'; if (code == "-.-") return 'K'; if (code == ".-..") return 'L';
  if (code == "--") return 'M'; if (code == "-.") return 'N'; if (code == "---") return 'O';
  if (code == ".--.") return 'P'; if (code == "--.-") return 'Q'; if (code == ".-.") return 'R';
  if (code == "...") return 'S'; if (code == "-") return 'T'; if (code == "..-") return 'U';
  if (code == "...-") return 'V'; if (code == ".--") return 'W'; if (code == "-..-") return 'X';
  if (code == "-.--") return 'Y'; if (code == "--..") return 'Z';

  // Digits
  if (code == "-----") return '0'; if (code == ".----") return '1'; if (code == "..---") return '2';
  if (code == "...--") return '3'; if (code == "....-") return '4'; if (code == ".....") return '5';
  if (code == "-....") return '6'; if (code == "--...") return '7'; if (code == "---..") return '8';
  if (code == "----.") return '9';

  // Symbols
  if (code == "..--..") return '?';
  if (code == ".-.-.-") return '.';
  if (code == "--..--") return ',';
  if (code == "-....-") return '-';
  if (code == ".-..-.") return '"';
  if (code == "-..-.") return '/';
  if (code == ".-.-.") return '+';
  if (code == "-..-") return '*';
  if (code == "..--.-") return '_';
  if (code == ".--.-.") return '@';

  return '?';
}
