#ifndef PLAYER_H
#define PLAYER_H
#include "SDL/SDL.h"


class Player
{
    public:
        Player();
        int stateTimer;
        bool isPunching;
        bool isJumping;
        bool isDucking;
        bool hasSuper;
        bool isInvisible;
        int invisibleTimer;
        Uint8 yinyangs;
        Uint8 lives;
        Uint8 opacity;
        void hurt(int a);
        int leftside;
        int rightside;
    protected:
    private:
};

#endif // PLAYER_H
