#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

LiquidCrystal lcd(4, 5, 6, 7, 8, 9);
SoftwareSerial GSerial(2, 3); // RX=2, TX=3

unsigned char cmd_arr[1050];
char cmd_arr2[100];
int cmd_count = 0;
char inchar;
int ledPin = 13;

void serial_get_command() {
  
  if (GSerial.available() > 0) {
    inchar = GSerial.read();
    
    // ========== RECEIVE IMAGE DATA <...> ==========
    if (inchar == '<') {
      digitalWrite(ledPin, HIGH);
      lcd.clear();
      lcd.print(F("Receiving Img..."));
      Serial.println(F("Receiving image..."));
      cmd_count = 0;
      
      // Read until '>' or buffer full
      while (inchar != '>' && cmd_count < 1048) {
        if (GSerial.available() > 0) {
          inchar = GSerial.read();
          if (inchar != '>') {
            cmd_arr[cmd_count++] = inchar;
          }
        }
      }
      
      if (inchar == '>') {
        lcd.clear();
        lcd.print(F("Image Received!"));
        lcd.setCursor(0, 1);
        lcd.print(F("Bytes: "));
        lcd.print(cmd_count);
        
        Serial.print(F("Image Data ("));
        Serial.print(cmd_count);
        Serial.print(F(" bytes): "));
        for (int i = 0; i < cmd_count; i++) {
          Serial.print(cmd_arr[i]);
          Serial.print(F(" "));
        }
        Serial.println();
        
        delay(3000);
        lcd.clear();
        lcd.print(F("Ready"));
      } else {
        Serial.println(F("ERROR: Incomplete image"));
        lcd.clear();
        lcd.print(F("Error!"));
        delay(2000);
        lcd.clear();
        lcd.print(F("Ready"));
      }
      
      digitalWrite(ledPin, LOW);
      cmd_count = 0;
    }
    
    // ========== RECEIVE TEXT DATA {...} ==========
    else if (inchar == '{') {  // ✅ CHANGED TO 'else if'!
      digitalWrite(ledPin, HIGH);
      lcd.clear();
      lcd.print(F("Receiving Text.."));
      Serial.println(F("Receiving text..."));
      cmd_count = 0;
      
      // Read until '}' or buffer full
      while (inchar != '}' && cmd_count < 98) {
        if (GSerial.available() > 0) {
          inchar = GSerial.read();
          if (inchar != '}') {
            cmd_arr2[cmd_count++] = inchar;
          }
        }
      }
      
      cmd_arr2[cmd_count] = '\0';  // Null terminate
      
      if (inchar == '}') {
        lcd.clear();
        lcd.print(F("Text Received:"));
        lcd.setCursor(0, 1);
        lcd.print(cmd_arr2);  // Show text on LCD
        
        Serial.print(F("Text: "));
        Serial.println(cmd_arr2);
        
        delay(3000);
        lcd.clear();
        lcd.print(F("Ready"));
      } else {
        Serial.println(F("ERROR: Incomplete text"));
        lcd.clear();
        lcd.print(F("Error!"));
        delay(2000);
        lcd.clear();
        lcd.print(F("Ready"));
      }
      
      digitalWrite(ledPin, LOW);
      cmd_count = 0;
      cmd_arr2[0] = '\0';
    }
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("Receiver Started");
  
  pinMode(ledPin, OUTPUT);
  
  GSerial.begin(2400);  // Match transmitter baud rate!
  
  lcd.begin(16, 2);
  
  // ✅ Test LCD at startup
  lcd.clear();
  lcd.print(F("LCD Test"));
  lcd.setCursor(0, 1);
  lcd.print(F("1234567890"));
  delay(2000);
  
  lcd.clear();
  lcd.print(F("LiFi RX Ready"));
  delay(2000);
  
  lcd.clear();
  lcd.print(F("Ready"));
}

void loop() {
  serial_get_command();
}
