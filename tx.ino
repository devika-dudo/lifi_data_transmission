#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

LiquidCrystal lcd(4, 5, 6, 7, 8, 9);
SoftwareSerial GSerial(11, 12);

int arr_count = 0, i;
unsigned char arr[1027];
char cmd_arr2[100];
int rec_flag1 = 0;
int rec_flag2 = 0;
int buttonPin1 = 10;
int buttonPin2 = 3;
int lastButtonState1 = HIGH;
int lastButtonState2 = HIGH;

void serial_get_command() {
  int cmd_count = 0;
  char inchar = 0;
  char cmd_arr[30];
  
  if (Serial.available() > 0) {
    inchar = Serial.read();
    
    // ========== RECEIVE IMAGE DATA {xHEX} ==========
    if (inchar == '{') {
      arr[0] = 128; 
      arr[1] = 64;
      arr_count = 2;
      
      while (inchar != '}') {
        if (Serial.available() > 0) {
          inchar = Serial.read();
          if (inchar == 'x' || inchar == 'X') {
            while (Serial.available() == 0);
            cmd_arr[2] = Serial.read();
            while (Serial.available() == 0);
            cmd_arr[3] = Serial.read();
            cmd_arr[0] = '0';
            cmd_arr[1] = 'x';
            cmd_arr[4] = '\0';
            uint8_t intVal;
            sscanf(cmd_arr, "%x", &intVal);
            arr[arr_count++] = intVal;
            arr[arr_count] = '\0';
          }
        }
      }
      
      if (inchar == '}') {
        Serial.print(F("✓ Image loaded: "));
        Serial.print(arr_count);
        Serial.println(F(" bytes. Press BTN1 to send."));
        lcd.clear();
        lcd.print(F("Image Ready!"));
        lcd.setCursor(0, 1);
        lcd.print(F("BTN1 to send"));
        rec_flag1 = 1;  // ✅ Set flag (allows new data to overwrite)
      }
    }
    
    // ========== RECEIVE TEXT DATA <text> ==========
    else if (inchar == '<') {  // ✅ Changed to else if to prevent conflicts
      cmd_count = 0;
      while (inchar != '>' && cmd_count < 200) {
        if (Serial.available() > 0) {
          inchar = Serial.read();
          cmd_arr2[cmd_count++] = inchar;
          cmd_arr2[cmd_count] = '\0';
        }
      }
      
      if (inchar == '>') {
        cmd_arr2[cmd_count - 1] = '\0';  // Remove '>'
        Serial.print(F("✓ Text loaded: \""));
        Serial.print(cmd_arr2);
        Serial.println(F("\". Press BTN2 to send."));
        lcd.clear();
        lcd.print(F("Text Ready!"));
        lcd.setCursor(0, 1);
        lcd.print(cmd_arr2);
        delay(1500);
        lcd.clear();
        lcd.print(F("BTN2 to send"));
        lcd.setCursor(0, 1);
        lcd.print(cmd_arr2);
        rec_flag2 = 1;  // ✅ Set flag (allows new data to overwrite)
      }
    }
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println(F("========================================"));
  Serial.println(F("       LiFi Transmitter Ready"));
  Serial.println(F("========================================"));
  Serial.println(F("Send text: <your message here>"));
  Serial.println(F("Send image: {x80x40xFF...}"));
  Serial.println(F("Then press button to transmit!"));
  Serial.println(F("========================================"));
  Serial.println();
  
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  
  delay(100);
  
  GSerial.begin(2400);
  
  lcd.begin(16, 2);
  lcd.print(F("LiFi TX Ready"));
  delay(2000);
  lcd.clear();
  lcd.print(F("Waiting for"));
  lcd.setCursor(0, 1);
  lcd.print(F("data..."));
}

void loop() {
  serial_get_command();  // ✅ Always checking for new data!
  
  // ========== SEND IMAGE DATA ==========
  if (rec_flag1 == 1) {
    int buttonState1 = digitalRead(buttonPin1);
    
    if (buttonState1 == LOW && lastButtonState1 == HIGH) {
      delay(50);
      
      lcd.clear();
      lcd.print(F("Transmitting..."));
      lcd.setCursor(0, 1);
      lcd.print(F("Image"));
      
      Serial.println(F("→ Transmitting image via LiFi..."));
      
      GSerial.print(F("<"));
      for (i = 0; i < arr_count; i++) {
        GSerial.write(arr[i]);
      }
      GSerial.print(F(">"));
      
      lcd.clear();
      lcd.print(F("Sent!"));
      lcd.setCursor(0, 1);
      lcd.print(F("Ready for more"));
      
      Serial.println(F("✓ Image transmitted!"));
      Serial.println(F("→ Send new data or press BTN1 to resend."));
      Serial.println();
      
      delay(1500);
      lcd.clear();
      lcd.print(F("BTN1=Resend"));
      lcd.setCursor(0, 1);
      lcd.print(F("Or send new"));
      
      // ✅ Don't clear flag - allows both resend AND new data
      
      while (digitalRead(buttonPin1) == LOW) {
        delay(10);
      }
    }
    
    lastButtonState1 = buttonState1;
  }
  
  // ========== SEND TEXT DATA ==========
  if (rec_flag2 == 1) {
    int buttonState2 = digitalRead(buttonPin2);
    
    if (buttonState2 == LOW && lastButtonState2 == HIGH) {
      delay(50);
      
      lcd.clear();
      lcd.print(F("Transmitting..."));
      lcd.setCursor(0, 1);
      lcd.print(cmd_arr2);
      
      Serial.print(F("→ Transmitting: \""));
      Serial.print(cmd_arr2);
      Serial.println(F("\""));
      
      GSerial.print(F("{"));
      GSerial.print(cmd_arr2);
      GSerial.print(F("}"));
      
      lcd.clear();
      lcd.print(F("Sent!"));
      lcd.setCursor(0, 1);
      lcd.print(F("Ready for more"));
      
      Serial.println(F("✓ Text transmitted!"));
      Serial.println(F("→ Send new data or press BTN2 to resend."));
      Serial.println();
      
      delay(1500);
      lcd.clear();
      lcd.print(F("BTN2=Resend"));
      lcd.setCursor(0, 1);
      lcd.print(F("Or send new"));
      
      // ✅ Don't clear flag - allows both resend AND new data
      
      while (digitalRead(buttonPin2) == LOW) {
        delay(10);
      }
    }
    
    lastButtonState2 = buttonState2;
  }
}
