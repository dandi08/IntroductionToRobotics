#include "lcdCustomCharacters.h"

const byte topOptionRow = 0;
const byte bottomOptionRow = 1;
const byte optionIndex = 0;
const byte optionHighlightIndex = 15;
const char optionHighlight[] = {'<', '\0'};
const byte noOptions = 3;
const byte asciiLetterStartPos = 65;
const byte asciiLetterEndPos = 90;

//game states:
const byte greetingState = 1;
const byte mainMenuState = 2;
const byte chooseNameState = 3;
const byte inGameState = 4;
const byte leaderboardState = 5;
const byte settingsState = 6;
const byte difficultyState = 7;
const byte lcdContrastState = 8;
const byte lcdBrightnessState = 9;
const byte matrixBrightnessState = 10;
const byte howToPlayState = 11;
const byte creditsState = 12;
const byte gameEndedState = 13;


const char greetingMsgPart1[] = {'W', 'e', 'l', 'c', 'o', 'm', 'e', ' ', 't', 'o', '\0'};
const byte greetingMsgPart1StartRow = 0;
const byte greetingMsgPart1StartIndex = 3;

const char greetingMsgPart2[] = {'K', 'e', 'e', 'p', ' ', 'J', 'u', 'm', 'p', 'i', 'n', 'g', '\0'};
const byte greetingMsgPart2StartRow = 1;
const byte greetingMsgPart2StartIndex = 2;

const byte upCursorPosition = 0;
const byte downCursorPosition = 1;

const char mainMenuMsg[][17] = {
  {'S', 't', 'a', 'r', 't', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0'},
  {'L', 'e', 'a', 'd', 'e', 'r', 'b', 'o', 'a', 'r', 'd', ' ', ' ', ' ', ' ', ' ', '\0'},
  {'S', 'e', 't', 't', 'i', 'n', 'g', 's', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0'},
  {'H', 'o', 'w', ' ', 't', 'o', ' ', 'p', 'l', 'a', 'y', ' ', ' ', ' ', ' ', ' ', '\0'},
  {'C', 'r', 'e', 'd', 'i', 't', 's', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0'},
};

const byte mainMenuNumberOfOptions = 5;
const byte mainMenuRedirect[] = {chooseNameState, leaderboardState, settingsState, howToPlayState, creditsState};
byte mainMenuSelectedOption = 0;
byte mainMenuTopOption = 0;
byte mainMenuBottomOption = 1;

char chooseNameMsg[][17] = {
  {'C', 'h', 'o', 'o', 's', 'e', ' ', 'y', 'o', 'u', 'r', ' ', 'n', 'a', 'm', 'e', '\0'},
  {' ', ' ', ' ', ' ', ' ' ,'"', asciiLetterStartPos, asciiLetterStartPos, asciiLetterStartPos, asciiLetterStartPos, '"', ' ', ' ', ' ', ' ', ' ', '\0'},
};

byte firstLetterPosition = 6;
byte lastLetterPosition = 9;
byte selectedLetter = 6;
byte chooseNameTopOption = 0;
byte chooseNameBottomOption = 1;

const char leaderboardMsg[][17] = {
  {'H', 'i', 'g', 'h', ' ', 's', 'c', 'o', 'r', 'e', 's', ':', ' ', ' ', ' ', ' ', '\0'},
  {'1', '.', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0'},
  {'2', '.', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0'},
  {'3', '.', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0'},
  {'4', '.', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0'},
  {'5', '.', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0'},
};

const byte leaderboardNumberOfOptions = 6;
byte leaderboardSelectedOption = 0;
byte leaderboardTopOption = 0;
byte leaderboardBottomOption = 1;

const char settingsMsg[][17] = {
  {'D', 'i', 'f', 'f', 'i', 'c', 'u', 'l', 't', 'y', ' ', ' ', ' ', ' ', ' ', ' ', '\0'},
  {'L', 'C', 'D', ' ', 'c', 'o', 'n', 't', 'r', 'a', 's', 't', ' ', ' ', ' ', ' ', '\0'},
  {'L', 'C', 'D', ' ', 'b', 'r', 'i', 'g', 'h', 't', 'n', 'e', 's', 's', ' ', ' ', '\0'},
  {'M', 'a', 't', ' ', 'b', 'r', 'i', 'g', 'h', 't', 'n', 'e', 's', 's', ' ', ' ', '\0'},
  {'B', 'a', 'c', 'k', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0'},
};

const byte settingsNumberOfOptions = 5;
const byte settingsRedirect[] = {difficultyState, lcdContrastState, lcdBrightnessState, matrixBrightnessState, mainMenuState};
byte settingsSelectedOption = 0;
byte settingsTopOption = 0;
byte settingsBottomOption = 1;

const char gameDifficultyMsg[][17] = {
  {'G', 'a', 'm', 'e', ' ', 'd', 'i', 'f', 'f', 'i', 'c', 'u', 'l', 't', 'y', ':', '\0'},
  {'W', 'I', 'P', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '\0'},
};

byte gameDifficultyTopOption = 0;
byte gameDifficultyBottomOption = 1;

char matrixBrightnessMsg[][17] = {
  {'M', 'a', 't', ' ', 'b', 'r', 'i', 'g', 'h', 't', 'n', 'e', 's', 's', ' ', ' ', '\0'},
  {'<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>', '\0'},
};

const char emptyChar = ' ';
const char fullChar = 'X';
byte matrixBrightnessTopOption = 0;
byte matrixBrightnessBottomOption = 1;
byte matrixBrightnessValue = 6;
byte matrixBrightnessLowPos = 0;
byte matrixBrightnessHighPos = 14;

const char lcdBrightnessMsg[][17] = {
  {'L', 'C', 'D', ' ', 'b', 'r', 'i', 'g', 'h', 't', 'n', 'e', 's', 's', ' ', ' ', '\0'},
  {'W', 'I', 'P', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '\0'},
};

byte lcdBrightnessTopOption = 0;
byte lcdBrightnessBottomOption = 1;

const char lcdContrastMsg[][17] = {
  {'L', 'C', 'D', ' ', 'c', 'o', 'n', 't', 'r', 'a', 's', 't', ' ', ' ', ' ', ' ', '\0'},
  {'W', 'I', 'P', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '\0'},
};

byte lcdContrastTopOption = 0;
byte lcdContrastBottomOption = 1;

const char howToPlayMsg[][17] = {
  {'J', 'u', 'm', 'p', '-', 'b', 'u', 't', 't', 'o', 'n', ' ', ' ', ' ', ' ', ' ', '\0'},
  {'M', 'o', 'v', 'e', '-', 'j', 'o', 'y', 's', 't', 'i', 'c', 'k', ' ', ' ', ' ', '\0'},
};

byte howToPlayTopOption = 0;
byte howToPlayBottomOption = 1;

const char creditsMsg[][17] = {
  {'M', 'a', 'd', 'e', ' ', 'b', 'y', ' ', 'D', 'u', 't', 'a', ' ', 'A', '.', ' ', '\0'},
  {'G', 'i', 't', 'H', 'u', 'b', ':', ' ', 'd', 'a', 'n', 'd', 'i', '0', '8', ' ', '\0'},
};

byte creditsTopOption = 0;
byte creditsBottomOption = 1;

const char gameEndedMsg[][17] = {
  {'Y', 'o', 'u', 'r', ' ', 's', 'c', 'o', 'r', 'e', ':', ' ', ' ', ' ', ' ', ' ', '\0'},
  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0'},
};

byte gameEndedTopOption = 0;
byte gameEndedBottomOption = 1;
