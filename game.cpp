// contains the function that control the main game loop
// -- initializing/destroying
// -- handling events
// -- rendering
// mainly calls other files' functions
#include "game.h"
#include "board.h"
#include "menu.h"
#include <cmath>
#include <chrono>
#include <iostream>

GameScreen screen;
SDL_Renderer* renderer;
SDL_Window* window;
bool gameRunning;

void run() {
	initialize();
	
	int startTicks, endTicks;
	int delayTicks;
	while (gameRunning) {
		startTicks = SDL_GetTicks();
		handleEvents();
		update();
		render();
		endTicks = SDL_GetTicks();

		delayTicks = 20 - (endTicks - startTicks);
		if (delayTicks > 0) SDL_Delay(delayTicks);
	}
	destroy();
}

void initialize() {
	gameRunning = false;
	int height = 1080, width = 1920;
	SDL_DisplayMode dm;
	//SDL_GetDesktopDisplayMode(0, &dm/*);
	//int height = dm.h, width = dm.w;*/
	std::cout << height << ", " << width << "\n";
	int flags = SDL_WINDOW_FULLSCREEN;
	//int flags = 0;

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0 && IMG_Init(IMG_INIT_PNG)) {
		window = SDL_CreateWindow("Shenzhen Solitaire", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, height, width, flags);
		if (window) {
			std::cout << "Window Created\n";
			SDL_SetWindowMinimumSize(window, 100, 100);
		}
		renderer = SDL_CreateRenderer(window, -1, 0);
		if (renderer) {
			std::cout << "Renderer Created\n";
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		}
	}
	else {
		std::cout << "SDL Initialization failed, " << SDL_GetError() << "\n";
		return;
	}

	SDL_Surface* image = IMG_Load("imgs/button-icon.png");
	SDL_SetWindowIcon(window, image);
	
	srand(time(NULL));
	screen = Board;
	gameRunning = true;

	initBoard();
}

void handleEvents() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				gameRunning = false;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_q) {
					gameRunning = false;
				}
				if (event.key.keysym.sym == SDLK_r) {
					initBoard();
				}
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					switch (screen) {
					case StartMenu:
						gameRunning = false;
						break;
					case MenuOverlay:
						screen = Board;
						break;
					case Board:
						screen = MenuOverlay;
						break;
					}
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				handleMouseDown(event.button);
				break;
			case SDL_MOUSEBUTTONUP:
				handleMouseUp(event.button);
				break;
		}
	}
}

void handleMouseDown(SDL_MouseButtonEvent button) {
	if (screen == Board) {
		handleMouseDownBoard(button);
	}
}

void handleMouseUp(SDL_MouseButtonEvent button) {
	if (screen == Board) {
		handleMouseUpBoard(button);
	}
}

void update() {
	if (screen == Board) {
		updateBoard();
	}
}

void render() {
	switch (screen) {
	case MenuOverlay:
		renderMenuOverlay();
		break;
	case Board:
		renderBoard();
		break;
	}
	SDL_RenderPresent(renderer);
}

void destroy() {
	if (renderer) {
		SDL_DestroyRenderer(renderer);
	}
	if (window) {
		SDL_DestroyWindow(window);
	}
	SDL_Quit();
}