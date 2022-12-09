#define LED_h

#include <Arduino.h>

class Bullet {
  private:
    byte xPos;
    byte yPos;
    byte xVelocity;
    byte yVelocity;
    int bulletMoveDelay = 250;
    unsigned long long bulletTimer;

  public:
    Bullet(byte xPos, byte yPos, byte xVelocity, byte yVelocity, int bulletMoveDelay);
    void moveBullet();
    byte getXPos();
    byte getYPos();
    int getBulletMoveDelay();
    unsigned long long getBulletTimer();
};
