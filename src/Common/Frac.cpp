
#include <cmath>
#include <numeric>
#include <string>

#include "Frac.hh"
#include "Common/Exceptions.hh"

Frac::Frac(int num, int den) {
    if (den == 0) {
        throw NumericsError("Denominator cannot be zero");
    }
    int d = std::gcd(num, den);
    this->num = num / d;
    this->den = den / d;
}
Frac::Frac(double d) {
    double num_d = d;
    this->den = 1;
    while (std::abs(num_d - std::round(num_d)) > TOL) {
        num_d += d;
        this->den += 1;
    }
    if (d < 0) {
        this->num = static_cast<int>(std::round(num_d) - TOL);
    } else {
        this->num = static_cast<int>(std::round(num_d) + TOL);
    }
}

Frac Frac::operator+(const Frac &other) const {
    return Frac(this->num * other.den + other.num * this->den, this->den * other.den);
}
Frac Frac::operator-(const Frac &other) const {
    return Frac(this->num * other.den - other.num * this->den, this->den * other.den);
}
Frac Frac::operator*(const Frac &other) const {
    return Frac(this->num * other.num, this->den * other.den);
}
Frac Frac::operator/(const Frac &other) const {
    return Frac(this->num * other.den, this->den * other.num);
}
void Frac::operator=(const Frac &other) {
    this->num = other.num;
    this->den = other.den;
}
const bool Frac::operator==(const Frac &other) {
    return (this->num == other.num) && (this->den == other.den);
}
bool Frac::operator==(Frac &&other) {
    return (this->num == other.num) && (this->den == other.den);
}
const bool Frac::operator!=(const Frac &other) {
    return (this->num != other.num) || (this->den != other.den);
}
const bool Frac::operator<(const Frac &other) const {
    return (this->num * other.den) < (other.num * this->den);
}
auto Frac::operator<=>(const Frac &other) const {
    return (this->num * other.den) <=> (other.num * this->den);
}

double Frac::to_double() const {
    return static_cast<double>(this->num) / static_cast<double>(this->den);
}
std::pair<Frac, double> Frac::from_double(double d) {
    Frac f = Frac(d);
    return {f, f.to_double()};
}

std::string Frac::to_string() const {
    return std::to_string(this->num) + "/" + std::to_string(this->den);
}