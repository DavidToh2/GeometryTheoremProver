
#pragma once

#include <stdexcept>
#include <string>

class Contradiction : public std::logic_error {
public:
    explicit Contradiction(const std::string& message)
        : std::logic_error(message) {}
};