/*
 * Ketturi Electronics AutoDRAIN
 * Automatic presurized air system water draining timer.
 */
#include <LiquidCrystal.h>
#include <EEPROM.h>

LiquidCrystal lcd(6, 7, 5, 4, 3, 2); //LCD connections

const int buttonPinUp = 10; //UP/Manual drain2 button
const int buttonPinSel = 9; //Select/Menu button
const int buttonPinDn = 8;  //DOWN/Manual drain1 button
const int relayPin1 = 16;   //Relay control for drain valve 1
const int relayPin2 = 14;   //Relay control for drain valve 2

int buttonStateSel = 0;
int buttonStateUp = 0;
int buttonStateDn = 0;

boolean menu = false;                     //menu system
unsigned int menuState = 0;

unsigned int  drain1delay = 15;           //drain duration, seconds/10
unsigned int  drain2delay = 15;
float drain1delaydisplay = static_cast<float>(drain1delay) / 10.0f; //seconds/10 to desimal seconds
float drain2delaydisplay = static_cast<float>(drain2delay) / 10.0f;

unsigned int  drain1interval = 60;        //interval between drains, minutes
unsigned int  drain2interval = 60;

unsigned long drain1cntdwn = drain1interval; //countdown timers for automatic drain
unsigned long drain2cntdwn = drain2interval;

unsigned long previousMillis = 0;
unsigned long seconds = 1;

void setup() { //setup on boot
  //Set pin directions
  pinMode(buttonPinUp, INPUT);
  pinMode(buttonPinSel, INPUT);
  pinMode(buttonPinDn, INPUT);
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  digitalWrite(relayPin1, LOW);
  digitalWrite(relayPin2, LOW);

  lcd.begin(20, 2); //initialize lcd library, 20x2 HD44780 display

  if (digitalRead(buttonPinSel) == HIGH) { //resets all setting if menu button is held on power up
    resetEEPROM();
  }

  //load settings from eeprom
  drain1interval = EEPROM.read(0);
  drain1delay = EEPROM.read(1);
  drain2interval = EEPROM.read(2);
  drain2delay = EEPROM.read(3);
  drain1cntdwn = drain1interval;
  drain2cntdwn = drain2interval;

  //Show model and firmware version
  lcd.setCursor(0, 0);
  lcd.print("Ketturi Electronics ");
  lcd.setCursor(0, 1);
  lcd.print("AutoDRAIN v0.4      ");
  delay(5000);
  lcd.clear();
}

void loop() {
  keepTime(); //does timing for automatid drain

  // If menu/select button is pressed, run menu
  buttonStateSel = digitalRead(buttonPinSel);
  if ( buttonStateSel == HIGH) {
    menu = true; //sets menu active
    menuf();    //run menu function
  }

  //Manual drain 1 when down button is pressed
  while (digitalRead(buttonPinDn) == HIGH) {
    digitalWrite(relayPin1, LOW);
    lcd.setCursor(0, 0);
    lcd.print("Manual Drain 1 act. ");
    digitalWrite(relayPin1, HIGH);
    delay(drain1delay * 100);
  }

  //Manual drain 2 when up button is pressed
  while (digitalRead(buttonPinUp) == HIGH) {
    lcd.setCursor(0, 1);
    lcd.print("Manual Drain 2 act. ");
    digitalWrite(relayPin2, HIGH);
    delay(drain2delay * 100);
  }

  //Main display, shows time left to next drain
  lcd.setCursor(0, 0);
  lcd.print("Next drain 1:    min");
  lcd.setCursor(14, 0);
  lcd.print(drain1cntdwn);

  lcd.setCursor(0, 1);
  lcd.print("Next drain 2:    min");
  lcd.setCursor(14, 1);
  lcd.print(drain2cntdwn);
  digitalWrite(relayPin1, LOW); //set relays low
  digitalWrite(relayPin2, LOW);
}

void menuf() { //menu function, sets intervals and durations
  lcd.clear();
  lcd.print("AutoDrain settings");
  lcd.setCursor(0, 1);
  lcd.print("menu");

  while (digitalRead(buttonPinSel) == HIGH) { //switch debounce
    delay(1);
  }
  delay(2000);
  while (menu == true) { //runs as long as menu is true
    switch (menuState) {

      case 0: //Drain 1 interval
        lcd.setCursor(0, 0);
        lcd.print("Set Drain 1 interval");

        if ((digitalRead(buttonPinDn) == HIGH) && (drain1interval > 5)) //substract time
          drain1interval = drain1interval - 5;
        if ((digitalRead(buttonPinUp) == HIGH) && (drain1interval < 255)) //add time
          drain1interval = drain1interval + 5;

        EEPROM.write(0, drain1interval);
        drain1cntdwn = drain1interval;
        lcd.setCursor(0, 1);
        lcd.print(drain1interval);
        lcd.print("min               ");
        delay(150); //Debounce
        break;

      case 1: //Drain 1 duration
        lcd.setCursor(0, 0);
        lcd.print("Set Drain 1 duration");

        if ((digitalRead(buttonPinDn) == HIGH) && (drain1delay > 1))
          drain1delay = drain1delay - 1;
        if ((digitalRead(buttonPinUp) == HIGH) && (drain1delay < 255))
          drain1delay = drain1delay + 1;

        EEPROM.write(1, drain1delay);
        drain1delaydisplay = static_cast<float>(drain1delay) / 10.0f; //convert seconds/10 to desimal seconds
        lcd.setCursor(0, 1);
        lcd.print(drain1delaydisplay);
        lcd.print("sec               ");
        delay(150);
        break;

      case 2: //Drain 2 interval
        lcd.setCursor(0, 0);
        lcd.print("Set drain 2 interval");

        if ((digitalRead(buttonPinDn) == HIGH) && (drain2interval > 5))
          drain2interval = drain2interval - 5;
        if ((digitalRead(buttonPinUp) == HIGH) && (drain2interval < 255))
          drain2interval = drain2interval + 5;

        EEPROM.write(2, drain2interval);
        drain2cntdwn = drain2interval;
        lcd.setCursor(0, 1);
        lcd.print(drain2interval);
        lcd.print("min               ");
        delay(150);
        break;

      case 3: //Drain 2 duration
        lcd.setCursor(0, 0);
        lcd.print("Set Drain 2 duration");

        if ((digitalRead(buttonPinDn) == HIGH) && (drain2delay > 1))
          drain2delay = drain2delay - 1;
        if ((digitalRead(buttonPinUp) == HIGH) && (drain2delay < 255))
          drain2delay = drain2delay + 1;

        EEPROM.write(3, drain2delay);
        drain2delaydisplay = static_cast<float>(drain2delay) / 10.0f;
        lcd.setCursor(0, 1);
        lcd.print(drain2delaydisplay); lcd.print("sec               ");
        delay(150);
        break;
    }

    //Switch trough setting pages with select button, and then exit
    buttonStateSel = digitalRead(buttonPinSel);
    if (buttonStateSel == HIGH) {
      if (menuState <= 3) {
        menuState = menuState + 1;
      }
      if (menuState > 3) {
        menuState = 0;
        menu = false;
      }
      while (digitalRead(buttonPinSel) == HIGH) { //Debounce
        delay(1);
      }
    }
  }
}

//Timer for automatic drain, counts minutes to next drain
void keepTime() {

  unsigned long currentMillis = millis (); //get heartbeat from millis counter

  if (currentMillis - previousMillis >= 60000) { //Runs every minute, substracts time
    previousMillis = currentMillis;
    drain1cntdwn = drain1cntdwn - 1;
    drain2cntdwn = drain2cntdwn - 1;
  }

  if (drain1cntdwn == 0) { //Activate drain 1 when countdown finished
    drain1cntdwn = drain1interval;
    digitalWrite(relayPin1, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("Drain valve 1 active");
    delay(drain1delay * 100);
    digitalWrite(relayPin1, LOW);
  }
  if (drain2cntdwn == 0) { //Activate drain 2 when countdown finished
    drain2cntdwn = drain2interval;
    digitalWrite(relayPin2, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("Drain valve 2 active");
    delay(drain2delay * 100);
    digitalWrite(relayPin2, LOW);
  }
}

//Reset eeprom function, fills eeprom with zeros
void resetEEPROM() {
  lcd.setCursor(0, 0);
  lcd.print("Clearing settings  ");
  lcd.setCursor(0, 1);

  for ( int i = 0 ; i < EEPROM.length() ; i++ ) { //fill whole eeprom with zeros, show nice bargraph on lcd
    EEPROM.write(i, 0);
    if ((i % 51) == 0)
      lcd.print(".");
  }

  //load default settings to eeprom
  EEPROM.write(0, drain1interval);
  EEPROM.write(1, drain1delay);
  EEPROM.write(2, drain2interval);
  EEPROM.write(3, drain2delay);

  lcd.setCursor(0, 0);
  lcd.print("Settings cleared   ");
  delay(1000);
}

