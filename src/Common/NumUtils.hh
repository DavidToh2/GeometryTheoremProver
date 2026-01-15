#pragma once

#include <cmath>
#include "Constants.hh"

namespace NumUtils {
	inline bool is_close(double a, double b, double tol = TOL)  {
		return std::abs(a - b) < tol;
	}
	std::pair<bool, std::pair<double, double>> solve_quadratic(double a, double b, double c);
} // namespace NumUtils