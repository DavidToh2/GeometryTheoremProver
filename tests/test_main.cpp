#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <random>

std::random_device rd = std::random_device();
std::mt19937 gen(rd());