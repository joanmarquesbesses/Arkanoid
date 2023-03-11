#include "Game.h"
#include <math.h>

Game::Game() {}
Game::~Game(){}

bool Game::Init()
{
	//Initialize SDL with all subsystems
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	//Create our window: title, x, y, w, h, flags
	Window = SDL_CreateWindow("Spaceship: arrow keys + space", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (Window == NULL)
	{
		SDL_Log("Unable to create window: %s", SDL_GetError());
		return false;
	}
	//Create a 2D rendering context for a window: window, device index, flags
	Renderer = SDL_CreateRenderer(Window, -1, 0);
	if (Renderer == NULL)
	{
		SDL_Log("Unable to create rendering context: %s", SDL_GetError());
		return false;
	}
	//Initialize keys array
	for (int i = 0; i < MAX_KEYS; ++i)
		keys[i] = KEY_IDLE;

	//Init variables
	loadedSurface[0] = IMG_Load("Link Espalda.png");
	Link.Init((WINDOW_WIDTH/2)-(loadedSurface[0]->w / 2), (WINDOW_HEIGHT - (loadedSurface[0]->h)) , loadedSurface[0]->w, loadedSurface[0]->h, 5);
	if (loadedSurface[0] == NULL) {
		SDL_GetError();
	}
	imgT[0] = SDL_CreateTextureFromSurface(Renderer, loadedSurface[0]);
	if (imgT == NULL) {
		SDL_GetError();
	}
	loadedSurface[1] = IMG_Load("shot.png");
	if (loadedSurface[1] == NULL) {
		SDL_GetError();
	}
	imgT[1] = SDL_CreateTextureFromSurface(Renderer, loadedSurface[1]);
	if (imgT == NULL) {
		SDL_GetError();
	}
	loadedSurface[2] = IMG_Load("background.png");
	if (loadedSurface[2] == NULL) {
		SDL_GetError();
	}
	imgT[2] = SDL_CreateTextureFromSurface(Renderer, loadedSurface[2]);
	if (imgT == NULL) {
		SDL_GetError();
	}

	for (size_t i = 0; i < 2; i++)
	{
		BG[i].x = 0;
		BG[i].y = 0;
		BG[i].w = 3072;
		BG[i].h = 768;
	}

	BG[1].x = BG[1].w;

	return true;
}
void Game::Release()
{
	//Clean up all SDL initialized subsystems
	SDL_Quit();
}
bool Game::Input()
{
	SDL_Event event;
	if (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)	return false;
	}

	SDL_PumpEvents();
	const Uint8* keyboard = SDL_GetKeyboardState(NULL);
	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keyboard[i])
			keys[i] = (keys[i] == KEY_IDLE) ? KEY_DOWN : KEY_REPEAT;
		else
			keys[i] = (keys[i] == KEY_REPEAT || keys[i] == KEY_DOWN) ? KEY_UP : KEY_IDLE;
	}

	return true;
}
bool Game::Update()
{
	//Read Input
	if (!Input())	return true;

	int x, y, w, h;
	Link.GetRect(&x, &y, &w, &h);

	//Process Input
	int fx = 0, fy = 0;
	if (keys[SDL_SCANCODE_ESCAPE] == KEY_DOWN)	return true;
	if (keys[SDL_SCANCODE_UP] == KEY_REPEAT && Link.GetY() > 1)	fy = -1;
	if (keys[SDL_SCANCODE_DOWN] == KEY_REPEAT && (Link.GetY() + h) < WINDOW_HEIGHT - 1)	fy = 1;
	if (keys[SDL_SCANCODE_LEFT] == KEY_REPEAT && Link.GetX() > 1)	fx = -1;
	if (keys[SDL_SCANCODE_RIGHT] == KEY_REPEAT && (Link.GetX() + w) < WINDOW_WIDTH - 1)	fx = 1;
	if (keys[SDL_SCANCODE_SPACE] == KEY_DOWN)
	{
		if (!Link.shoot) {
			
			Link.shoot = true;
		}
		else {
			
			Link.shoot = false;
		}
	}

	//Logic
	//Link update
	Link.Move(fx, fy);
	//Shots update
	

	BG[1].x-= 8;
	BG[0].x-= 8;

	for (size_t i = 0; i < 2; i++)
	{
		if (BG[i].x + BG[i].w <= 0) {
			BG[i].x = BG[i].w;
		}
	}

		
	return false;
}
void Game::Draw()
{
	//Set the color used for drawing operations
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
	//Clear rendering target
	SDL_RenderClear(Renderer);

	SDL_RenderCopy(Renderer, imgT[2], NULL, &BG[0]);
	SDL_RenderCopy(Renderer, imgT[2], NULL, &BG[1]);

	//Draw Link
	SDL_Rect rc;
	Link.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	SDL_RenderCopy(Renderer, imgT[0], NULL, &rc);
	
	//Draw shots
	//SDL_SetRenderDrawColor(Renderer, 192, 0, 0, 255);
	

	//Update screen
	SDL_RenderPresent(Renderer);

	SDL_Delay(10);	// 1000/10 = 100 fps max
}