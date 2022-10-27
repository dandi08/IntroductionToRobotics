const int redInputPin = A3;
const int greenInputPin = A4;
const int blueInputPin = A5;

const int redPin = 11;
const int greenPin = 10;
const int bluePin = 9;

const int inputMin = 0;
const int inputMax = 1023;

const int outputMin = 0;
const int outputMax = 255;

const int maxNoiseMargin = 20;

const bool commonAnode = false;

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  pinMode(redInputPin, INPUT);
  pinMode(greenInputPin, INPUT);
  pinMode(blueInputPin, INPUT);

  Serial.begin(9600);
}

long readAndConvert(int inputPin) {
  long input = analogRead(inputPin);
  if (input <= maxNoiseMargin)
    input = inputMin;

  if (commonAnode)
    return map(input, inputMin, inputMax, outputMax, outputMin);

  return map(input, inputMin, inputMax, outputMin, outputMax);
}

void loop() {
  long redOutput = readAndConvert(redInputPin);
  long greenOutput = readAndConvert(greenInputPin);
  long blueOutput = readAndConvert(blueInputPin);

  analogWrite(redPin, redOutput);
  analogWrite(greenPin, greenOutput);
  analogWrite(bluePin, blueOutput);
}

