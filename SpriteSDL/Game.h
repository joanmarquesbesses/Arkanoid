#pragma once

#include "SDL/include/SDL.h"
#pragma comment( lib, "SDL/libx86/SDL2.lib" )
#pragma comment( lib, "SDL/libx86/SDL2main.lib" )
#include "SDL_image/include/SDL_image.h"
#pragma comment(lib, "SDL_image/libx86/SDL2_image.lib")

#include "Entity.h"

#define WINDOW_WIDTH	1024
#define WINDOW_HEIGHT	768
#define MAX_KEYS		256
#define MAX_SHOTS		32

class Game
{
public:
	Game();
	~Game();

	bool Init();
	void Release();
	
	bool Input();
	bool Update();
	void Draw();

private:
	SDL_Window *Window;
	SDL_Renderer *Renderer;

	Entity Player, Shots[MAX_SHOTS];
	int idx_shot;

	enum KEY_STATE { KEY_IDLE, KEY_DOWN, KEY_REPEAT, KEY_UP	};
	KEY_STATE keys[MAX_KEYS]; 
	SDL_Surface* loadedSurface[3];
	SDL_Texture* imgT[3];
	SDL_Rect BG[2];
};
