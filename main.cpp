
//The headers
//{
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_mixer.h"
#include <string>
#include <fstream>
#include "Timer.h"
#include "Player.h"
#include "Enemy.h"

#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <deque>

using std::string;
using std::deque;
//}

//CONSTANTS
//{
const int screenWidth = 300;	// Pretty
const int screenHeight = 200;	// self
const int screenBPP = 32;		// explanatory
//}

//GLOBAL DEFINES
//{
#define PI 3.1415926f
#define actionbutton SDLK_f
#define upbutton SDLK_w
#define downbutton SDLK_s
#define endbutton SDLK_g
#define restartbutton SDLK_t
//}

//GLOBAL OBJECTS
//{
SDL_Surface *screen = NULL;     // main screen

SDL_Event event;
//}

//GLOBAL VARIABLES
//{
int gameModeNum = 0;

bool quitAll = false;

Uint8 score[6] = {1,0,0,0,0,0};
//}

//init standard SDL functions
//{
SDL_Surface *loadImage( string filename );

void applySurface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect clip, Uint8 blitAlpha = 255 );

bool initScreen();//}

//init general game functions
//{
Enemy newEnemy(int t);
SDL_Rect newRect(int x, int y, int w, int h);
void createMountains(SDL_Surface* thisSurface);
void createStatic(SDL_Surface* thisSurface);
//}

//init game modes
//{
void startEngine();
void gameMain();
void endEngine();
//}

//game mode function pointer array
void (*gameMode[3])() = {&startEngine, &gameMain, &endEngine};

int main( int argc, char *argv[] )
{
    //init phase
    //{
    initScreen();
    //}
    SDL_ShowCursor(SDL_DISABLE);
    //main run
    //{
    while(quitAll == false)
    {
    gameMode[gameModeNum]();
    }
    //}

    Mix_CloseAudio();
    SDL_Quit();

    return 0;
}

//Game modes
//{
void startEngine()
{

    //initialize starting sequence
    //{
    const string pressStartString = "pressStart.png";
    const string pressStartScreenString = "pressStartScreen.png";
    Mix_Music *music = Mix_LoadMUS("openingtheme.wav");
    Timer fps;
    Mix_PlayMusic( music, -1 );

    int framesPerSecond = 30;

    Uint8 transparency = 5;
    SDL_Surface *startFlashing = loadImage(pressStartString);
    SDL_Surface *pressStartBackground = loadImage(pressStartScreenString);

    SDL_Surface *staticForeground = loadImage("staticbase.png");
    createStatic (staticForeground);
    bool switchAlpha = true;
    bool quit = false;
    //}
    //main loop
    //{
    while(quit == false)
    {

        fps.start();

        SDL_FillRect(screen,&screen->clip_rect,SDL_MapRGB(screen->format,0x38, 0x90, 0xf2));
        applySurface(0,0,pressStartBackground,screen, newRect(0,0,pressStartBackground->w,pressStartBackground->h));
        applySurface(170,(screen->h*3/4),startFlashing,screen,newRect(0,0,startFlashing->w,startFlashing->h),transparency);
        applySurface(0,0,staticForeground,screen,newRect(0,0,300,200),40);

        if(switchAlpha)
        {
            transparency += 10;
            if(transparency == 255)
            {
                switchAlpha = false;
            }
        }else{
            transparency -= 10;
            if(transparency == 5)
            {
                switchAlpha = true;
            }
        }

        if( fps.get_ticks() < 1000 / framesPerSecond )
        {
            //Sleep the remaining frame time
            SDL_Delay( ( 1000 / framesPerSecond ) - fps.get_ticks() );
        }
        SDL_Flip( screen );

        while( SDL_PollEvent( &event ) )
        {
            //If the user has Xed out the window
            if( event.type == SDL_QUIT )
            {
                quitAll = true;
                quit = true;
            }else if( event.type == SDL_KEYDOWN )
            {
                //Set the proper message surface
                switch( event.key.keysym.sym )
                {
                    case actionbutton:
                    quit = true;
                    gameModeNum = 1;
                    break;
                    default: ;
                }
            }
        }
    }
    //exiting the mode
    Mix_FadeOutMusic(1000);
    SDL_FreeSurface(staticForeground);
    SDL_FreeSurface(pressStartBackground);
    SDL_FreeSurface(startFlashing);
    Mix_FreeMusic( music );
    //}
}

void gameMain()
{

    Timer fps;
    SDL_Surface *mountainsBackground = loadImage("mountainsbase.png");
    createMountains(mountainsBackground);

    SDL_Surface *staticForeground = loadImage("staticbase.png");
    createStatic (staticForeground);

    SDL_Surface *HUD = loadImage("hudbase.png");

    SDL_Surface *pSurface = loadImage("playersprite.png");
    SDL_Surface *aSurface = loadImage("duckobs.png");
    SDL_Surface *gSurface = loadImage("jumpobs.png");
    SDL_Surface *eSurface = loadImage("punchobs.png");
    SDL_Surface *hSurface = loadImage("hudobs.png");
    SDL_Surface *ySurface = loadImage("yinyang.png");

    Mix_Chunk *jumpM = Mix_LoadWAV("jump.wav");
    Mix_Chunk *hurtM = Mix_LoadWAV("hurt.wav");
    Mix_Chunk *duckM = Mix_LoadWAV("grinding.wav");
    Mix_Chunk *punchM = Mix_LoadWAV("punch.wav");
    Mix_Chunk *killM = Mix_LoadWAV("kill.wav");
    Mix_Chunk *superM = Mix_LoadWAV("super.wav");

    Mix_Music *music = Mix_LoadMUS("leveltheme.wav");
    //Mix_PlayMusic( music, -1 );
    Mix_FadeInMusic(music, -1,1000);

    int ticksPerFrame = 9;
    int quit = false;
    int frame = 0;

    deque<Enemy> objectOverview(14, newEnemy(0));
    /*deque<Projectile> projectileQue;
    deque<Ground> groundQue;
    deque<Enemy> enemyQue;*/
    Player kid;

    //main loop
    //{

    //calculate status

    while(quit==false)
    {
        fps.start();
        frame++;
        frame %= 2400;
        if(frame%5 == 0)
        {
            for(int i = 0; i <6; i++)
            {
                score[i]++;
                score[i] %= 10;
                if(score[i] != 0)
                {
                    break;
                }
            }
        }
        //calculate super
        if((!kid.hasSuper) && kid.yinyangs >= 50)
        {
            kid.hasSuper = true;
            kid.isDucking = false;
            kid.isJumping = false;
            kid.isPunching = false;
            kid.stateTimer = 750;
            Mix_PlayChannel(1,superM,-1);
        }
        if(!kid.hasSuper)
        {
            //Get the keystates
            Uint8 *keystates = SDL_GetKeyState( NULL );
            //calculate player status
            if(!kid.isJumping)
            {
                if(!kid.isPunching)
                {
                    if(keystates[downbutton])
                    {
                        if(!kid.isDucking)
                        {
                            kid.isDucking = true;
                            Mix_PlayChannel(1, duckM, -1);
                        }
                    }else{
                        if(kid.isDucking)
                        {
                            kid.isDucking = false;
                            Mix_HaltChannel(1);
                        }
                        if(keystates[upbutton])
                        {
                            kid.isJumping = true;
                            Mix_PlayChannel(2,jumpM,0);
                            kid.stateTimer = 100;
                            //posssibly perform one frame of jumping
                        }else if(keystates[actionbutton])
                        {
                            kid.isPunching = true;
                            Mix_PlayChannel(2,punchM,0);
                            kid.stateTimer = 40;
                        }
                    }
                }else{
                    kid.stateTimer--;
                    if(kid.stateTimer == 0)
                    {
                        kid.isPunching = false;
                    }
                }
            }else{
                kid.stateTimer--;

                if(kid.stateTimer == 0)
                {
                    kid.isJumping = false;
                }
            }
        }else{
            kid.stateTimer--;
            //printf("%d\n",kid.stateTimer);
            if(kid.stateTimer%15 == 0)
            {
                kid.yinyangs--;
                printf("%d\n",kid.yinyangs);
            }
            if(kid.stateTimer == 0)
            {
                kid.hasSuper = false;
                Mix_HaltChannel(1);
                kid.isInvisible = true;
                kid.invisibleTimer = (1000/ticksPerFrame);
                kid.opacity = 128;
            }
        }
        if(kid.isInvisible)
        {
            kid.invisibleTimer--;
            if (kid.invisibleTimer == 0)
            {
                kid.isInvisible = false;
                kid.opacity = 255;
            }
        }


        //Calculate creations
        if(frame%25 == 0)
        {
            objectOverview.pop_front();

            int newtype = 0;

            if(objectOverview[12].type == 3)
            {
                if(rand()%10 <2)
                {
                    newtype = 3;
                }else{
                    newtype = rand()%2;
                }
            }else if(objectOverview[12].type == 4){
                newtype = 0;
            }else if(objectOverview[12].type >1 ){
                newtype = rand()%2;
            }else if(objectOverview[11].type >1){
                newtype = rand()%2;
            }else if(objectOverview[10].type >1){
                newtype = rand()%2;
            }else{
                newtype = rand()%5;
            }
            objectOverview.push_back(newEnemy(newtype));
            //printf("",objectOverview.size());
        }
        for ( int i = 0; i < objectOverview.size(); i++)
        {
            objectOverview[i].leftside--;
        }

        //Calculate interactions

        for(int i = 3; i<5; i++)
        {
            switch(objectOverview[i].type)
            {
                case 1:
                    if(kid.isJumping && kid.stateTimer<75 && kid.stateTimer > 25)
                    {
                        break;
                    }
                    if(!(objectOverview[i].leftside > kid.rightside || objectOverview[i].leftside+16 < kid.leftside))
                    {
                        objectOverview[i].type = 0;
                        if(!kid.hasSuper || kid.isInvisible)
                        {
                            kid.yinyangs++;
                        }
                    }
                    break;

                case 2:
                    if(kid.isJumping || kid.isInvisible)
                    {
                        break;
                    }
                    if(!(objectOverview[i].leftside > kid.rightside || objectOverview[i].leftside+16 < kid.leftside))
                    {
                        if(kid.hasSuper)
                        {
                            objectOverview[i].type = 0;
                            Mix_PlayChannel(-1,killM,0);
                        }else{
                            kid.hurt(ticksPerFrame);
                        }
                    }
                    break;

                case 3:
                    if(kid.isDucking || kid.isInvisible)
                    {
                        break;
                    }
                    if(!(objectOverview[i].leftside > kid.rightside || objectOverview[i].leftside+13 < kid.leftside))
                    {
                        if(!kid.hasSuper)
                        {
                            kid.hurt(ticksPerFrame);
                        }else{
                            objectOverview[i].type = 0;
                            Mix_PlayChannel(-1,killM,0);
                        }
                    }

                case 4:
                    if(!(objectOverview[i].leftside > kid.rightside || objectOverview[i].leftside+52 < kid.leftside))
                    {
                        if(kid.isPunching || kid.hasSuper)
                        {
                            objectOverview[i].type = 0;
                            Mix_PlayChannel(-1,killM,0);
                        }else if(!kid.isInvisible){
                            kid.hurt(ticksPerFrame);
                        }
                    }
                    break;
            }

        }

        //calcutale graphics

        if (frame%2400 > 1200)
        {
            applySurface(0,0,mountainsBackground,screen,newRect((frame%2400/4),0,600-frame%2400/4,100));
            applySurface(600-(frame%2400)/4,0,mountainsBackground,screen,newRect(0,0,(frame%2400/4)-300,100));
        }else{
            applySurface(0,0,mountainsBackground,screen,newRect((frame%2400/4),0,300,100));
        }
        if (frame%600 > 300)
        {
            applySurface(0,100,mountainsBackground,screen,newRect((frame%600),100,600-(frame%600),100));
            applySurface(600-(frame%600),100,mountainsBackground,screen,newRect(0,100,(frame%600)-300,100));
        }else{
            applySurface(0,100,mountainsBackground,screen,newRect(frame%600,100,300,100));
        }
        for (int i = 0; i < objectOverview.size(); i++)
        {
            switch(objectOverview[i].type)
            {
                case 1:
                    applySurface(objectOverview[i].leftside,163, ySurface,screen, newRect(16* ((frame%600/15) % 8),0,16,16)); break;
                case 2:
                    applySurface(objectOverview[i].leftside,158, gSurface,screen, newRect(16* ((frame%600/15) % 4),30*objectOverview[i].frametype,16,30)); break;
                case 3:
                    applySurface(objectOverview[i].leftside,140, aSurface,screen, newRect(17* ((frame%600/15) % 2),13*objectOverview[i].frametype,17,13)); break;
                case 4:
                    applySurface(objectOverview[i].leftside,130, eSurface,screen, newRect(40*((frame%600/10) % 6),52*objectOverview[i].frametype,40,52)); break;
            }
        }

        if(kid.lives == 0)
        {
            gameModeNum = 2;
            kid.opacity = 40;
            quit = true;
        }

        if(kid.hasSuper)
        {
            applySurface(kid.leftside-10,135,pSurface,screen,newRect(54 + (96*((frame%600/15) % 2)),103,70,46));
        }else if(kid.isDucking){
            applySurface(kid.leftside,135+46-25,pSurface,screen,newRect(64* ((frame%600/15) % 4),46,64,25),kid.opacity);
        }else if(kid.isJumping){
            applySurface(kid.leftside,135-30*sin((100-kid.stateTimer)*PI/100),pSurface,screen,newRect(64* ((frame%600/15) % 4),71,32,32),kid.opacity);
        }else if(kid.isPunching){
            applySurface(kid.leftside,135,pSurface,screen,newRect(0,104,37,46),kid.opacity);
        }else{
            applySurface(kid.leftside,135,pSurface,screen,newRect(32* ((frame%600/15) % 8),0,32,46),kid.opacity);
        }
        SDL_FillRect(HUD, &HUD->clip_rect,SDL_MapRGB(HUD->format,128,255,255));
        applySurface(10,4,hSurface,HUD,newRect(0,0,18,16));
        applySurface(30,4,hSurface,HUD,newRect(0,0,18,16));
        applySurface(50,4,hSurface,HUD,newRect(0,0,18,16));
        switch(kid.lives)
        {

            case 3:
                applySurface(50,4,hSurface,HUD,newRect(18,0,18,16));
            case 2:
                applySurface(30,4,hSurface,HUD,newRect(18,0,18,16));
            case 1:
                applySurface(10,4,hSurface,HUD,newRect(18,0,18,16));
                break;
        }

        applySurface(70,4,hSurface,HUD,newRect(40,0,106,16));
        applySurface(73,7,hSurface,HUD,newRect(146,3,kid.yinyangs*2,10));

        int tempint = 5;
        bool tempcheck = false;
        while(tempcheck == false)
        {
            if (score[tempint] == 0)
            {
                tempint--;
            }else{
                tempcheck = true;
            }
        }
        for(int i = tempint; i>=0; i--)
        {
            applySurface(280-(19*i),2,hSurface,screen,newRect(19*score[i],16,19,19));
        }

        //printf("%d\n",kid.yinyangs);
        applySurface(0,0,HUD,screen,newRect(0,0,300,30));
        applySurface(0,0,staticForeground,screen,newRect(0,0,300,200),35);

        if((frame == 0) && (ticksPerFrame != 3))
        {
            ticksPerFrame--;
        }
        if( fps.get_ticks() < ticksPerFrame )
        {
            //Sleep the remaining frame time
            SDL_Delay( ticksPerFrame - fps.get_ticks() );
        }

        //printf("%d/n",kid.lives);
        SDL_Flip( screen );

        while( SDL_PollEvent( &event ) )
        {
            //If the user has Xed out the window
            if( event.type == SDL_QUIT )
            {
                quitAll = true;
                quit = true;
            }
        }
    }
    //exiting the mode
    Mix_FadeOutMusic(1000);
    SDL_FreeSurface(mountainsBackground);
    SDL_FreeSurface(staticForeground);
    SDL_FreeSurface(HUD);
    SDL_FreeSurface(pSurface);
    SDL_FreeSurface(aSurface);
    SDL_FreeSurface(gSurface);
    SDL_FreeSurface(eSurface);
    SDL_FreeSurface(hSurface);
    SDL_FreeSurface(ySurface);
    Mix_FreeChunk(jumpM);
    Mix_FreeChunk(hurtM);
    Mix_FreeChunk(duckM);
    Mix_FreeChunk(punchM);
    Mix_FreeChunk(killM);
    Mix_FreeChunk(superM);
    Mix_FreeMusic(music);
    //}
}

void endEngine()
{
    Timer fps;
    bool quit = false;
    int framesPerSecond = 100;
    SDL_Surface *fSurface = loadImage("endchoice.png");
    applySurface(0, 0, fSurface, screen, newRect(0, 0, 300, 200), 255);
    SDL_Flip(screen);
    while(!quit)
    {
        fps.start();

        if( fps.get_ticks() < 1000 / framesPerSecond )
        {
            //Sleep the remaining frame time
            SDL_Delay( ( 1000 / framesPerSecond ) - fps.get_ticks() );
        }

        while( SDL_PollEvent( &event ) )
        {
            //If the user has Xed out the window
            if( event.type == SDL_QUIT )
            {
                quitAll = true;
                quit = true;
            }else if( event.type == SDL_KEYDOWN )
            {
                //Set the proper message surface
                switch( event.key.keysym.sym )
                {
                    case endbutton:
                        quitAll = true; //remove this later
                        quit = true;
                        break;
		case restartbutton:
		  quit = true;
		  gameModeNum = 1;
                    default: ;
                }
            }
        }
    }
    SDL_FreeSurface(fSurface);
}
//}


//Custom functions
//{
Enemy newEnemy(int t)
{
    Enemy en1;
    en1.type = t;
    en1.leftside = 300;
    en1.frametype = rand()%2;
    return en1;
}

SDL_Rect newRect(int x, int y, int w, int h)
{
    SDL_Rect tempRect;
    tempRect.x = x;
    tempRect.y = y;
    tempRect.w = w;
    tempRect.h = h;

    return tempRect;

}


void createMountains(SDL_Surface* thisSurface)
{
    int totalWidth = thisSurface->w;
    int currentWidth = 0;
    int incHeight = 50;
    int incRandom = 25;
    srand(SDL_GetTicks());

    while(currentWidth <= totalWidth)
    {
        if(currentWidth < 150 || currentWidth > 650)
        {
            incRandom -= 1;
        }else if(currentWidth > 300)
        {
            incRandom += 1;
        }
        SDL_Rect tempRect = newRect(currentWidth,0,1,incHeight+(incRandom/3.0)*sin(8*PI*currentWidth/totalWidth));
        SDL_FillRect(thisSurface,&tempRect, SDL_MapRGB(thisSurface->format,0x38, 0x90, 0xf2));
        currentWidth++;
    }
}

void createStatic(SDL_Surface* thisSurface)
{
    srand(SDL_GetTicks());
    SDL_Surface *tempSurface = loadImage("dot.png");

    for(int t = 0; t < 20000; t++)
    {
        SDL_FillRect(tempSurface, &tempSurface->clip_rect, SDL_MapRGB(tempSurface->format,rand()%256,rand()%256,rand()%256));
        applySurface(rand()%300,rand()%200,tempSurface,thisSurface,tempSurface->clip_rect,64);
    }
}

//}
//standard SDL functions
//{
SDL_Surface *loadImage( string filename )
{
    //The image that's loaded
    SDL_Surface* loadedImage = NULL;

    //The optimized surface that will be used
    SDL_Surface* optimizedImage = NULL;

    //Load the image
    loadedImage = IMG_Load( filename.c_str() );

    //If the image loaded
    if( loadedImage != NULL )
    {
        //Create an optimized surface
        optimizedImage = SDL_DisplayFormat( loadedImage );

        //Free the old surface
        SDL_FreeSurface( loadedImage );

        //If the surface was optimized
        if( optimizedImage != NULL )
        {
            //Color key surface
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 0x80, 0xFF, 0xFF ) );
        }
    }

    //Return the optimized surface
    return optimizedImage;
}

void applySurface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect clip, Uint8 blitAlpha)
{
    //Holds offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    SDL_SetAlpha(source, SDL_SRCALPHA, blitAlpha);

    //Blit
    SDL_BlitSurface(source, &clip, destination, &offset);
}

bool initScreen()
{
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }
    freopen( "CON", "w", stdout );
    freopen( "CON", "w", stderr );

    //Set up the screen
    screen = SDL_SetVideoMode( screenWidth, screenHeight, screenBPP, SDL_SWSURFACE | SDL_RESIZABLE | SDL_FULLSCREEN);

    //If there was an error in setting up the screen
    if( screen == NULL )
    {
        return false;
    }

    //Initialize SDL_mixer
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 1, 4096 ) == -1 )
    {
        return false;
    }

    //Set the window caption
    SDL_WM_SetCaption( "Countryside Kung-fu Champion", NULL );

    //If everything initialized fine
    return true;
}
//}
