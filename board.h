#pragma once
#include "game.h"
#include "card.h"

std::vector<Card> createDeck();
void initBoard();
void renderBoard();
void handleMouseDownBoard(SDL_MouseButtonEvent button);
void handleMouseUpBoard(SDL_MouseButtonEvent button);
void updateBoard();