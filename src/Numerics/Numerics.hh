#pragma once

#include <string>

typedef struct Frac {
	int num;
	int den;

	std::string to_string();
} Frac;

typedef struct Coords {
	Frac x;
	Frac y;
} Coords;

typedef struct EqLine {
	Frac m;
	Frac c;
} EqLine;

typedef struct EqCircle {
	Coords c;
	Frac r;
} EqCircle;