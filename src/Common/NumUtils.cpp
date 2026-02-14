
#include "NumUtils.hh"

namespace NumUtils {
    std::pair<bool, std::pair<double, double>> solve_quadratic(double a, double b, double c) {
        a *= 2;
        double D = b * b - 2 * a * c;
        if (D < 0) return {false, {0.0, 0.0}};
        double sqrtD = D < 1e-6 ? 0.0 : std::sqrt(D);
        return {true, {(-b - sqrtD) / a, (-b + sqrtD) / a}};
    }

    double urand(double lower, double upper) {
        std::uniform_real_distribution<> dis(lower, upper);
        return dis(gen);
    }
} // namespace NumUtils