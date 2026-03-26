#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

const int sensorPin = 2;   // DO pin of LDR module

const int UNIT = 100;   // app unit duration

bool lightState = false;
bool lastState = false;

unsigned long stateStart = 0;
unsigned long duration = 0;

String morse = "";
String message = "";

char decode(String code)
{
  if(code==".-") return 'A';
  if(code=="-...") return 'B';
  if(code=="-.-.") return 'C';
  if(code=="-..") return 'D';
  if(code==".") return 'E';
  if(code=="..-.") return 'F';
  if(code=="--.") return 'G';
  if(code=="....") return 'H';
  if(code=="..") return 'I';
  if(code==".---") return 'J';
  if(code=="-.-") return 'K';
  if(code==".-..") return 'L';
  if(code=="--") return 'M';
  if(code=="-.") return 'N';
  if(code=="---") return 'O';
  if(code==".--.") return 'P';
  if(code=="--.-") return 'Q';
  if(code==".-.") return 'R';
  if(code=="...") return 'S';
  if(code=="-") return 'T';
  if(code=="..-") return 'U';
  if(code=="...-") return 'V';
  if(code==".--") return 'W';
  if(code=="-..-") return 'X';
  if(code=="-.--") return 'Y';
  if(code=="--..") return 'Z';
  return '?';
}

void setup()
{
  Serial.begin(9600);

  pinMode(sensorPin, INPUT);

  lcd.init();
  lcd.backlight();

  lcd.print("LiFi Ready");
  delay(2000);
  lcd.clear();

  stateStart = millis();
}

void loop()
{
  int val = digitalRead(sensorPin);

  lightState = (val == LOW);   // light detected

  if(lightState != lastState)
  {
    duration = millis() - stateStart;
    stateStart = millis();

    if(lastState == true)
    {
      // LIGHT ON ended → dot or dash
      if(duration < UNIT*2)
      morse += ".";
      else
      morse += "-";

      Serial.println(morse);
    }
    else
    {
      // DARK period ended → gap detection
      if(duration > UNIT*3)
      {
        char letter = decode(morse);
        message += letter;

        lcd.clear();
        lcd.print(message);

        Serial.print("Letter: ");
        Serial.println(letter);

        morse = "";
      }

      if(duration > UNIT*7)
      {
        message += " ";
      }
    }

    lastState = lightState;
  }
}
