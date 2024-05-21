#include <Wire.h>
#include <Keypad.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#define SS_PIN 21
#define RST_PIN 22
#define I2C_SDA_PIN 0
#define I2C_SCL_PIN 4

#define PIN_RED 17    // GPIO17
#define PIN_GREEN 16  // GPIO16

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int buzzerPin = 5;  // Define the pin connected to the piezo buzzer
const byte ROWS = 4;      // Four rows
const byte COLS = 4;      // Four columns

char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

// byte rowPins[ROWS] = { 12, 13, 14, 15 };  //connect to the row pinouts of the keypad
// byte colPins[COLS] = { 16, 17, 18, 19 };  //connect to the column pinouts of the keypad

byte rowPins[ROWS] = { 13, 12, 14, 27 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = { 26, 25, 33, 32 };  //connect to the column pinouts of the keypad


Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const char* correctPassword = "1234";  // Predefined correct password

void setup() {
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);

  pinMode(buzzerPin, OUTPUT);  // Set the buzzer pin as an output
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  SPI.begin();         // Initiate  SPI bus
  mfrc522.PCD_Init();  // Initiate MFRC522
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  Serial.begin(9600);
  digitalWrite(PIN_RED,HIGH);
}

void loop() {
  // Wire.beginTransmission(deviceAddress);
  // Wire.write(data);
  // Wire.endTransmission();

  char enteredPassword[5];  // Buffer to store entered password
  int passwordIndex = 0;    // Index for the entered password

  // Serial.println("Enter 4-digit password:");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Password");

  while (passwordIndex < 4) {
    char key = keypad.getKey();
    if (key) {
      //Serial.print(key);
      tone(buzzerPin, 300);
      delay(100);
      noTone(buzzerPin);  // Turn off the tone

      lcd.setCursor(passwordIndex, 1);
      lcd.print("*");
      enteredPassword[passwordIndex] = key;
      passwordIndex++;
      delay(100);  // Delay for debouncing
    }
  }


  enteredPassword[4] = '\0';  // Null-terminate the entered password string

  Serial.println();

  if (strcmp(enteredPassword, correctPassword) == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Password Correct");
    lcd.setCursor(0, 1);
    lcd.print("Waiting For RFID");
    // Serial.println("Password correct. Door unlocked.");

    delay(2000);
    if (!mfrc522.PICC_IsNewCardPresent()) {
      return;
    }
    // Select one of the cards
    if (!mfrc522.PICC_ReadCardSerial()) {
      return;
    }
    String content = "";
    byte letter;
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      // Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      // Serial.print(mfrc522.uid.uidByte[i], HEX);
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }

    content.toUpperCase();
    if (content.substring(1) == "59 3E C5 A3")  //change here the UID of the card/cards that you want to give access
    {
      digitalWrite(PIN_RED,LOW);
      digitalWrite(PIN_GREEN,HIGH);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("DOOR UNLOCKED!!!");
      // Serial.println("Authorized access");
      // Serial.println();
      Serial.print("1");
      tone(buzzerPin, 700);
      delay(1000);
      noTone(buzzerPin);
      delay(4000);
      digitalWrite(PIN_GREEN,LOW);
      digitalWrite(PIN_RED,HIGH);

    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Access denied");
      tone(buzzerPin, 500);
      delay(200);
      noTone(buzzerPin);
      delay(200);
      tone(buzzerPin, 500);
      delay(200);
      noTone(buzzerPin);
      delay(1000);
      // Serial.println(" Access denied");
    }
    // Add your unlocking mechanism here
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Password");
    lcd.setCursor(0, 1);
    lcd.print("Incorrect!!!");
    // Serial.println("Password incorrect. Please try again.");
    Serial.print("0");
    tone(buzzerPin, 500);
    delay(200);
    noTone(buzzerPin);
    delay(200);
    tone(buzzerPin, 500);
    delay(200);
    noTone(buzzerPin);
    // delay(2000);
  }
}
