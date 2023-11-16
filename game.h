#pragma once
#include "SDL.h"
#include "SDL_image.h"

enum GameScreen {
	StartMenu,
	MenuOverlay,
	Board
};

// defines the functions and objects used in game.cpp
extern GameScreen screen;
extern SDL_Renderer* renderer;
extern SDL_Window* window;
extern bool gameRunning;

void run();
void initialize();
void handleEvents();
void update();
void render();
void destroy();

void handleMouseDown(SDL_MouseButtonEvent button);
void handleMouseUp(SDL_MouseButtonEvent button);