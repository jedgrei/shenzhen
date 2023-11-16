#include "board.h"
#include "colors.h"
#include "locations.h"
#include <vector>
#include <iostream>
using namespace std;

// cell that can hold a card
// can be covered by a set of 4 dragons
struct Cell {
	bool covered = false;
	bool occupied = false;
	Card card;

	Cell() {
		card = Card();
	}
};

enum BoardLocation {
	PileLoc,
	CellLoc,
	FlowerCellLoc,
	FoundationLoc
};

struct MovingCard {
	int origX, origY;
	float t;
	int x, y;
	int targetX, targetY;
	BoardLocation dest;
	int destIndex;
	float velX, velY;
	Card c;
};

vector<Card> piles[8];
Cell cells[3];
Cell flowerCell;
Cell foundations[3];
bool buttonsActive[3];

bool mouseDown = false;
vector<Card> heldCard;
int mouseXOffset = 0;
int mouseYOffset = 0;

bool heldFromPile;
int heldOriginIndex;

vector<MovingCard> movingCards;

SDL_Texture* cardTexture;

void initBoard() {
	// set up tableu
	vector<Card> deck = createDeck();
	for (int i = 0; i < 8; ++i) {
		piles[i] = vector<Card>();
		for (int c = 0; c < 5; ++c) {
			piles[i].push_back(deck.front());
			deck.erase(deck.begin());
		}
	}

	// set up cells
	for (int n = 0; n < 3; ++n) {
		cells[n] = Cell();
	}
	flowerCell = Cell();
	for (int n = 0; n < 3; ++n) {
		foundations[n] = Cell();
	}

	// set buttons
	for (int n = 0; n < 3; ++n) {
		buttonsActive[n] = true;
	}

	// load images
	SDL_Surface* image;
	image = IMG_Load("imgs/cards.png");
	cardTexture = SDL_CreateTextureFromSurface(renderer, image);
}

std::vector<Card> createDeck() {
	std::vector<Card> u;
	for (Suit s : {red, green, black}) {
		for (int n = 1; n <= 9; ++n) {
			u.push_back(Card(n, s));
		}
		for (int i = 0; i < 4; ++i) {
			u.push_back(Card(0, s, dragon));
		}
	}
	u.push_back(Card(0, none, flower));

	std::vector<Card> d;
	while (u.size() > 0) {
		int n = rand() % u.size();
		d.push_back(u[n]);
		u.erase(u.begin() + n);
	}

	return d;
}

const float RESOLUTION = 2.0833;

void drawCard(Card c, SDL_Rect r) {
	if (c.faceup) {
		SDL_Rect srcRect = SDL_Rect();
		srcRect.w = 500 * RESOLUTION;
		srcRect.h = 1000 * RESOLUTION;
		switch (c.suit) {
		case red:
			srcRect.y = 0;
			break;
		case green:
			srcRect.y = 1000;
			break;
		case black:
			srcRect.y = 2000;
			break;
		}
		switch (c.type) {
		case number:
			srcRect.x = 500 * (c.num - 1);
			break;
		case dragon:
			srcRect.x = 4500;
			break;
		case flower:
			srcRect.x = 5000;
			srcRect.y = 0;
			break;
		}

		srcRect.x *= RESOLUTION;
		srcRect.y *= RESOLUTION;

		SDL_RenderCopy(renderer, cardTexture, &srcRect, &r);
	}
	else {
		SDL_Rect srcRect = SDL_Rect();
		srcRect.w = 500 * RESOLUTION;
		srcRect.h = 1000 * RESOLUTION;

		srcRect.x = 5000;
		srcRect.y = 1000;

		srcRect.x *= RESOLUTION;
		srcRect.y *= RESOLUTION;

		SDL_RenderCopy(renderer, cardTexture, &srcRect, &r);
	}
}

// type: 0=cell, 1=flower
void drawCell(Cell c, SDL_Rect r, bool flower = false) {
	if (c.covered && c.occupied) {
		SDL_Rect srcRect = SDL_Rect();
		srcRect.w = 500 * RESOLUTION;
		srcRect.h = 1000 * RESOLUTION;

		srcRect.x = 5000;
		srcRect.y = 1000;

		srcRect.x *= RESOLUTION;
		srcRect.y *= RESOLUTION;

		SDL_RenderCopy(renderer, cardTexture, &srcRect, &r);
	}
	else if (c.occupied && !c.covered) {
		drawCard(c.card, r);
	}
	else {
		SDL_Rect srcRect = SDL_Rect();
		srcRect.w = 500 * RESOLUTION;
		srcRect.h = 1000 * RESOLUTION;

		srcRect.x = 5500;
		srcRect.y = 1000 * flower;

		srcRect.x *= RESOLUTION;
		srcRect.y *= RESOLUTION;

		SDL_RenderCopy(renderer, cardTexture, &srcRect, &r);
	}
}

void drawButton(int n, bool active, SDL_Rect r) {
	SDL_Rect srcRect = SDL_Rect();
	srcRect.w = 250 * RESOLUTION;
	srcRect.h = 270 * RESOLUTION;

	srcRect.x = 5000 + 250 * n;
	srcRect.y = (active ? 2000 : 2270);

	srcRect.x *= RESOLUTION;
	srcRect.y *= RESOLUTION;

	SDL_RenderCopy(renderer, cardTexture, &srcRect, &r);
}

SDL_Rect getRect(BoardLocation loc, int index) {
	switch (loc) {
	case PileLoc:
		return PileRect(index, piles[index].size() - 1);
	case CellLoc:
		return CellRect(index);
	case FlowerCellLoc:
		return FlowerCellRect();
	case FoundationLoc:
		return FoundationRect(index);
	}
}

void moveCard(Card c, int x, int y, BoardLocation dest, int destIndex) {
	MovingCard m;
	m.origX = x;
	m.origY = y;
	m.t = 0;
	m.x = x;
	m.y = y;
	m.c = c;
	SDL_Rect destRect = getRect(dest, destIndex);
	m.targetX = destRect.x;
	m.targetY = destRect.y;
	m.dest = dest;
	m.destIndex = destIndex;
	movingCards.push_back(m);
}

void renderBoard() {
	SDL_SetRenderDrawColor(renderer, BG_GREEN.r, BG_GREEN.g, BG_GREEN.b, BG_GREEN.a);
	SDL_RenderClear(renderer);

	// render tableu
	for (int n = 0; n < 8; ++n) {
		for (int c = 0; c < piles[n].size(); ++c) {
			drawCard(piles[n][c], PileRect(n, c));
			//cout << "b";
		}
	}
	// cells
	for (int n = 0; n < 3; ++n) {
		drawCell(cells[n], CellRect(n));
	}
	// buttons
	for (int n = 0; n < 3; ++n) {
		drawButton(n, buttonsActive[n], ButtonRect(n));
	}
	// flower
	drawCell(flowerCell, FlowerCellRect(), true);
	// foundations
	for (int n = 0; n < 3; ++n) {
		drawCell(foundations[n], FoundationRect(n));
	}
	// held cards
	for (int c = heldCard.size() - 1; c >= 0; --c) {
		int x, y;
		SDL_GetMouseState(&x, &y);
		drawCard(heldCard[c], HeldRect(heldCard.size() - c, x, y, mouseXOffset, mouseYOffset));
	}
	// moving cards
	for (int i = 0; i < movingCards.size(); ++i) {
		drawCard(movingCards[i].c, MovingRect(movingCards[i].x, movingCards[i].y));
	}
}


void handleMouseDownBoard(SDL_MouseButtonEvent button) {
	// M1/LMB
	if (button.button == 1 && heldCard.size() == 0) {
		// check in tableu
		for (int n = 0; n < 8; ++n) {
			for (int c = piles[n].size() - 1; c >= 0; c--) {
				if (inRect(button.x, button.y, PileRect(n, c))) {
					bool valid = true;
					if (piles[n][c].type == number) {
						for (int d = piles[n].size() - 1; d > c; d--) {
							if (
								piles[n][d].type != number ||
								piles[n][d].suit == piles[n][d - 1].suit ||
								piles[n][d].num != piles[n][d - 1].num - 1
							) {
								valid = false;
							}
						}
					}
					else {
						// non-number cards are only valid if alone
						valid = c == piles[n].size() - 1;
					}
					if (valid) {
						mouseXOffset = button.x - PileRect(n, c).x;
						mouseYOffset = button.y - PileRect(n, c).y;
						heldFromPile = true;
						heldOriginIndex = n;
						// pick up card
						for (int d = piles[n].size() - 1; d >= c; d--) {
							heldCard.push_back(piles[n][d]);
							piles[n].erase(piles[n].end() - 1);
						}
						return;
					}
				}
			}
		}
		// check in cells
		for (int n = 0; n < 3; ++n) {
			if (inRect(button.x, button.y, CellRect(n))) {
				if (cells[n].occupied && !cells[n].covered) {
					mouseXOffset = button.x - CellRect(n).x;
					mouseYOffset = button.y - CellRect(n).y;
					heldFromPile = false;
					heldOriginIndex = n;
					heldCard.push_back(cells[n].card);
					cells[n].card = Card();
					cells[n].occupied = false;
					return;
				}
			}
		}
		// check on buttons
		for (int n = 0; n < 3; ++n) {
			if (buttonsActive[n]) {
				if (inRect(button.x, button.y, ButtonRect(n))) {
					Suit s = (Suit)(n + 1);
					int toCell = -1;

					// check for an empty cell
					bool empty = false;
					for (int c = 0; c < 3; ++c) {
						if (!cells[c].occupied || (cells[c].card.type == dragon && cells[c].card.suit == s)) {
							empty = true;
							toCell = c;
							break;
						}
					}
					if (!empty) return;

					// go through piles for dragons not on top
					for (int p = 0; p < 8; p++) {
						if (piles[p].size() > 1) {
							for (int c = 0; c < piles[p].size() - 1; c++) {
								if (piles[p][c].type == dragon && piles[p][c].suit == s) {
									return;
								}
							}
						}
					}

					// pull dragons from cells
					for (int c = 0; c < 3; ++c) {
						if (cells[c].occupied && !cells[c].covered) {
							if (cells[c].card.type == dragon && cells[c].card.suit == s) {
								SDL_Rect r = CellRect(c);
								cells[c].card.faceup = false;
								moveCard(cells[c].card, r.x, r.y, CellLoc, toCell);
								cells[c].occupied = false;
								cells[c].card = Card();
							}
						}
					}

					// pull dragons from piles
					for (int p = 0; p < 8; ++p) {
						if (piles[p].size() > 0 && piles[p].back().type == dragon && piles[p].back().suit == s) {
							SDL_Rect r = PileRect(p, piles[p].size());
							piles[p].back().faceup = false;
							moveCard(piles[p].back(), r.x, r.y, CellLoc, toCell);
							piles[p].erase(piles[p].end() - 1);
						}
					}

					// cover chosen cell
					//cells[toCell].card = Card(0, s, dragon);
					//cells[toCell].occupied = true;
					cells[toCell].covered = true;
					buttonsActive[n] = false;
					return;
				}
			}
		}
	}
}

void handleMouseUpBoard(SDL_MouseButtonEvent button) {
	if (button.button == 1 && heldCard.size() > 0) {
		// check piles
		for (int n = 0; n < 8; ++n) {
			// nonempty piles
			if (piles[n].size() > 0) {
				if (inRect(button.x, button.y, PileRect(n, piles[n].size() - 1))) {
					// if valid drop
					if (
						heldCard.back().type == number &&
						piles[n].back().type == number &&
						heldCard.back().suit != piles[n].back().suit &&
						heldCard.back().num == piles[n].back().num - 1
						) {
						for (int c = heldCard.size() - 1; c >= 0; c--) {
							piles[n].push_back(heldCard[c]);
							heldCard.erase(heldCard.end() - 1);
						}
						return;
					}
				}
			}
			// empty piles
			else {
				if (inRect(button.x, button.y, PileRect(n, 0))) {
					for (int c = heldCard.size() - 1; c >= 0; c--) {
						piles[n].push_back(heldCard[c]);
						heldCard.erase(heldCard.end() - 1);
					}
					return;
				}
			}
		}
		// check cells
		for (int n = 0; n < 3; ++n) {
			if (inRect(button.x, button.y, CellRect(n))) {
				if (heldCard.size() == 1 && !cells[n].occupied) {
					cells[n].card = heldCard[0];
					cells[n].occupied = true;
					heldCard.erase(heldCard.begin());
					return;
				}
			}
		}
		// check flower cell
		if (inRect(button.x, button.y, FlowerCellRect())) {
			if (heldCard.size() == 1 && heldCard[0].type == flower) {
				flowerCell.card = heldCard[0];
				flowerCell.occupied = true;
				heldCard.erase(heldCard.begin());
				return;
			}
		}
		// check foundation
		for (int n = 0; n < 3; ++n) {
			if (inRect(button.x, button.y, FoundationRect(n))) {
				if (heldCard.size() == 1) {
					// if valid foundation placement
					if (foundations[n].occupied) {
						if (
							heldCard[0].type == number &&
							heldCard[0].suit == foundations[n].card.suit &&
							heldCard[0].num == foundations[n].card.num + 1
						) {
							foundations[n].card = heldCard[0];
							foundations[n].occupied = true;
							heldCard.erase(heldCard.begin());
							return;
						}
					}
					else {
						if (heldCard[0].type == number && heldCard[0].num == 1) {
							foundations[n].card = heldCard[0];
							foundations[n].occupied = true;
							heldCard.erase(heldCard.begin());
							return;
						}
					}
				}
			}
		}
		// return to origin
		if (heldFromPile) {
			for (int c = heldCard.size() - 1; c >= 0; c--) {
				piles[heldOriginIndex].push_back(heldCard[c]);
				heldCard.erase(heldCard.end() - 1);
			}
			return;	
		}
		else {
			cells[heldOriginIndex].card = heldCard[0];
			cells[heldOriginIndex].occupied = true;
			heldCard.erase(heldCard.begin());
		}
	}
}

const float VEL_MULTIPLIER = 0.1;

const int MOVING_THRESHOLD = 5;

void updateBoard() {
	// update moving cards
	for (int i = 0; i < movingCards.size(); ++i) {
		movingCards[i].t += VEL_MULTIPLIER;
		float t = 0.5 * (-cos(movingCards[i].t * M_PI) + 1);
		//float t = pow(movingCards[i].t, 2);
		//float t = movingCards[i].t;
		//float t = sqrt(movingCards[i].t);
		MovingCard m = movingCards[i];

		movingCards[i].x = m.origX + (m.targetX - m.origX) * t;
		movingCards[i].y = m.origY + (m.targetY - m.origY) * t;

		if (m.t >= 1) {
			switch (m.dest) {
			case PileLoc:
				piles[m.destIndex].push_back(m.c);
				break;
			case CellLoc:
				cells[m.destIndex].card = m.c;
				cells[m.destIndex].occupied = true;
				break;
			case FlowerCellLoc:
				flowerCell.card = m.c;
				flowerCell.occupied = true;
				break;
			case FoundationLoc:
				foundations[m.destIndex].card = m.c;
				foundations[m.destIndex].occupied = true;
				break;
			}

			movingCards.erase(movingCards.begin() + i);
			--i;
		}
	}

	if (heldCard.size() == 0 && movingCards.size() == 0) {
		bool moved = false;
		// check for auto-moves
		for (int i = 0; i < 8; i++) {
			if (piles[i].size() > 0) {
				// auto-move to foundations
				int minFoundationVal = 10;
				for (int n = 0; n < 3; ++n) {
					if (!foundations[n].occupied) {
						minFoundationVal = 0;
						break;
					}
					if (foundations[n].card.num < minFoundationVal) {
						minFoundationVal = foundations[n].card.num;
					}
				}
				if (piles[i].back().num == minFoundationVal + 1) {
					for (int n = 0; n < 3; ++n) {
						if (!foundations[n].occupied || piles[i].back().suit == foundations[n].card.suit) {
							SDL_Rect r = PileRect(i, piles[i].size() - 1);
							moveCard(piles[i].back(), r.x, r.y, FoundationLoc, n);
							piles[i].erase(piles[i].end() - 1);
							moved = true;
							break;
						}
					}
				}
				else if (piles[i].back().type == flower) {
					SDL_Rect r = PileRect(i, piles[i].size() - 1);
					moveCard(piles[i].back(), r.x, r.y, FlowerCellLoc, 0);
					piles[i].erase(piles[i].end() - 1);
					moved = true;
				}
			}
			if (moved) break;
		}
	}


	// check for win
	bool win = true;
	for (int p = 0; p < 8; ++p) {
		if (piles[p].size() > 0) {
			win = false;
			break;
		}
	}
}