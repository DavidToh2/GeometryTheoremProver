#pragma once

#include <string>

/* Fraction class.

The `Frac` constructor applies `std::gcd` by default, so that fractions
are always in their lowest forms. */
typedef struct Frac {
	int num;
	int den;
	static constexpr double TOL = 1e-6;

	Frac() : num(0), den(1) {}
	Frac(int n): num(n), den(1) {};
	Frac(int num, int den);
	Frac(double d);

	Frac operator+(const Frac &other) const;
	Frac operator-(const Frac &other) const;
	Frac operator*(const Frac &other) const;
	Frac operator/(const Frac &other) const;
	void operator=(const Frac &other);
	bool operator==(const Frac &other);
	bool operator==(Frac &&other);
	bool operator<(const Frac &other) const;
	auto operator<=>(const Frac &other) const;

	double to_double() const;
	static std::pair<Frac, double> from_double(double d);

	std::string to_string() const;
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