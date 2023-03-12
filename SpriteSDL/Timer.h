#pragma once
#include "SDL/include/SDL.h"
#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )
class Timer
{
public:
	Timer();
	void setTimer(int temps) { maxTime = temps; };
	void refreshTimer();
	float getTimer() { return currentTimerElapsed; };
	void resetTimer();
	bool hasCompleted();
	int getTicks() { return SDL_GetTicks(); };
	float getDeltaTimer(){ return deltaTimer; };
private:
	float deltaTimer;
	float lastTime;
	float maxTime;
	float currentTimerElapsed;
};

