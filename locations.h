#pragma once 
#include "card.h"

SDL_Rect PileRect(int n, int c);
SDL_Rect CellRect(int n);
SDL_Rect FlowerCellRect();
SDL_Rect FoundationRect(int n);
SDL_Rect HeldRect(int c, int x, int y, int xoff, int yoff);
SDL_Rect ButtonRect(int n);
SDL_Rect MovingRect(int x, int y);

bool inRect(int px, int py, SDL_Rect r);