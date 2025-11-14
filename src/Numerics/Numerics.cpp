
#include <string>

#include "Numerics.hh"

std::string Frac::to_string() {
    return std::to_string(num) + "/" + std::to_string(den);
}