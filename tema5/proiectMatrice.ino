#include "LedControl.h" //  need the library
#include "bulletClass.h"
#include "messages.h"
#include "matrixPatterns.h"
#include "LiquidCrystal.h"
#include "EEPROM.h"

const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;
const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;

const byte buttonPin = 2;
bool wasButtonPressed;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

LedControl matrixLc = LedControl(dinPin, clockPin, loadPin, 1); // DIN, CLK, LOAD, No. DRIVER

byte matrixBrightness = 6;

byte pos[2] = {4, 4};

const byte moveInterval = 100;
const byte moveEnemiesInterval = 1500;
const int enemyShotcooldown = 2500;

unsigned long long lastMoved = 0;

bool matrixChanged = true;

const int debounceDelay = 50;

const int pinJoystickXAxis = A0;
const int pinJoystickYAxis = A1;

const int joystickAxisPinsArray[] = {pinJoystickXAxis, pinJoystickYAxis};

const int minNeutralJoystickValue = 450;
const int maxNeutralJoystickValue = 600;

//pos 0 for x axis and 1 for y axis
bool joystickAxisMoved[2];
bool joystickAxisTimerWasStarted[2];

unsigned long long joystickAxisTimerStart[2];
volatile unsigned long lastDebounceTime;

const int index = 0;
const int indexYAxis = 0;
const int indexXAxis = 1;

unsigned long long lastEnemyMoved = 0;
unsigned long long lastEnemyShot = millis();

byte gameMatrix[matrixSize][matrixSize] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}  
};

const byte enemiesSize = 4;

Bullet* bullet;

byte enemiesPos[enemiesSize][2] = {
  {0, 0},
  {0, matrixSize - 1},
  {matrixSize - 1, 0},
  {matrixSize - 1, matrixSize - 1}
};

byte enemiesMovement[enemiesSize][2] = {
  {1, 0},
  {0, -1},
  {0, 1},
  {-1, 0}
};

const byte enemiesShot[enemiesSize][2] = {
  {0, 1},
  {1, 0},
  {-1, 0},
  {0, -1}
};

unsigned long long gameStartTimer;
const int greetingMessageTime = 4000;
bool greetingsEnded = false;

const bool commonAnodeMatrix = true;
byte highMatrixLedState = HIGH;
byte lowMatrixLedState = LOW;

bool wasSet = false;

byte gameState;
const byte lcdLength = 16;
const byte lcdWidth = 2;

const bool pullUp = true;
//the two variables below are configured initially for a PULLDOWN connected button
byte buttonState = LOW;
byte isButtonReleasedState = LOW;
bool buttonTrigger;

void setup() {
  Serial.begin(9600);
  if (pullUp) {
    buttonState = HIGH;
    isButtonReleasedState = HIGH;
  }

  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonInterrupt, CHANGE);
  if (commonAnodeMatrix) {
    highMatrixLedState = LOW;
    lowMatrixLedState = HIGH;
  }
  EEPROM.get(0, matrixBrightnessValue);
  for (int i = matrixBrightnessLowPos + 1; i <= matrixBrightnessValue; i++)
    matrixBrightnessMsg[matrixBrightnessBottomOption][i] = fullChar;
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  matrixLc.shutdown(0, false); // turn off power saving, enables display
  matrixLc.setIntensity(0, matrixBrightnessValue); // sets brightness (0~15 possible values)
  matrixLc.clearDisplay(0);// clear screen
  gameMatrix[pos[0]][pos[1]] = 1;

  lcd.begin(lcdLength, lcdWidth);
  lcd.createChar(0, arrowCharacter);
  updateMatrix(greetingSymbol);
  gameState = greetingState;
  gameStartTimer = millis();
}

void loop() {
  switch(gameState) {
    case greetingState:
      state1();
      break;
    case mainMenuState:
      state2();
      break;
    case chooseNameState:
      state3();
      break;
    case inGameState:
      level1();
      break;
    case leaderboardState:
      state5();
      break;
    case settingsState:
      state6();
      break;
    case difficultyState:
      state7();
      break;
    case lcdContrastState:
      state8();
      break;
    case lcdBrightnessState:
      state9();
      break;
    case matrixBrightnessState:
      state10();
      break;
    case howToPlayState:
      state11();
      break;
    case creditsState:
      state12();
      break;
    case gameEndedState:
      state13();
      break;
  };
}

//interrupt function with debounce which signals that button was pressed
void buttonInterrupt() {
  if (digitalRead(buttonPin) != isButtonReleasedState && !wasButtonPressed && !buttonTrigger) {
    lastDebounceTime = millis();
    wasButtonPressed = true;
  }
  else if (millis() - lastDebounceTime > debounceDelay && wasButtonPressed) {
    buttonTrigger = true;
    wasButtonPressed = false;
  }
}

void updateMatrix(byte matrix[][8]) {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      matrixLc.setLed(0, row, col, matrix[row][col]);
    }
  }
}

void moveJoystickAxis(byte axis, bool inGame) {
  int joystickAxisInput = analogRead(joystickAxisPinsArray[axis]);
  if (joystickAxisInput < minNeutralJoystickValue || joystickAxisInput > maxNeutralJoystickValue) {
    if (!joystickAxisTimerWasStarted[axis]) {
      joystickAxisTimerStart[axis] = millis();
      joystickAxisTimerWasStarted[axis] = true;
    }
    if (millis() - joystickAxisTimerStart[axis] > debounceDelay  && (!joystickAxisMoved[axis] || inGame)) {
      joystickAxisMoved[axis] = true;
      switch(gameState) {
        case mainMenuState:
          selectOptions(mainMenuNumberOfOptions, mainMenuSelectedOption, mainMenuTopOption, mainMenuBottomOption, joystickAxisInput);
          break;
        case leaderboardState:
          selectOptions(leaderboardNumberOfOptions, leaderboardSelectedOption, leaderboardTopOption, leaderboardBottomOption, joystickAxisInput);
          break;
        case settingsState:
          selectOptions(settingsNumberOfOptions, settingsSelectedOption, settingsTopOption, settingsBottomOption, joystickAxisInput);
          break;
        case chooseNameState:
          if (axis == indexXAxis)
            changeLetterPosition(joystickAxisInput);
          else
            changeLetterValue(joystickAxisInput, chooseNameMsg[chooseNameBottomOption][selectedLetter]);
          break;
        case matrixBrightnessState:
          changeSettings(joystickAxisInput, matrixBrightnessMsg[matrixBrightnessBottomOption], matrixBrightnessLowPos, matrixBrightnessHighPos, matrixBrightnessValue);
          break;
        case inGameState:
          movePlayer(axis, joystickAxisInput);
          break;
      };
    }
  } else {
    joystickAxisTimerWasStarted[axis] = false;
    joystickAxisMoved[axis] = false;
  }
}

void lcdDisplayMessage(byte index, byte row, char message[]) {
  lcd.setCursor(index, row);
  lcd.write(message);
}

void lcdDisplaySpecialCharacter(byte index, byte row, byte characterIndex) {
  lcd.setCursor(index, row);
  lcd.write(characterIndex);
}

void displayOptions(char msg[][17], byte selectedOption, byte topOption, byte bottomOption) {
  lcdDisplayMessage(optionIndex, topOptionRow, msg[topOption]);
  if (selectedOption == upCursorPosition)
    lcdDisplaySpecialCharacter(optionHighlightIndex, selectedOption, arrowCharacterIndex);
  lcdDisplayMessage(optionIndex, bottomOptionRow, msg[bottomOption]);
  if (selectedOption == downCursorPosition)
    lcdDisplaySpecialCharacter(optionHighlightIndex, selectedOption, arrowCharacterIndex);
}

void selectOptions(byte numberOfOptions, byte& selectedOption, byte& topOption, byte& bottomOption, int joystickAxisInput) {
  if (joystickAxisInput > maxNeutralJoystickValue) {
    if (selectedOption == upCursorPosition) {
      selectedOption = downCursorPosition;
    } else if (bottomOption < numberOfOptions - 1) {
      topOption++;
      bottomOption++;
    }
  } else {
    if (selectedOption == downCursorPosition) {
      selectedOption = upCursorPosition;
    } else if (topOption > 0) {
      topOption--;
      bottomOption--;
    }
  }
}

void changeLetterPosition(int joystickAxisInput) {
  if (joystickAxisInput < minNeutralJoystickValue) {
    if (selectedLetter == lastLetterPosition)
      selectedLetter = firstLetterPosition;
    else
      selectedLetter++;
  } else {
    if (selectedLetter == firstLetterPosition)
      selectedLetter = lastLetterPosition;
    else
      selectedLetter--;
  }
}

void changeLetterValue(int joystickAxisInput, char& letter) {
  if (joystickAxisInput < minNeutralJoystickValue) {
    if (letter == asciiLetterEndPos)
      letter = asciiLetterStartPos;
    else
      letter++;
  } else {
    if (letter == asciiLetterStartPos)
      letter = asciiLetterEndPos;
    else
      letter--;
  }
}

void changeSettings(int joystickAxisInput, char settingMsg[], byte lowPosition, byte highPosition, byte& value) {
  if (joystickAxisInput > maxNeutralJoystickValue) {
    if (value > lowPosition) {
      settingMsg[value] = emptyChar;
      value--;
      matrixLc.setIntensity(0, value);
    }
  } else {
    if (value < highPosition) {
      value++;
      settingMsg[value] = fullChar;
      matrixLc.setIntensity(0, value);
    }
  }
}

//greeting state
void state1() {
  lcdDisplayMessage(greetingMsgPart1StartIndex, greetingMsgPart1StartRow, greetingMsgPart1);
  lcdDisplayMessage(greetingMsgPart2StartIndex, greetingMsgPart2StartRow, greetingMsgPart2);
  if (!greetingsEnded && millis() - gameStartTimer > greetingMessageTime) {
    gameState = mainMenuState;
    greetingsEnded = true;
  }
}

//main menu state
void state2() {
  updateMatrix(mainMenuSymbol);
  displayOptions(mainMenuMsg, mainMenuSelectedOption, mainMenuTopOption, mainMenuBottomOption);
  moveJoystickAxis(indexYAxis, false);
  if (buttonTrigger) {
    gameState = mainMenuRedirect[mainMenuSelectedOption == 0 ? mainMenuTopOption : mainMenuBottomOption];
    buttonTrigger = false;
  }
}

//choose name state
void state3() {
  updateMatrix(questionMarkSymbol);
  displayOptions(chooseNameMsg, noOptions, chooseNameTopOption, chooseNameBottomOption);
  moveJoystickAxis(indexXAxis, false);
  moveJoystickAxis(indexYAxis, false);
  if (buttonTrigger) {
    gameState = inGameState;
    buttonTrigger = false;
  }
}

//main menu state
void state5() {
  updateMatrix(leaderboardSymbol);
  displayOptions(leaderboardMsg, leaderboardSelectedOption, leaderboardTopOption, leaderboardBottomOption);
  moveJoystickAxis(indexYAxis, false);
  if (buttonTrigger) {
    gameState = mainMenuState;
    buttonTrigger = false;
  }
}

//settings state
void state6() {
  updateMatrix(settingsSymbol);
  displayOptions(settingsMsg, settingsSelectedOption, settingsTopOption, settingsBottomOption);
  moveJoystickAxis(indexYAxis, false);
  if (buttonTrigger) {
    gameState = settingsRedirect[settingsSelectedOption == 0 ? settingsTopOption : settingsBottomOption];
    buttonTrigger = false;
  }
}

//update game difficulty
void state7() {
  updateMatrix(questionMarkSymbol);
  displayOptions(gameDifficultyMsg, noOptions, gameDifficultyTopOption, gameDifficultyBottomOption);
  if (buttonTrigger) {
    gameState = settingsState;
    buttonTrigger = false;
  }
}

//update lcd contrast
void state8() {
  updateMatrix(questionMarkSymbol);
  displayOptions(lcdContrastMsg, noOptions, lcdContrastTopOption, lcdContrastBottomOption);
  if (buttonTrigger) {
    gameState = settingsState;
    buttonTrigger = false;
  }
}

//update lcd brightness
void state9() {
  updateMatrix(questionMarkSymbol);
  displayOptions(lcdBrightnessMsg, noOptions, lcdBrightnessTopOption, lcdBrightnessBottomOption);
  if (buttonTrigger) {
    gameState = settingsState;
    buttonTrigger = false;
  }
}

//update matrix brightness
void state10() {
  updateMatrix(fullMatrixSymbol);
  displayOptions(matrixBrightnessMsg, noOptions, matrixBrightnessTopOption, matrixBrightnessBottomOption);
  moveJoystickAxis(indexYAxis, false);
  EEPROM.update(0, matrixBrightnessValue);
  if (buttonTrigger) {
    gameState = settingsState;
    buttonTrigger = false;
  }
}

//how to play state
void state11() {
  updateMatrix(howToPlaySymbol);
  displayOptions(howToPlayMsg, noOptions, howToPlayTopOption, howToPlayBottomOption);
  if (buttonTrigger) {
    gameState = mainMenuState;
    buttonTrigger = false;
  }
}

//credits state
void state12() {
  updateMatrix(creditsSymbol);
  displayOptions(creditsMsg, noOptions, creditsTopOption, creditsBottomOption);
  if (buttonTrigger) {
    gameState = mainMenuState;
    buttonTrigger = false;
  }
}

//game ended state
void state13() {
  displayOptions(gameEndedMsg, noOptions, gameEndedTopOption, gameEndedBottomOption);
  if (buttonTrigger) {
    gameState = mainMenuState;
    buttonTrigger = false;
  }
}

// Level 1
void level1() {
  if (buttonTrigger) {
    gameState = gameEndedState;
    buttonTrigger = false;
    return;
  } 

  if (millis() - lastMoved > moveInterval) {
    moveJoystickAxis(indexXAxis, true);
    moveJoystickAxis(indexYAxis, true);
    lastMoved = millis();
  }

  if (millis() - lastEnemyMoved > moveEnemiesInterval) {
    moveEnemies();
    lastEnemyMoved = millis();
  }
  
  if (millis() - lastEnemyShot > enemyShotcooldown && !wasSet) {
    shotEnemy();
    wasSet = true;
    lastEnemyShot = millis();
  }

  if (wasSet && millis() - bullet->getBulletTimer() > bullet->getBulletMoveDelay()) {
    gameMatrix[bullet->getXPos()][bullet->getYPos()] = 0;
    (*bullet).moveBullet();
    gameMatrix[bullet->getXPos()][bullet->getYPos()] = 1;
    if (outOfBounds((*bullet).getXPos()) || outOfBounds((*bullet).getYPos())) {
      wasSet = false;
    }
  }
}

void moveEnemies() {
  for (int i = index; i < enemiesSize; i++) {
    gameMatrix[enemiesPos[i][0]][enemiesPos[i][1]] = 0;
    if (outOfBounds(enemiesPos[i][0] + enemiesMovement[i][0]) || outOfBounds(enemiesPos[i][1] + enemiesMovement[i][1])) {
      enemiesMovement[i][0] *= -1;      enemiesMovement[i][1] *= -1;
    }
    enemiesPos[i][0] = enemiesPos[i][0] + enemiesMovement[i][0];
    enemiesPos[i][1] = enemiesPos[i][1] + enemiesMovement[i][1];
    gameMatrix[enemiesPos[i][0]][enemiesPos[i][1]] = 1;
  }

  updateMatrix(gameMatrix);
}

void shotEnemy() {
  byte enemyIndex = random(4);
  Bullet localBullet = Bullet(enemiesPos[enemyIndex][0], enemiesPos[enemyIndex][1], enemiesShot[enemyIndex][0], enemiesShot[enemyIndex][1], millis());
  bullet = &localBullet;
  gameMatrix[bullet->getXPos()][bullet->getYPos()] = 1;
}

void movePlayer(byte axis, int joystickAxisInput) {
  gameMatrix[pos[indexYAxis]][pos[indexXAxis]] = 0;
  if (joystickAxisInput < minNeutralJoystickValue) {
    pos[axis]--;
    if (pos[axis] > matrixSize)
      pos[axis] = matrixSize - 1;
    
    gameMatrix[pos[indexYAxis]][pos[indexXAxis]] = 1;
  } else {
    pos[axis]++;
    if (pos[axis] == matrixSize)
      pos[axis] = 0;
    
    gameMatrix[pos[indexYAxis]][pos[indexXAxis]] = 1;
  }
}

bool outOfBounds(byte position) {
  if (position >= matrixSize)
    return true;
  
  return false;
}


