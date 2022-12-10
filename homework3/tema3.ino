const int pinA = 12;
const int pinB = 8;
const int pinC = 5;
const int pinD = 3;
const int pinE = A0;
const int pinF = 11;
const int pinG = 6;
const int pinDp = 4;

const int pinGroundDisplay0 = 13;
const int pinGroundDisplay1 = 10;
const int pinGroundDisplay2 = 9;
const int pinGroundDisplay3 = 7;

const int pinJoystickXAxis = A1;
const int pinJoystickYAxis = A2;

const int pinJoystickButton = 2;

const int index = 0;
const int indexXAxis = 0;
const int indexYAxis = 1;

const int debounceDelay = 50;
const int incrementVar = 1;

const int sevenSegmentPinsArray[] =  {pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDp};
const int fourDisplayPinsArray[] =  {pinGroundDisplay0, pinGroundDisplay1, pinGroundDisplay2, pinGroundDisplay3};
const int joystickAxisPinsArray[] = {pinJoystickXAxis, pinJoystickYAxis};

const int numberOfSegments = 8;
const int numberOfDisplays = 4;
const int numberOfDirections = 4;

const int up = 0;
const int down = 1;
const int left = 2;
const int right = 3;
const int leftOrDown = 0;
const int rightOrUp = 1;
const int notAState = -1;

const int selectedSegHighTh1 = 250;
const int selectedSegLowTh = 500;
const int selectedSegHighTh2 = 750;

const int startDisplayPos = 0;
const int startSegmentPos = 7;

const int millisToSeconds = 1000;

const int joyStickMoves[2][2] = {
          {left, right},  //x axis
          {down,    up}   //y axis
};

const int minNeutralJoystickValue = 300;
const int maxNeutralJoystickValue = 750;

const bool commonAnode = false;
const bool pnpTransistors = false;
const bool pullUp = true;
const bool startState = true;

const unsigned long resetTime = 2000;

//negative numbers appear when the display is changed
int nextState[numberOfSegments][numberOfDirections] = {
        //UP DOWN LEFT RIGHT
          {-9,  6,  5,  1}, // a
          { 0,  6,  5, -3}, // b
          { 6,  3,  4, -4}, // c
          { 6,  7,  4,  2}, // d
          { 6,  3, -6,  2}, // e
          { 0,  6, -7,  1}, // f
          { 0,  3, -2, -2}, // g
          { 3, -9, -9, -9}  // dp
};

// configured for common cathod
byte segmentsStates[numberOfDisplays][numberOfSegments] = {
        // a, b, c, d, e, f, g, dp
          {0, 0, 0, 0, 0, 0, 0, 0}, // display 0
          {0, 0, 0, 0, 0, 0, 0, 0}, // display 1
          {0, 0, 0, 0, 0, 0, 0, 0}, // display 2
          {0, 0, 0, 0, 0, 0, 0, 0}  // display 3
};

//pos 0 for x axis and 1 for y axis
bool joystickAxisMoved[2];
bool joystickAxisTimerWasStarted[2];
bool isChoosingSegment;
volatile bool wasButtonPressed;

unsigned long joystickAxisTimerStart[2];\
volatile unsigned long lastDebounceTime;

int currentDisplayPos;
int currentSegmentPos;

//the two variables below are configured initially for a PULLDOWN connected button
byte buttonState = LOW;
byte isButtonReleasedState = LOW;
//configured for common cathod
byte highDisplayValue = HIGH;
byte lowDisplayValue = LOW;
//configured for npn transistor
byte highSegmentValue = HIGH;
byte lowSegmentValue = LOW;

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
    for (int i = index; i < numberOfDisplays; i++)
      for (int j = index; j < numberOfSegments; j++)
        segmentsStates[i][j] = lowSegmentValue;
  }

  currentDisplayPos = startDisplayPos;
  currentSegmentPos = startSegmentPos;
  isChoosingSegment = startState;

  for (int i = index; i < numberOfSegments; i++)
    pinMode(sevenSegmentPinsArray[i], OUTPUT);

  for (int i = index; i < numberOfDisplays; i++)
    pinMode(fourDisplayPinsArray[i], OUTPUT);
    
  pinMode(pinJoystickButton, INPUT_PULLUP);
  pinMode(pinJoystickXAxis, INPUT);
  pinMode(pinJoystickYAxis, INPUT);

  attachInterrupt(digitalPinToInterrupt(pinJoystickButton), buttonInterrupt, CHANGE);
  digitalWrite(sevenSegmentPinsArray[currentSegmentPos], HIGH);

  Serial.begin(9600);
}

void loop() {
  if (wasButtonPressed && millis() - lastDebounceTime > resetTime && isChoosingSegment) {
    wasButtonPressed = false;
    resetDisplays();
  }

  moveJoystickAxis(indexXAxis);
  if (isChoosingSegment)
    moveJoystickAxis(indexYAxis);

  displaySegments();
}

//interrupt function with debounce which signals that button was pressed
void buttonInterrupt() {
  if (digitalRead(pinJoystickButton) != isButtonReleasedState) {
    lastDebounceTime = millis();
    wasButtonPressed = true;
  }
  else if (millis() - lastDebounceTime > debounceDelay && wasButtonPressed) {
    isChoosingSegment = !isChoosingSegment;
    wasButtonPressed = false;
  }
}

//when in choosingSegmentState can move the segment, in the other state can change LED's state
void moveJoystickAxis(int axis) {
  int joystickAxisInput = analogRead(joystickAxisPinsArray[axis]);
  if (joystickAxisInput < minNeutralJoystickValue || joystickAxisInput > maxNeutralJoystickValue) {
    if (!joystickAxisTimerWasStarted[axis]) {
      joystickAxisTimerStart[axis] = millis();
      joystickAxisTimerWasStarted[axis] = true;
    }
    if (millis() - joystickAxisTimerStart[axis] > debounceDelay && !joystickAxisMoved[axis]) {
      joystickAxisMoved[axis] = true;
      if (isChoosingSegment) {
        if (joystickAxisInput < minNeutralJoystickValue)
          moveSelection(axis, leftOrDown);
        else
          moveSelection(axis, rightOrUp);
      } else {
        if (joystickAxisInput < minNeutralJoystickValue)
          segmentsStates[currentDisplayPos][currentSegmentPos] = lowSegmentValue;
        else
          segmentsStates[currentDisplayPos][currentSegmentPos] = highSegmentValue;
      }
    }
  } else {
    joystickAxisTimerWasStarted[axis] = false;
    joystickAxisMoved[axis] = false;
  }
}

//computes the next state and checks if is possible, if yes changes the segment pos and display pos
void moveSelection(int axis, int direction) {
  int nextDisplayPos = currentDisplayPos;
  int nextSegmentPos = nextState[currentSegmentPos][joyStickMoves[axis][direction]];
  if (nextSegmentPos < index) {
    if (direction == leftOrDown && currentDisplayPos > index)
      nextDisplayPos = currentDisplayPos - incrementVar;
    else if (direction == rightOrUp && currentDisplayPos < numberOfDirections - incrementVar)
      nextDisplayPos = currentDisplayPos + incrementVar;
    if (nextDisplayPos == currentDisplayPos) {
      nextSegmentPos = notAState;
    } else {
      nextSegmentPos = numberOfSegments + nextSegmentPos;
    }
  }

  if (nextSegmentPos != notAState) {
    digitalWrite(sevenSegmentPinsArray[currentSegmentPos], LOW);
    currentDisplayPos = nextDisplayPos;
    currentSegmentPos = nextSegmentPos;
  }
}

void displaySegments() {
  int milliSeconds = millis () % millisToSeconds;
  for (int i = index; i < numberOfDisplays; i++) {
    digitalWrite(fourDisplayPinsArray[i], highDisplayValue);
    for (int j = index; j < numberOfSegments; j++) {
      if (i == currentDisplayPos && j == currentSegmentPos && isChoosingSegment)
        if (milliSeconds < selectedSegHighTh1 || milliSeconds > selectedSegLowTh && milliSeconds < selectedSegHighTh2)
         digitalWrite(sevenSegmentPinsArray[currentSegmentPos], highSegmentValue);
        else
          digitalWrite(sevenSegmentPinsArray[currentSegmentPos], lowSegmentValue);
      else
        digitalWrite(sevenSegmentPinsArray[j], segmentsStates[i][j]);
      digitalWrite(sevenSegmentPinsArray[j], lowSegmentValue);
    }
    digitalWrite(fourDisplayPinsArray[i], lowDisplayValue);
  }
}

void resetDisplays() {
  for (int i = index; i < numberOfDisplays; i++)
    for (int j = index; j < numberOfSegments; j++)
      segmentsStates[i][j] = lowSegmentValue;
  currentDisplayPos = startDisplayPos;
  currentSegmentPos = startSegmentPos;
  isChoosingSegment = startState;
}
