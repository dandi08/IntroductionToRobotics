//Code for a traffic light crosswalk for pedestrians with button, buzzer and option to control traffic flow.

const int yellowCarTrafficLightPin = 3;
const int redCarTrafficLightPin = 4;
const int greenCarTrafficLightPin = 5;

const int redPedestrianTrafficLightPin = A4;
const int greenPedestrianTrafficLightPin = A5;

const int buzzerPin = 6;
const int buttonPin = 2;

const int pinA = 9;
const int pinB = 10;
const int pinC = 11;
const int pinD = 13;
const int pinE = 12;
const int pinF = 8;
const int pinG = 7;

const int sevenSegmentPinsArray[] =  {pinA, pinB, pinC, pinD, pinE, pinF, pinG};

const int trafficStatePin = A0;

const int millisToSeconds = 1000;
const int errorMargin = 1;
const int debounceDelay = 50;
const int index = 0;
const int pedestrianHasGreenTh = 0;
const int pedestrianHasRedTh = 1000;

const int buttonPressDelay = 3000;

const int buzzerGreenLightTone = 800;
const int buzzerLastSecondsTone = 100;
const int buzzerRedLightTone = 15;
const int buzzerDuration = 100;
const int buzzerWaittingTime = 2;
const int buzzerShouldRingValue = 0;

const int yellowLightTh = 1000;
const int ledChangeStateTh = 500;
const int ledStartBlinkingTh = 4000;

const int minPedestrianCrossTime = 0;
const int maxPedestrianCrossTime = 10000;
const int minPedestrianWaittingTimer = 1000;
const int maxPedestrianWaittingTimer = 5000;
const int minCarTraffic = 0;
const int maxCarTraffic = 1023;

const int numberOfSegments = 7;
const int sevenDigitResetState = 10;
const int sevenSegmentNumberOfStates = 11;

int trafficValue;
int previousSecond;
int pedestrianWaittingTimer;
int buttonPressedTimeStart;
int pedestrianCrossTimeStart;
volatile int lastDebounceTime;

unsigned long pedestrianGreenLightEndTime;
unsigned long pedestrianWaittingTimeStart;

const bool commonAnode = false;
const bool pullUp = true;

bool isPedestrianWaitting;
bool pedestrianHasGreen;
volatile bool wasButtonPressed;

//the two variables below are configured initially for a PULLUP connected button
byte buttonState = LOW;
byte isButtonReleasedState = LOW;

//matrix is configured for common Cathode
byte digitsMatrix[sevenSegmentNumberOfStates][numberOfSegments] { 
          //a  b  c  d  e  f  g
           {1, 1, 1, 1, 1, 1, 0}, //0
           {0, 1, 1, 0, 0, 0, 0}, //1
           {1, 1, 0, 1, 1, 0, 1}, //2
           {1, 1, 1, 1, 0, 0, 1}, //3
           {0, 1, 1, 0, 0, 1, 1}, //4
           {1, 0, 1, 1, 0, 1, 1}, //5
           {1, 0, 1, 1, 1, 1, 1}, //6
           {1, 1, 1, 0, 0, 0, 0}, //7
           {1, 1, 1, 1, 1, 1, 1}, //8
           {1, 1, 1, 1, 0, 1, 1}, //9
           {0, 0, 0, 0, 0, 0, 0}  //reset state
};

void setup() {
  if (pullUp) {
    buttonState = HIGH;
    isButtonReleasedState = HIGH;
  }

  if (commonAnode) {
    for (int i = index; i < sevenSegmentNumberOfStates; i++)
      for (int j = index; j < numberOfSegments; j++)
        if (digitsMatrix[i][j] == HIGH)
          digitsMatrix[i][j] = LOW;
        else
          digitsMatrix[i][j] = HIGH;
  }

  pinMode(yellowCarTrafficLightPin, OUTPUT);
  pinMode(redCarTrafficLightPin, OUTPUT);
  pinMode(greenCarTrafficLightPin, OUTPUT);

  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  for (int i = index; i < numberOfSegments; i++)
    pinMode(sevenSegmentPinsArray[i], OUTPUT);

  pinMode(trafficStatePin, INPUT);

  pinMode(redPedestrianTrafficLightPin, OUTPUT);
  pinMode(greenPedestrianTrafficLightPin, OUTPUT);
  initialLedsState();
  attachInterrupt(digitalPinToInterrupt(buttonPin), blink, CHANGE);
  
  Serial.begin(9600);
}

void loop() {
  if (pedestrianHasGreen) {
    pedestrianCrossAction();
  } else {
    //checking button delay
    if (wasButtonPressed && millis() - pedestrianGreenLightEndTime > buttonPressDelay) {
      if (!isPedestrianWaitting) {
        pedestrianWaittingTimeStart = millis();
        pedestrianWaittingTimer = map(analogRead(trafficStatePin), minCarTraffic, maxCarTraffic, minPedestrianWaittingTimer, maxPedestrianWaittingTimer);
        isPedestrianWaitting = true;
      }

      //checking traffic delay
      int timeUntillPedestrianGreen = pedestrianWaittingTimer - (millis() - pedestrianWaittingTimeStart);
      if (timeUntillPedestrianGreen < pedestrianHasGreenTh) {
        pedestrianHasGreen = true;
        pedestrianCrossTimeStart = millis();
      } else if (timeUntillPedestrianGreen < yellowLightTh)
        setCarTrafficLight(LOW, HIGH, LOW);
    }

    int second = millis() / millisToSeconds;
    if (second % buzzerWaittingTime == buzzerShouldRingValue && second != previousSecond) {
      tone(buzzerPin, buzzerRedLightTone, buzzerDuration);
      previousSecond = second;
    }
  }
}

void initialLedsState() {
  setPedestrianTraffigLight(HIGH, LOW);
  setCarTrafficLight(LOW, LOW, HIGH);
}

//interrupt function with debounce that signals that button was pressed
void blink() {
  if (wasButtonPressed)
    return;
  if (digitalRead(buttonPin) != isButtonReleasedState)
    lastDebounceTime = millis();
  else if ((millis() - lastDebounceTime) > debounceDelay)
    wasButtonPressed = true;
}

void pedestrianCrossAction() {
  int pedestrianTimer = maxPedestrianCrossTime - millis() + pedestrianCrossTimeStart - errorMargin;
  int pedestrianTimerSeconds = pedestrianTimer / millisToSeconds;
  int pedestrianTimerMillis = pedestrianTimer % millisToSeconds;
  //case when cars have green again
  if (pedestrianTimer < minPedestrianCrossTime) {
    pedestrianHasGreen = false;
    wasButtonPressed = false;
    pedestrianGreenLightEndTime = millis();
    isPedestrianWaitting = false;
    displayDigit(sevenDigitResetState);
    setCarTrafficLight(LOW, LOW, HIGH);
    return;
  }

  displayDigit(pedestrianTimerSeconds);
  //case when crossing time is over
  setCarTrafficLight(HIGH, LOW, LOW);
  if (pedestrianTimer < pedestrianHasRedTh) {
    noTone(buzzerPin);
    setPedestrianTraffigLight(HIGH, LOW);
    return;
  }

  //case when pedestrians have green light
  if (pedestrianTimer > ledStartBlinkingTh || pedestrianTimerMillis >= ledChangeStateTh) {
    setPedestrianTraffigLight(LOW, HIGH);
    if (pedestrianTimer > ledStartBlinkingTh)
      tone(buzzerPin, buzzerGreenLightTone, buzzerDuration);
    else
      tone(buzzerPin, buzzerLastSecondsTone, buzzerDuration);
  } else {
    setPedestrianTraffigLight(LOW, LOW);
    noTone(buzzerPin);
  }
}

void setPedestrianTraffigLight(byte redLightValue, byte greenLightValue) {
  digitalWrite(redPedestrianTrafficLightPin, redLightValue);
  digitalWrite(greenPedestrianTrafficLightPin, greenLightValue);
}

void setCarTrafficLight(byte redLightValue, byte yellowLightValue, byte greenLightValue) {
  digitalWrite(redCarTrafficLightPin, redLightValue);
  digitalWrite(yellowCarTrafficLightPin, yellowLightValue);
  digitalWrite(greenCarTrafficLightPin, greenLightValue);
}

//displays the digit received as parameter on the 7 segment digit display
void displayDigit(int digit) {
  for (int i = index; i < numberOfSegments; i++)
    digitalWrite(sevenSegmentPinsArray[i], digitsMatrix[digit][i]);
}
