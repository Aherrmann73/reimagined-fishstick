#include <LiquidCrystal.h> // includes the LiquidCrystal Library 
#include <Keypad.h>
//LCD config
LiquidCrystal lcd( 7, 8, 9, 10, 11, 12); // Creates an LCD object. Parameters: (rs, enable, d4, d5, d6, d7)
//Joystick config
const int SW_pin = 6; // digital pin connected to switch output
const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output
int joySw = 1;
//Page
int pageInt = 0;
int keyDisp = 0;
//LED config
const int red = 4;
const int grn = 5;
//Master password
String masPas = "95539553";
String pass = "";
//Password entry status
int masPasStat = 0;
//Keypad config
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the symbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {39, 41, 43, 45}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {47, 49, 51, 53}; //connect to the column pinouts of the keypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
int keyEntry = 0;
//Target Coordinates config
String tgtCo0 = "";
String tgtCo1 = "";
int tC0Len = 0;
int tC1Len = 0;
int tgtComLen = tC0Len + tC1Len;
int tgtFlag = 0;
//Disarm config
char canKey = 'C';
char disKey = 'D';
//Tgt Clear config
char noKey = 'A';
char yesKey = 'B';
//Fire Control config
char fireKey = '#';


void setup() {
  Serial.begin(9600);                                           //DEBUG ONLY REMOVE UPON COMPLETION
  //LCD init
  analogWrite(8, 28); // Generate PWM signal at pin D11, value of 100 (out of 255) (Contrast)
  lcd.begin(16, 2); // Initializes the interface to the LCD screen, and specifies the dimensions (width and height) of the display }
  //Joystick switch config
  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH);
  //LED init
  pinMode(red, OUTPUT);
  pinMode(grn, OUTPUT);
  passChk();
  pageDisp();
  ledChk();
}

void pageNum(int y) {
  if (pageInt < 0) {
    pageInt = 0;
    updateDisp();
  } else if (pageInt > 3) {
    pageInt = 3;
    updateDisp();
  } else if (y > 1000) {
    if (pageInt < 3) {
      pageInt += 1;
      updateDisp();
      delay(150);
    }
  } else if (y < 30) {
    if (pageInt > 0) {
      pageInt -= 1;
      updateDisp();
      delay(150);
    }
  }
}

void pageDisp() {
  switch (pageInt) {
    case 0:
      lcd.clear();
      lcd.print("MISSLE STATUS: ");
      if (masPasStat == 1) {
        lcd.setCursor(0, 1);
        lcd.print("ARMED");
      } else {
        lcd.setCursor(0, 1);
        lcd.print("DISARMED");
      }
      break;
    case 1:
      lcd.clear();
      lcd.print("MISSLE TYPE:");
      lcd.setCursor(0, 1);
      lcd.print("LGM-30");              //Could add code to be set/changed during "Installation and setup"
      break;
    case 2:
      lcd.clear();
      lcd.print("TGT COORDINATES:");
      if (masPasStat == 0) {
        lcd.setCursor(0, 1);
        lcd.print("SYSTEM LOCKED");
      } else if (tgtFlag == 1) {
        lcd.setCursor(0, 1);
        lcd.print(tgtCo0);
        lcd.print(" ");
        lcd.print(tgtCo1);
      }
      break;
    case 3:
      lcd.clear();
      lcd.print("      FIRE");
      lcd.setCursor(0, 1);
      lcd.print("     CONTROL");
      break;
  }
}

void ledChk() {
  if (masPasStat == 1) {
    digitalWrite(grn, HIGH);
    digitalWrite(red, LOW);
  } else {
    digitalWrite(grn, LOW);
    digitalWrite(red, HIGH);
  }
}

void passChk() {
  unsigned long currentMillis = millis();            //sets time since start for disarm function
  if (pass == masPas) {
    masPasStat = 1;
  } else {
    masPasStat = 0;
  }
}

void updateDisp() {
  lcd.clear();
  pageDisp();
  ledChk();
}

void keyEnt() {
  do {
    char customKey = 0;
    customKey = customKeypad.getKey();
    if (customKey != 0) {
      pass.concat(customKey);
      lcd.print(customKey);
    }
  } while (pass.length() != 8);
  if (pass != masPas) {
    lcd.clear();
    lcd.print("INCORRECT");
    lcd.setCursor(0, 1);
    lcd.print("CODE");
    delay(1500);
    pass.remove(0, 8);
    updateDisp();
  } else if (pass == masPas) {
    passChk();
    updateDisp();
  }
}

void tgtEnt() {
  lcd.clear();
  lcd.print("ENTER TGT COORD:");
  lcd.setCursor(0, 1);
  while (tgtComLen != 8) {
    tC0Len = tgtCo0.length();
    tC1Len = tgtCo1.length();
    tgtComLen = tC0Len + tC1Len;
    Serial.print(tC0Len);
    Serial.println(tgtCo0);
    char tgtKey = 0;
    tgtKey = customKeypad.getKey();
    if (tgtKey != 0 && tC0Len <= 3) {
      tgtCo0.concat(tgtKey);
      lcd.print(tgtKey);
    } else if (tgtKey != 0 && tC1Len <= 4) {
      tgtCo1.concat(tgtKey);
      lcd.print(tgtKey);
    }
  }
  tgtFlag = 1;
  lcd.clear();
  updateDisp();
}

void tgtClear() {
  tgtCo0 = "";
  tgtCo1 = "";
  tC0Len = 0;
  tC1Len = 0;
  tgtComLen = 0;
  tgtFlag = 0;
}

void launch() {
  lcd.clear();
  pass.remove(0, 8);
  passChk();
  tgtClear();
  pageInt = 0;
  updateDisp();
}

void enterKeyInput() {
  joySw = digitalRead(SW_pin);
  if (joySw == 0 && pageInt == 0 && masPasStat == 0) {
    lcd.clear();
    lcd.print("ENTER CODE:");
    lcd.setCursor(0, 1);
    keyEnt();
  } else if (joySw == 0 && pageInt == 2 && masPasStat == 1) {
    if (tgtComLen != 8) {
      tgtEnt();
    } else {
      lcd.clear();
      lcd.print("CLEAR TGT ?");
      lcd.setCursor(0, 1);
      lcd.print("A: NO B: YES");
      char keyVar = 0;
      keyVar = customKeypad.getKey();
      while (keyVar != noKey || keyVar != yesKey) {
        keyVar = customKeypad.getKey();
        if (keyVar == noKey) {        //Cancel
          updateDisp();
          break;
        } else if (keyVar == yesKey) {             //Clear tgt
          tgtClear();
          updateDisp();
          break;
        }
      }
    }
  } else if (joySw == 0 && pageInt == 0 && masPasStat == 1) {
    lcd.clear();
    lcd.print("DISARM: PRESS D");
    lcd.setCursor(0, 1);
    lcd.print("CANCEL: PRESS C");
    char keyVar = 0;
    keyVar = customKeypad.getKey();
    while (keyVar != canKey || keyVar != disKey) {
      keyVar = customKeypad.getKey();
      if (keyVar == canKey) {        //Cancel
        updateDisp();
        break;
      } else if (keyVar == disKey) {             //Disarm
        lcd.clear();
        pass.remove(0, 8);
        passChk();
        tgtClear();
        updateDisp();
        break;
      }
    }
  } else if (joySw == 0 && pageInt == 3 && masPasStat == 1) {
    lcd.clear();
    lcd.print("FIRE: # 5 TIMES");
    lcd.setCursor(0, 1);
    lcd.print("CANCEL: C");
    char keyVar = 0;
    keyVar = customKeypad.getKey();
    while (keyVar != canKey || keyVar != fireKey) {
      keyVar = customKeypad.getKey();
      if (keyVar == canKey) {        //Cancel
        updateDisp();
        break;
      } else if (keyVar == fireKey) {             //Fire
        int secCoun = 0;
        while (secCoun != 5) {
          keyVar = customKeypad.getKey();
          if (keyVar == '#') {
            delay(50);
            secCoun += 1;
          } else if (keyVar == canKey) {
            secCoun = 0;
            lcd.clear();
            updateDisp();
            break;
          }
        }
        if (secCoun == 5) {
          launch();
        }
        break;
      }
    }
  }
}

void serialDebug() {                        //turn on and off vars    REMOVE UPON COMPLETION
  //char testVar = customKeypad.getKey();
  //Serial.println(customKeypad.getKey());
  //Serial.println(joySw);
  //Serial.println(analogRead(Y_pin));
  //Serial.println(pageInt);
  //Serial.println(pass);
  //Serial.println(testVar);

}

void loop() {
  passChk();
  pageNum(analogRead(Y_pin));
  enterKeyInput();
  serialDebug();                                              //DEBUG REMOVE AT COMPLETION
  delay(50);
}
