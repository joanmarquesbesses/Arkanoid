#include "Game.h"
#include <math.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h> 

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
	idx_shot = 0;
	loadedSurface[0] = IMG_Load("Link Espalda.png");
	Link.Init((WINDOW_WIDTH/2)-(loadedSurface[0]->w / 2), (WINDOW_HEIGHT - (loadedSurface[0]->h*2)) - 10 , loadedSurface[0]->w * 2, loadedSurface[0]->h * 2, 5);
	if (loadedSurface[0] == NULL) {
		SDL_GetError();
	}
	imgT[0] = SDL_CreateTextureFromSurface(Renderer, loadedSurface[0]);
	if (imgT == NULL) {
		SDL_GetError();
	}
	loadedSurface[1] = IMG_Load("Master Sword.png");
	if (loadedSurface[1] == NULL) {
		SDL_GetError();
	}
	imgT[1] = SDL_CreateTextureFromSurface(Renderer, loadedSurface[1]);
	if (imgT == NULL) {
		SDL_GetError();
	}
	loadedSurface[2] = IMG_Load("bf.png");
	if (loadedSurface[2] == NULL) {
		SDL_GetError();
	}
	imgT[2] = SDL_CreateTextureFromSurface(Renderer, loadedSurface[2]);
	if (imgT == NULL) {
		SDL_GetError();
	}
	loadedSurface[3] = IMG_Load("Goblin.png");
	if (loadedSurface[3] == NULL) {
		SDL_GetError();
	}
	imgT[3] = SDL_CreateTextureFromSurface(Renderer, loadedSurface[3]);
	if (imgT == NULL) {
		SDL_GetError();
	}

	for (size_t i = 0; i < 2; i++)
	{
		if (loadedSurface[2] != NULL) {
			BG[i].w = loadedSurface[2]->w *4;
			BG[i].h = loadedSurface[2]->h * 4;
		}
		BG[i].x = WINDOW_WIDTH/2 - BG[i].w/2;
		BG[i].y = 0;
	}

	BG[1].y = -(BG[1].h);
	TimeToSpawn = 2000;
	EnemyTimer.setTimer(TimeToSpawn);
	Enemies.resize(0);
	ShootTimer.setTimer(300);

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
	ShootTimer.refreshTimer();
	//Process Input
	int fx = 0, fy = 0;
	if (keys[SDL_SCANCODE_ESCAPE] == KEY_DOWN)	return true;
	//if (keys[SDL_SCANCODE_UP] == KEY_REPEAT && Link.GetY() > 1)	fy = -1;
	//if (keys[SDL_SCANCODE_DOWN] == KEY_REPEAT && (Link.GetY() + h) < WINDOW_HEIGHT - 1)	fy = 1;
	if (keys[SDL_SCANCODE_LEFT] == KEY_REPEAT && Link.GetX() > 1)	fx = -1;
	if (keys[SDL_SCANCODE_RIGHT] == KEY_REPEAT && (Link.GetX() + w) < WINDOW_WIDTH - 1)	fx = 1;
	if (keys[SDL_SCANCODE_SPACE] == KEY_DOWN && ShootTimer.hasCompleted())
	{
		Sword[idx_shot].Init(x + (w/2) - (loadedSurface[1]->w/4), y, loadedSurface[1]->w/2, loadedSurface[1]->h/2, 10);
		idx_shot++;
		idx_shot %= MAX_SHOTS;
		ShootTimer.resetTimer();
	}

	//Logic
	//Link update
	Link.Move(fx, fy);
	//Shots update
	
	EnemyTimer.refreshTimer();
	if (EnemyTimer.hasCompleted()) {
		Enemies.push_back(new Entity);
		Enemies[Enemies.size() - 1]->Init(rand() % (WINDOW_WIDTH-(loadedSurface[3]->w / 4)), -loadedSurface[3]->h, loadedSurface[3]->w / 4, loadedSurface[3]->h / 4, 2);
		if (TimeToSpawn > 500) {
			TimeToSpawn -= 100;
		}
		EnemyTimer.setTimer(TimeToSpawn);
		EnemyTimer.resetTimer();
	}

	SDL_Rect r1,r2;

	Link.GetRect(&r1.x, &r1.y, &r1.w, &r1.h);

	for (int i = 0; i < Enemies.size(); ++i)
	{
		Enemies[i]->Move(0, 1);
		Enemies[i ]->GetRect(&r2.x, &r2.y, &r2.w, &r2.h);
		if (Enemies[i]->GetY() > WINDOW_HEIGHT)
		{
			delete Enemies[i];
			Enemies.erase(std::next(Enemies.begin(), i));
			Link.hit();
			if (!Link.IsAlive())
			{
				return true;
			}
		}
		else if (SDL_HasIntersection(&r1, &r2)) {
			delete Enemies[i];
			Enemies.erase(std::next(Enemies.begin(), i));
			Link.hit();
			if (!Link.IsAlive())
			{
				return true;
			}
		}
	}

	for (int i = 0; i < MAX_SHOTS; ++i)
	{
		if (Sword[i].IsAlive())
		{
			Sword[i].Move(0, -1);
			if (Sword[i].GetY() < 0 - h) Sword[i].ShutDown();
			else {
				Sword[i].GetRect(&r1.x, &r1.y, &r1.w, &r1.h);
				for (int x = 0; x < Enemies.size(); ++x)
				{
					Enemies[x]->GetRect(&r2.x, &r2.y, &r2.w, &r2.h);
					if (SDL_HasIntersection(&r1,&r2))
					{
						delete Enemies[x];
						Enemies.erase(std::next(Enemies.begin(), x));
						Sword[i].ShutDown();
					}
				}
			}
		}
	}

	BG[1].y+= 4;
	BG[0].y+= 4;

	for (size_t i = 0; i < 2; i++)
	{
		if (BG[i].y >= WINDOW_HEIGHT) {
			if (i == 0) {
				BG[0].y = BG[1].y - BG[1].h;
			}
			else {
				BG[1].y = BG[0].y - BG[0].h;
			}
			
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
	
	//Draw shots
	SDL_SetRenderDrawColor(Renderer, 192, 0, 0, 255);
	for (int i = 0; i < MAX_SHOTS; ++i)
	{
		if (Sword[i].IsAlive())
		{
			Sword[i].GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
			SDL_RenderCopyEx(Renderer, imgT[1], NULL, &rc, 0, NULL, SDL_FLIP_VERTICAL);
		}
	}

	Link.GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
	SDL_RenderCopy(Renderer, imgT[0], NULL, &rc);

	for (int i = 0; i < Enemies.size(); ++i)
	{
		Enemies[i]->GetRect(&rc.x, &rc.y, &rc.w, &rc.h);
		SDL_RenderCopy(Renderer, imgT[3], NULL, &rc);
	}
	
	//Update screen
	SDL_RenderPresent(Renderer);

	SDL_Delay(10);	// 1000/10 = 100 fps max
}