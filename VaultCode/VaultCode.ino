#include <LiquidCrystal.h>
#include <Servo.h>
#include <avr/sleep.h>

// LCD
LiquidCrystal lcd (12, 7, 11, 10, 9, 8);


// LED Pins
#define FirstGreenLedPin 4
#define SecondGreenLedPin 3
#define ThirdGreenLedPin 2
#define RedLedPin 5

// Input Pins
#define ServoPin 6
#define ButtonPin 13
#define ResetPin A1
#define DialInput A0
#define SleepButtonPin A2

// Vault Code
int FirstNumber = 5;
int SecondNumber = 1;
int ThirdNumber = 3;

// New code temp
int NFirstNumber;
int NSecondNumber;
int NThirdNumber;

// Vault input
int FirstNumberInput;
int SecondNumberInput;
int ThirdNumberInput;

// Input Values
int SensorValue;
int SensorNumber;

int ButtonState;
int ButtonResetState;

// Button press
unsigned long KeyPrevMillis = 0;
const unsigned long KeySampleIntervalMs = 25;
byte LongkeyPressCountMax = 200; //200 * 25= 5000 ms
byte LongKeyPressCount = 0;
byte PrevKeyState = HIGH;

// Change Code
bool Change = false;

// Master Variable
bool Accepted = false;

// Servo
Servo Servo1;

// Time Var
int Time = 0;
const int TimeLimit = 1800;

void setup() {
  //Pin Setup
  pinMode(ButtonPin, INPUT);
  pinMode(ResetPin, INPUT);
  pinMode(SleepButtonPin, INPUT);
  pinMode(FirstGreenLedPin, OUTPUT);
  pinMode(SecondGreenLedPin, OUTPUT);
  pinMode(ThirdGreenLedPin, OUTPUT);
  pinMode(RedLedPin, OUTPUT);
  pinMode(ServoPin, OUTPUT);

  //LCD Setup
  Serial.begin(9600);
  lcd.begin(16, 2);
  //Servo Setup
  Servo1.attach(ServoPin);
  Servo1.write(0);

  // LCD text
  lcd.print("Number:");
}

void loop() {
  // Set Input values
  SensorValue = analogRead(DialInput);
  SensorNumber = map(SensorValue, 0, 1023, 1, 9);
  ButtonState = digitalRead(ButtonPin);
  ButtonResetState = digitalRead(ResetPin);
  // Displays Number on lcd
  lcd.setCursor(0, 1);
  lcd.print(SensorNumber);

  if(millis() - KeyPrevMillis >= KeySampleIntervalMs) {
    KeyPrevMillis = millis();

    byte CurrKeyState = digitalRead(ButtonPin);

    if((PrevKeyState == HIGH) && (CurrKeyState == LOW)) {
      KeyPress();
    } else if((PrevKeyState == LOW) && (CurrKeyState == HIGH)) {
      KeyRelease();
    } else if (CurrKeyState == LOW) {
      LongKeyPressCount++;
    }

    PrevKeyState = CurrKeyState;
  }

  if (ButtonResetState == HIGH) {
    Reset();
  }

  Time++;
  if(Time == TimeLimit) {
    goToSleep();
  }
}

// Checks if Vault input is the same as Vault code
int check(int Input, int Number, int Result, int Led) {
  if (Input == Number) {
    digitalWrite(Led, HIGH);
    Result = Number;
  } else {
    Reset();
  }

  return Result;
}

// Reset Function

int Reset() {
  Accepted = false;
  FirstNumberInput = 0;
  SecondNumberInput = 0;
  ThirdNumberInput = 0;

  digitalWrite(FirstGreenLedPin, LOW);
  digitalWrite(SecondGreenLedPin, LOW);
  digitalWrite(ThirdGreenLedPin, LOW);
  Servo1.write(0);

  digitalWrite(RedLedPin, HIGH);
  delay(500);
  digitalWrite(RedLedPin, LOW);
  delay(500);
  digitalWrite(RedLedPin, HIGH);
  delay(500);
  digitalWrite(RedLedPin, LOW);
}

// Servo Function
int ServoRotate(bool ok) {
  if (ok) {
    // Rotate servo
    Servo1.write(90);


    // Print code to screen
    lcd.clear();
    String code = "Code: " + String(FirstNumberInput) + String(SecondNumberInput) + String(ThirdNumberInput);
    lcd.setCursor(0, 0);
    lcd.print(code);

    delay(5000);

    lcd.clear();
    lcd.print("Number:");
  }
}

void SetNewCode(int nc, int c, int p) {
  c = nc;
  lcd.setCursor(p, 1);
  lcd.print(c);
}

void ShortKeyPress() {
  if (!Accepted) {
    // Check if first number is correct
    if (FirstNumberInput != FirstNumber) {
      FirstNumberInput= check(SensorNumber, FirstNumber, FirstNumberInput, FirstGreenLedPin);
      delay(100);
    } else {
      // Check if second number is correct
      if (SecondNumberInput != SecondNumber) {
        SecondNumberInput = check(SensorNumber, SecondNumber, SecondNumberInput, SecondGreenLedPin);
        delay(100);
      } else  {
        // Check if third number is correct
        if (ThirdNumberInput != ThirdNumber) {
          ThirdNumberInput = check(SensorNumber, ThirdNumber, ThirdNumberInput, ThirdGreenLedPin);
        } else  {
          // Change Master variable
          Accepted = true;
          ServoRotate(Accepted);
        }
      }
    }
  } else if (Change && Accepted) {
    if (NFirstNumber = 0) {
      NFirstNumber = SensorNumber;
      SetNewCode(NFirstNumber, FirstNumber, 0);
    } else if (NSecondNumber = 0) {
      NSecondNumber = SensorNumber;
      SetNewCode(NSecondNumber, SecondNumber, 2);
    } else if (NThirdNumber = 0) {
      NThirdNumber = SensorNumber;
      SetNewCode(NThirdNumber, ThirdNumber, 4);
    } else {
      delay(5000);
      NFirstNumber = 0;
      NSecondNumber = 0;
      NThirdNumber = 0;

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Number:");

      Change = false;
    }
  }
}

// Called when button is kept pressed
void LongKeyPress() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("New Code:");
  Change = true;
}

// Called when button is pressed once
void KeyPress() {
    LongKeyPressCount = 0;
}

// Called when key goes from pressed to not pressed
void KeyRelease() {
  if (LongKeyPressCount >= LongkeyPressCountMax) {
    LongKeyPress();
  }
  else {
    ShortKeyPress();
  }
}

void goToSleep() {
  sleep_enable();
  attachInterrupt(SleepButtonPin, wakeUp, LOW);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_cpu();
}
void wakeUp() {
  sleep_disable();
  detachInterrupt(0);
}
