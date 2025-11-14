#pragma once

#include <string>

class Predicate;

class Node {

public:
    std::string id;

    Node* parent = nullptr;
    Node* root = nullptr;
    Predicate* parent_why = nullptr;
};
