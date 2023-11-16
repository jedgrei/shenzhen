#pragma once
#include <vector>

enum Suit {
	none,
	red,
	green,
	black
};

enum Type {
	number,
	dragon,
	flower
};

struct Card {
	int num;
	Suit suit;
	Type type;
	bool faceup;
	
	Card(int num = 0, Suit suit = none, Type type = number, bool faceup = true) {
		this->num = num;
		this->suit = suit;
		this->type = type;
		this->faceup = faceup;
	}
};
