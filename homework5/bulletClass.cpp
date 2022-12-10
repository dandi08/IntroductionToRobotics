#include "bulletClass.h"

Bullet::Bullet(byte xPos, byte yPos, byte xVelocity, byte yVelocity, int bulletMoveDelay) {
  this->xPos = xPos;
  this->yPos = yPos;
  this->xVelocity = xVelocity;
  this->yVelocity = yVelocity;
  this->bulletTimer = millis();
  this->bulletMoveDelay = bulletMoveDelay;
}

void Bullet::moveBullet() {
  if (millis() - bulletTimer > bulletMoveDelay) {
    bulletTimer = millis();
    xPos += xVelocity;
    yPos += yVelocity;
  }
}

byte Bullet::getXPos() {
  return xPos;
}

byte Bullet::getYPos() {
  return yPos;
}

int Bullet::getBulletMoveDelay() {
  return bulletMoveDelay;
}

unsigned long long Bullet::getBulletTimer() {
  return bulletTimer;
}