
#include <map>
#include <memory>
#include <iostream>

#include "DDEngine.hh"
#include "Predicate.hh"
#include "Predicate2.hh"

void DDEngine::__add_theorem_from_text(std::string s) { 

    std::unique_ptr<Theorem> _thr = std::make_unique<Theorem>(s);
    std::string name = _thr.get()->name;
    theorems.insert({name, std::move(_thr)});
    
}

void DDEngine::__print_theorems() {
    for (auto& thr : theorems) {
        Theorem* theorem = thr.second.get();
        std::cout << theorem->to_string() << std::endl;
    }
}