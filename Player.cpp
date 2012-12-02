#include "Player.h"

Player::Player()
{
    stateTimer = 0;
    isJumping = false;
    isDucking = false;
    isInvisible = false;
    isPunching = false;
    invisibleTimer = 0;
    hasSuper= false;
    lives = 3;
    yinyangs = 0;
    opacity = 255;
    leftside = 30;
    rightside = 62;
}

void Player::hurt(int a)
{
    isInvisible = true;
    invisibleTimer = (1000/a);
    opacity = 128;
    lives--;
   // printf("bla\n");
}
