#include <Keypad.h>           
#include <LiquidCrystal.h>    
#include <SoftwareSerial.h>

#define tx 12  //tx wire to pin 13
#define rx 13  //rx wire to pin 12
SoftwareSerial configBt(rx, tx);  

// Keypad setup
const byte ROWS = 4, COLS = 4;  
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {11, 10, 9, 8};
byte colPins[COLS] = {A0, A1, A2, A3};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// LCD setup
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

// multi-tap  outgoing buffer
char lastKey = NO_KEY; //checking which key is being cycled
int cycleIndex = 0;  // position in pressed key's letter list       
unsigned long pressTime  = 0; // key first pressed    
String buffer = "";     

// incoming-SMS state
String rxBuffer = ""; 
unsigned long rxTime = 0; //timestamp when msg completed
bool showingRx = false; // initial state: we are not displaying a SMS yet

void setup() {
  lcd.begin(16, 2);
  lcd.print("Hold a key...");
  configBt.begin(9600);    // hc-05 baud
}

void loop() {
  // incoming bluetooth data
  if (configBt.available()) {
    char ch = configBt.read(); //if HC-05 sent data, read one char
    if (ch == '\n' || ch == '\r') {
      // when line completed, display it
      if (rxBuffer.length() > 0) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Text Received"); // per spec
        lcd.setCursor(0, 1);
        lcd.print(rxBuffer);
        rxTime = millis(); //records timestamp
        showingRx = true;
        rxBuffer = ""; // empties it
      }
    }
    else {
      rxBuffer += ch;
    }
  }

  // after 10 s of showing the received SMS, return to input mode
  if (showingRx && millis() - rxTime >= 10000) {
    showingRx = false;
    lcd.clear();
    lcd.print("Hold a key...");
    buffer = "";
  }

  // when showing an incoming text, skip keypad input
  if (showingRx) return;

  // multi-tap keypad + send logic
  keypad.getKeys();

  for (int i = 0; i < LIST_MAX; i++) {
    if (keypad.key[i].kstate == PRESSED) {
      lastKey = keypad.key[i].kchar;
      cycleIndex = 0;
      pressTime = millis();
      showChar();
    }
    else if (keypad.key[i].kstate == HOLD && keypad.key[i].kchar == lastKey) {
      int len = charCount(lastKey);
      int idx = ((millis() - pressTime) / 1000) % len;
      if (idx != cycleIndex) {
        cycleIndex = idx;
        showChar();
      }
    }
    else if (keypad.key[i].kstate == RELEASED && keypad.key[i].kchar == lastKey) {

      // send when on '#'
      if (lastKey == '#') {
        if (buffer.length() > 0) {
          configBt.println(buffer);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Message Sent");
          buffer = "";
          delay(2000);
        }
        lcd.clear();
        lcd.print("Hold a key...");
      }
      // clear on '*'
      else if (lastKey == '*') {
        buffer = "";
        updateDisplay();
      }
      // append any other character
      else {
        char c = getChar(lastKey, cycleIndex);
        buffer += c;
        updateDisplay();
      }

      lastKey = NO_KEY;
      delay(200);
    }
  }
}

// draw only the currently cycling char on first line, skip '*'
void showChar() {
  char c = getChar(lastKey, cycleIndex);
  lcd.setCursor(0, 0);
  lcd.print("                ");
  if (c != '*') {
    lcd.setCursor(0, 0);
    lcd.print(c);
  }
}

// redraw the full buffer on second line
void updateDisplay() {
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(buffer);
}

// number of symbols per key
int charCount(char k) {
  switch (k) {
    case '1': return 1;   case '2': return 4;
    case '3': return 4;   case '4': return 4;
    case '5': return 4;   case '6': return 4;
    case '7': return 4;   case '8': return 4;
    case '9': return 4;   case '0': return 5; // leading space + OPER
    case '*': return 1;   case '#': return 1;
    default:  return 1;
  }
}

// map (key, index) to actual character
char getChar(char k, int idx) {
  switch (k) {
    case '1': { const char*s="1";      return s[idx]; }
    case '2': { const char*s="2ABC";   return s[idx]; }
    case '3': { const char*s="3DEF";   return s[idx]; }
    case '4': { const char*s="4GHI";   return s[idx]; }
    case '5': { const char*s="5JKL";   return s[idx]; }
    case '6': { const char*s="6MNO";   return s[idx]; }
    case '7': { const char*s="7PRS";   return s[idx]; }
    case '8': { const char*s="8TUV";   return s[idx]; }
    case '9': { const char*s="9WXY";   return s[idx]; }
    case '0': { const char*s="0OPER";  return s[idx]; }
    default:   return k;  // '*', '#', A–D
  }
}
