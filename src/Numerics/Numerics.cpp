
#include <numeric>
#include <string>

#include "Numerics.hh"
#include "Common/Exceptions.hh"

Frac::Frac(int num, int den) {
    if (den == 0) {
        throw NumericsError("Denominator cannot be zero");
    }
    int d = std::gcd(num, den);
    this->num = num / d;
    this->den = den / d;
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
bool Frac::operator==(const Frac &other) {
    return (this->num == other.num) && (this->den == other.den);
}
bool Frac::operator==(Frac &&other) {
    return (this->num == other.num) && (this->den == other.den);
}


std::string Frac::to_string(Frac &f) {
    return std::to_string(f.num) + "/" + std::to_string(f.den);
}