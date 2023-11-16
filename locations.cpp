#include "game.h"
#include "card.h"
#include "locations.h"

int horizontalSpace(int w) {
	return w / 33;
}

int verticalSpace(int w) {
	return w / 33;
}

int verticalPileSpace(int w) {
	return w / 45;
}

int cardWidth(int w) {
	return w / 11;
}

int cardHeight(int w) {
	return 2 * w / 11;
}

SDL_Rect PileRect(int n, int c) {
	int w;
	SDL_GetWindowSize(window, &w, NULL);
	SDL_Rect r = SDL_Rect();
	r.x = (1 + n) * horizontalSpace(w) + n * cardWidth(w);
	r.y = 2 * verticalSpace(w) + cardHeight(w) + c * verticalPileSpace(w);
	r.w = cardWidth(w);
	r.h = cardHeight(w);
	return r;
}

SDL_Rect CellRect(int n) {
	int w;
	SDL_GetWindowSize(window, &w, NULL);
	SDL_Rect r = SDL_Rect();
	r.x = (1 + n)* horizontalSpace(w) + n * cardWidth(w);
	r.y = verticalSpace(w);
	r.w = cardWidth(w);
	r.h = cardHeight(w);
	return r;
}

SDL_Rect FlowerCellRect() {
	int w;
	SDL_GetWindowSize(window, &w, NULL);
	SDL_Rect r = SDL_Rect();
	r.x = 4.67 * horizontalSpace(w) + 3.67 * cardWidth(w);
	r.y = verticalSpace(w);
	r.w = cardWidth(w);
	r.h = cardHeight(w);
	return r;
}

SDL_Rect FoundationRect(int n) {
	int w;
	SDL_GetWindowSize(window, &w, NULL);
	SDL_Rect r = SDL_Rect();
	r.x = (6 + n) * horizontalSpace(w) + (5 + n) * cardWidth(w);
	r.y = verticalSpace(w);
	r.w = cardWidth(w);
	r.h = cardHeight(w);
	return r;
}

SDL_Rect HeldRect(int c, int x, int y, int xoff, int yoff) {
	int w;
	SDL_GetWindowSize(window, &w, NULL);
	SDL_Rect r = SDL_Rect();
	r.x = x - xoff;
	r.y = y - yoff + (c-1) * verticalPileSpace(w);
	r.w = cardWidth(w);
	r.h = cardHeight(w);
	return r;
}

SDL_Rect ButtonRect(int n) {
	int w;
	SDL_GetWindowSize(window, &w, NULL);
	SDL_Rect r = SDL_Rect();
	r.x = 3.7 * horizontalSpace(w) + 3 * cardWidth(w);
	r.y = verticalSpace(w) + cardHeight(w) * n * 0.365;
	// w & h from dimensions of buttons in illustrator
	r.w = cardWidth(w) * 0.5;
	r.h = cardHeight(w) * 0.27;
	return r;
}

SDL_Rect MovingRect(int x, int y) {
	int w;
	SDL_GetWindowSize(window, &w, NULL);
	SDL_Rect r = SDL_Rect();
	r.x = x;
	r.y = y;
	r.w = cardWidth(w);
	r.h = cardHeight(w);
	return r;
}


bool inRect(int px, int py, SDL_Rect r) {
	return
		(px >= r.x) &&
		(py >= r.y) &&
		(px <= r.x + r.w) &&
		(py <= r.y + r.h);
}