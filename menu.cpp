#include "game.h"
#include "board.h"
#include "menu.h";

void renderMenuOverlay() {
	renderBoard();

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 192);
	SDL_RenderFillRect(renderer, NULL);
}