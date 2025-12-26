#pragma once

#include <Geometry/Object.hh>
#include <Geometry/Object2.hh>
#include <Geometry/Node.hh>
#include <stdexcept>

class InvalidTextualInputError : public std::runtime_error {
public:
    explicit InvalidTextualInputError(const std::string& message)
        : std::runtime_error(message) {}
};

class GGraphInternalError : public std::runtime_error {
public:
    explicit GGraphInternalError(const std::string& message)
        : std::runtime_error(message) {}
};

class DDInternalError : public std::runtime_error {
public:
    explicit DDInternalError(const std::string& message)
        : std::runtime_error(message) {}
};

class ARInternalError : public std::runtime_error {
public:
    explicit ARInternalError(const std::string& message)
        : std::runtime_error(message) {}
};

class NumericsError : public std::runtime_error {
public:
    explicit NumericsError(const std::string& message)
        : std::runtime_error(message) {}
};