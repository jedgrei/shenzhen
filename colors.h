#pragma once
#include <string>
using namespace std;


struct Color {
	int r, g, b, a;
	Color(int r, int g, int b, int a = 255) : r(r), g(g), b(b), a(a) {}
	Color(string s) {
		// expects RRGGBB(AA), using hex digits
		r = stoi(s.substr(0, 2), NULL, 16);
		g = stoi(s.substr(2, 2), NULL, 16);
		b = stoi(s.substr(4, 2), NULL, 16);
		if (s.size() == 6) {
			a = 255;
		}
		else {
			a = stoi(s.substr(6, 2), NULL, 16);
		}
	}
};

const Color BLACK = Color("000000");
const Color WHITE = Color("FFFFFF");
const Color BG_GREEN = Color("216433");