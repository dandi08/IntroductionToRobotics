const int latchPin = 11;
const int clockPin = 10;
const int dataPin = 12;

const int pinDisplay1 = 7;
const int pinDisplay2 = 6;
const int pinDisplay3 = 5;
const int pinDisplay4 = 4;

const int pinJoystickXAxis = A1;
const int pinJoystickYAxis = A2;

const int pinJoystickButton = 2;

const int startNumber = 0;

const byte regSize = 8;

int fourDisplayPinsArray[] = {
  pinDisplay1, pinDisplay2, pinDisplay3, pinDisplay4
};

unsigned long joystickAxisTimerStart[2];
bool joystickAxisMoved[2];
bool joystickAxisTimerWasStarted[2];

const int millisToSeconds = 1000;
const int up = 0;
const int down = 1;
const int left = 2;
const int right = 3;

const int joyStickMoves[2][2] = {
          {left, right},  //x axis
          {down,    up}   //y axis
};

const int numberOfDigits = 16;

byte hexNumbers[17][8] { 
           {1,1,1,1,1,1,0,0}, //0
           {0,1,1,0,0,0,0,0}, //1
           {1,1,0,1,1,0,1,0}, //2
           {1,1,1,1,0,0,1,0}, //3
           {0,1,1,0,0,1,1,0}, //4
           {1,0,1,1,0,1,1,0}, //5
           {1,0,1,1,1,1,1,0}, //6
           {1,1,1,0,0,0,0,0}, //7
           {1,1,1,1,1,1,1,0}, //8
           {1,1,1,1,0,1,1,0}, //9
           {1,1,1,0,1,1,1,0}, //A/10
           {0,0,1,1,1,1,1,0}, //b/11
           {1,0,0,1,1,1,0,0}, //C/12
           {0,1,1,1,1,0,1,0}, //d/13
           {1,0,0,1,1,1,1,0}, //E/14
           {1,0,0,0,1,1,1,0}, //F/15
           {0,0,0,0,0,0,0,0}, //reset state
};

byte byteEncodings[numberOfDigits + 1] = {
//A B C D E F G DP 
  B11111100, // 0 
  B01100000, // 1
  B11011010, // 2
  B11110010, // 3
  B01100110, // 4
  B10110110, // 5
  B10111110, // 6
  B11100000, // 7
  B11111110, // 8
  B11110110, // 9
  B11101110, // A
  B00111110, // b
  B10011100, // C
  B01111010, // d
  B10011110, // E
  B10001110,  // F
  B00000000  // reset
};

byte state = 0;
int currentDisplayPos;

const int joystickAxisPinsArray[] = {pinJoystickXAxis, pinJoystickYAxis};

//the two variables below are configured initially for a PULLDOWN connected button
byte buttonState = LOW;
byte isButtonReleasedState = LOW;
//configured for common cathod
byte highDisplayValue = HIGH;
byte lowDisplayValue = LOW;
//configured for npn transistor
byte highSegmentValue = HIGH;
byte lowSegmentValue = LOW;

const int numberOfDisplays = 4;

const int minNeutralJoystickValue = 300;
const int maxNeutralJoystickValue = 750;

const int selectedDisplayHighTh1 = 250;
const int selectedDisplayLowTh = 500;
const int selectedDisplayHighTh2 = 750;

const int startDisplayPos = 0;
const int debounceDelay = 100;
const bool commonAnode = false;
const bool pnpTransistors = false;
const bool pullUp = true;

volatile unsigned long lastDebounceTime;
volatile bool wasButtonPressed;

const unsigned long resetTime = 2000;

byte numberOnDisplay[4] = {
  startNumber, startNumber, startNumber, startNumber
};

const int index = 0;
const int indexXAxis = 0;
const int indexYAxis = 1;
const int startState = 0;
const int chooseNumberState = 1;

byte registers[regSize];

void setup() {
  if (pullUp) {
    buttonState = HIGH;
    isButtonReleasedState = HIGH;
  }

  if (commonAnode) {
    highDisplayValue = LOW;
    lowDisplayValue = HIGH;
  }

  if (pnpTransistors) {
    highSegmentValue = LOW;
    lowSegmentValue = HIGH;
  }

  currentDisplayPos = startDisplayPos;

  for (int i = index; i < numberOfDisplays; i++)
    pinMode(fourDisplayPinsArray[i], OUTPUT);
    
  pinMode(pinJoystickButton, INPUT_PULLUP);
  pinMode(pinJoystickXAxis, INPUT);
  pinMode(pinJoystickYAxis, INPUT);

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(pinJoystickButton), buttonInterrupt, CHANGE);

  Serial.begin(9600);
}

void loop() {
  if (wasButtonPressed && millis() - lastDebounceTime > resetTime && state == startState) {
    wasButtonPressed = false;
    resetDisplays();
  }

  for (int i = index; i < numberOfDisplays; i++)
    displayDigit(i);

  if (state == startState)
    moveJoystickAxis(indexXAxis);
  else
    moveJoystickAxis(indexYAxis);
}

//interrupt function with debounce which signals that button was pressed
void buttonInterrupt() {
  if (digitalRead(pinJoystickButton) != isButtonReleasedState) {
    lastDebounceTime = millis();
    wasButtonPressed = true;
  }
  else {
    if (millis() - lastDebounceTime > debounceDelay && wasButtonPressed) {
      if (state == startState)
        state = chooseNumberState;
      else
        state = startState;
    }
    wasButtonPressed = false;
  }
}

void displayDigit(int displayIndex) {
  digitalWrite(fourDisplayPinsArray[displayIndex], highDisplayValue);
  writeReg(byteEncodings[numberOnDisplay[displayIndex]], displayIndex);
  resetReg();
  digitalWrite(fourDisplayPinsArray[displayIndex], lowDisplayValue);
}

void resetReg() {
  digitalWrite(latchPin, LOW);

  shiftOut(dataPin, clockPin, MSBFIRST, byteEncodings[17]);

  digitalWrite(latchPin, HIGH);
}

void writeReg(byte encoding, byte displayIndex) {
  digitalWrite(latchPin, LOW);
  int milliSeconds = millis () % millisToSeconds;
  //condition to make dp blink
  if (displayIndex == currentDisplayPos && (milliSeconds < selectedDisplayHighTh1 || milliSeconds > selectedDisplayLowTh && milliSeconds < selectedDisplayHighTh2 || state == chooseNumberState))
    encoding++;
  shiftOut(dataPin, clockPin, MSBFIRST, encoding);
  digitalWrite(latchPin, HIGH);
}

//when in start state, with x axis user can select the display, then in second state user can change the number using y axis
void moveJoystickAxis(int axis) {
  int joystickAxisInput = analogRead(joystickAxisPinsArray[axis]);
  if (joystickAxisInput < minNeutralJoystickValue || joystickAxisInput > maxNeutralJoystickValue) {
    if (!joystickAxisTimerWasStarted[axis]) {
      joystickAxisTimerStart[axis] = millis();
      joystickAxisTimerWasStarted[axis] = true;
    }
    if (millis() - joystickAxisTimerStart[axis] > debounceDelay && !joystickAxisMoved[axis]) {
      joystickAxisMoved[axis] = true;
      if (axis == indexXAxis) {
        if (joystickAxisInput > maxNeutralJoystickValue) {
          currentDisplayPos--;
          if (currentDisplayPos < index)
            currentDisplayPos = numberOfDisplays - 1;
        } else {
          currentDisplayPos++;
          if (currentDisplayPos == numberOfDisplays)
            currentDisplayPos = index;
        }
      } else {
        if (joystickAxisInput > maxNeutralJoystickValue) {
          numberOnDisplay[currentDisplayPos]++;
          if (numberOnDisplay[currentDisplayPos] == numberOfDigits)
            numberOnDisplay[currentDisplayPos] = index;
        } else {
          numberOnDisplay[currentDisplayPos]--;
          if (numberOnDisplay[currentDisplayPos] > numberOfDigits)
            numberOnDisplay[currentDisplayPos] = numberOfDigits - 1;
        }
      }
    }
  } else {
    joystickAxisTimerWasStarted[axis] = false;
    joystickAxisMoved[axis] = false;
  }
}

void resetDisplays() {
  for (int i = index; i < numberOfDisplays; i++)
    numberOnDisplay[i] = startNumber;
  currentDisplayPos = startDisplayPos;
}


