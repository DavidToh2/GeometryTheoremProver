#pragma once

#include <string>

class Predicate;

class Node {

public:
    std::string name;

    Node* parent = nullptr;
    Node* root = nullptr;
    Predicate* parent_why = nullptr;

    Node(std::string name) : name(name) {}
};
