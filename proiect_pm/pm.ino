#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <SPI.h>
#include <SD.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int pushButtonPins[] = {3, 4, 5}; // Pinii pentru butoane
const int buzzer = 2;
unsigned long buttonPressedTime[3] = {0, 0, 0};
bool buttonIsPressed[3] = {false, false, false};
unsigned long lastUpdate = 0; // Ultima actualizare a timpului afișat pe LCD

int frequencies[3][3] = {
    {440, 523, 622},
    {466, 554, 659},
    {494, 587}
};

void setup() {
    Serial.begin(9600);
    lcd.print("Initializing SD...");
    
    if (!SD.begin()) {
        Serial.print("SD fail!");
        return;
    }
    else {
      Serial.print("Succes!");
    }
    
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Hello, world!");
    for (int i = 0; i < 3; i++) {
        pinMode(pushButtonPins[i], INPUT_PULLUP);
    }
    pinMode(buzzer, OUTPUT);
}

void playMelodyFromSD() {
    File file = SD.open("melody.txt");
    if (!file) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("File not found!");
        return;
    }

    int note, duration;
    String line = file.readStringUntil('\n'); // Citirea primelor note
    String durationLine = file.readStringUntil('\n'); // Citirea duratelor

    int noteIndex = 0, durationIndex = 0;
    int lastIndex = 0, currIndex;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Playing music...");

    // Citire și redare note
    while ((currIndex = line.indexOf(',', lastIndex)) != -1) {
        note = line.substring(lastIndex, currIndex).toInt();
        lastIndex = currIndex + 1;

        if ((currIndex = durationLine.indexOf(',', durationIndex)) != -1) {
            duration = durationLine.substring(durationIndex, currIndex).toInt();
            durationIndex = currIndex + 1;

            // Afișarea fiecărei note și durate pe LCD
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Note: ");
            lcd.print(note);
            lcd.setCursor(0, 1);
            lcd.print("Duration: ");
            lcd.print(duration);

            tone(buzzer, note, duration);
            delay(duration); // Așteptăm durata notei
            noTone(buzzer);
        }
    }

    // Redă ultima notă
    note = line.substring(lastIndex).toInt();
    duration = durationLine.substring(durationIndex).toInt();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Note: ");
    lcd.print(note);
    lcd.setCursor(0, 1);
    lcd.print("Duration: ");
    lcd.print(duration);
    tone(buzzer, note, duration);
    delay(duration);
    noTone(buzzer);

    file.close();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Melody finished!");
}


void loop() {
    for (int i = 0; i < 3; i++) {
        int buttonState = digitalRead(pushButtonPins[i]);
        if (buttonState == LOW && !buttonIsPressed[i]) {
            buttonIsPressed[i] = true;
            buttonPressedTime[i] = millis();
            lastUpdate = millis(); // Reset the timer for LCD update
        } else if (buttonState == LOW && buttonIsPressed[i]) {
            // Update the LCD every 500 ms
            if (millis() - lastUpdate > 500) {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Button ");
                lcd.print(i + 1);
                lcd.print(": ");
                lcd.print((millis() - buttonPressedTime[i]) / 1000);
                lcd.print(" sec");
                lastUpdate = millis();
            }
        } else if (buttonState == HIGH && buttonIsPressed[i]) {
            buttonIsPressed[i] = false;
            unsigned long duration = millis() - buttonPressedTime[i];
            evaluatePressDuration(i, duration);
        }
    }
}

void evaluatePressDuration(int buttonIndex, unsigned long duration) {
    int pressType;
    if (duration >= 2000 && duration <= 4000) { // Apăsare lungă de 3 secunde
        pressType = 1; // Indice pentru nota lungă
    } else if (duration >= 5000 && duration <= 7000) {
        pressType = 2;
    } else {
        pressType = 0; // Indice pentru nota scurtă
    }
    playTone(buttonIndex, pressType);
}

void playTone(int buttonIndex, int pressType) {
    int frequency = frequencies[buttonIndex][pressType];
    tone(buzzer, frequency, 500); // Redă tonul pentru 500 ms
    lcd.clear();
    lcd.setCursor(0, 0);
    if ((pressType == 0) && (buttonIndex == 0)) {
        lcd.print("nota DO1 ");
    } else if ((pressType == 0) && (buttonIndex == 1)) {
        lcd.print("nota RE ");
    } else if ((pressType == 0) && (buttonIndex == 2)) {
        lcd.print("nota MI ");
    } else if ((pressType == 1) && (buttonIndex == 0)) {
        lcd.print("nota FA ");
    } else if ((pressType == 1) && (buttonIndex == 1)) {
        lcd.print("nota SOL ");
    } else if ((pressType == 1) && (buttonIndex == 2)) {
        lcd.print("nota LA ");
    } else if ((pressType == 2) && (buttonIndex == 0)) {
      lcd.print("nota SI ");
    } else if ((pressType == 2) && (buttonIndex == 1)) {
      lcd.print("nota DO2 ");
    } else if ((pressType == 2) && (buttonIndex == 2)) {
      lcd.print("Playing music!");
      playMelodyFromSD();
    }
    delay(500); // O pauză scurtă după redarea tonului
    noTone(buzzer);
}













