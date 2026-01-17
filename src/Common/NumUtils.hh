#pragma once

#include <cmath>
#include <random>

#include "Constants.hh"

extern std::mt19937 gen; // defined properly in GTPEngine.cpp

namespace NumUtils {
	inline bool is_close(double a, double b, double tol = TOL)  {
		return std::abs(a - b) < tol;
	}
	inline bool is_close_2(double a, double b, double tol = TOL2)  {
		return std::abs(a - b) < tol;
	}
	std::pair<bool, std::pair<double, double>> solve_quadratic(double a, double b, double c);

	double urand(double lower, double upper);
} // namespace NumUtils