
#include <map>
#include <memory>
#include <iostream>
#include <ostream>
#include <string>

#include "DD/DDEngine.hh"
#include "Predicate.hh"
#include "Common/Generator.hh"
#include "Common/Constants.hh"
#include "Geometry/GeometricGraph.hh"

/* Initialisation and adding theorems from rules.txt */

DDEngine::DDEngine() {
    base_pred = std::make_unique<Predicate>();
    for (std::string pname : Constants::PREDICATE_NAMES) {
        predicates_by_type[pname] = std::vector<std::string>();
    }
}

void DDEngine::__add_theorem_template_from_text(const std::string s) { 

    std::unique_ptr<Theorem> _thr = std::make_unique<Theorem>(s);
    std::string name = _thr.get()->name;
    theorems.insert({name, std::move(_thr)});
}

void DDEngine::__add_construction_template_from_texts(const std::vector<std::string> v) { 

    std::unique_ptr<Construction> _c = std::make_unique<Construction>(v);
    std::string name = _c.get()->name;
    constructions.insert({name, std::move(_c)});
}

/* Insertion and manipulation of predicates in the proof state */

void DDEngine::insert_predicate(std::unique_ptr<Predicate> &&predicate) {
    std::string h_ = predicate.get()->hash;
    predicates.insert({h_, std::move(predicate)});
    recent_predicates.emplace_back(h_);
}

bool DDEngine::has_predicate_by_hash(const std::string hash) {
    return predicates.find(hash) != predicates.end();
}

Generator<Predicate*> DDEngine::get_recent_predicates() {
    for (std::string h : recent_predicates) {
        co_yield predicates.at(h).get();
    }
    recent_predicates.clear();
    co_return;
}

/* Predicate matching functions */

Generator<bool> DDEngine::match_coll(PredicateTemplate* pred_template, GeometricGraph &ggraph) {
    char a = pred_template->args[0]->filled();
    char b = pred_template->args[1]->filled();
    char c = pred_template->args[2]->filled();
    char k = (a << 2) | (b << 1) | c;
    Point* p1 = pred_template->args[0]->get_point();
    Point* p2 = pred_template->args[1]->get_point();
    Point* p3 = pred_template->args[2]->get_point();
    Line* l1;
    int unsets[2];

    // Rearrange the arguments based on which are set
    switch(k) {
        case 0b011: 
            std::swap(p1, p3);
            unsets[0] = 0;
            break;
        case 0b101: 
            std::swap(p2, p3);
            unsets[0] = 1;
            break;
        case 0b110:
            unsets[0] = 2;
            break;

        case 0b001: 
            unsets[0] = 0;
            unsets[1] = 1;
            std::swap(p1, p3);
            break;
        case 0b010: 
            unsets[0] = 0;
            unsets[1] = 2;
            std::swap(p1, p2);
            break;
        case 0b100: 
            unsets[0] = 1;
            unsets[1] = 2;
            break;
    }

    switch(k) {
        case 0b111: // All args are set
            if (ggraph.check_collinear(p1, p2, p3)) {
                co_yield true;
            }
            co_return;
        case 0b011:
        case 0b101:
        case 0b110: { // Two args are set
            l1 = ggraph.try_get_line(p1, p2);
            if (l1) {
                for (auto& [pt, _] : l1->points) {
                    if (pt != p1 && pt != p2) {
                        pred_template->set_arg(unsets[0], pt);
                        co_yield true;
                        pred_template->clear_arg(unsets[0]);
                    }
                }
            }
            co_return;
        }
        case 0b001:
        case 0b010:
        case 0b100: { // One arg is set
            auto gen_lines = p1->on_lines();
            while (gen_lines) {
                l1 = gen_lines();
                auto gen_point_pairs = NodeUtils::on_pairs<Point>(l1->points);
                while (gen_point_pairs) {
                    auto [pt1, pt2] = gen_point_pairs();
                    if (pt1 != p1 && pt2 != p1) {
                        pred_template->set_arg(unsets[0], pt1);
                        pred_template->set_arg(unsets[1], pt2);
                        co_yield true;
                        pred_template->clear_arg(unsets[0]);
                        pred_template->clear_arg(unsets[1]);
                    }
                }
            }
            co_return;
        }
        case 0b000: { // No args are set
            for (Line* l1 : ggraph.root_lines) {
                if (l1->points.size() < 3) {
                    continue;
                }
                auto gen_point_triples = NodeUtils::on_triples<Point>(l1->points);
                while (gen_point_triples) {
                    auto [pt1, pt2, pt3] = gen_point_triples();
                    pred_template->set_arg(0, pt1);
                    pred_template->set_arg(1, pt2);
                    pred_template->set_arg(2, pt3);
                    co_yield true;
                    pred_template->clear_arg(0);
                    pred_template->clear_arg(1);
                    pred_template->clear_arg(2);
                }
            }
            co_return;
        }
    }
}

Generator<bool> DDEngine::match_cyclic(PredicateTemplate* pred_template, GeometricGraph &ggraph) {
    // TBA
    co_return;
}

Generator<bool> DDEngine::match_para(PredicateTemplate* pred_template, GeometricGraph &ggraph) {
    char a = pred_template->args[0]->filled();
    char b = pred_template->args[1]->filled();
    char c = pred_template->args[2]->filled();
    char d = pred_template->args[3]->filled();
    char k = (a << 3) | (b << 2) | (c << 1) | d;
    Point* p1 = pred_template->args[0]->get_point();
    Point* p2 = pred_template->args[1]->get_point();
    Point* p3 = pred_template->args[2]->get_point();
    Point* p4 = pred_template->args[3]->get_point();
    Line* l1, *l2;
    int unsets[3];
    // TBA
    // Note: We only consider matches where l1 and l2 are known to be distinct (they may however have a point in common).
    co_return;
}

Generator<bool> DDEngine::match_perp(PredicateTemplate* pred_template, GeometricGraph &ggraph) {
    // TBA
    co_return;
}

/* Search functions */

void DDEngine::search(GeometricGraph &ggraph) {

    for (auto& thr : theorems) {
        Theorem* theorem = thr.second.get();
        
    }

}






void DDEngine::__print_theorems(std::ostream& os) {
    for (auto& thr : theorems) {
        Theorem* theorem = thr.second.get();
        os << theorem->to_string() << std::endl;
    }
}

void DDEngine::__print_constructions(std::ostream& os) {
    for (auto& c : constructions) {
        Construction* construction = c.second.get();
        os << construction->to_string() << std::endl;
    }
}

void DDEngine::__print_predicates(std::ostream& os) {
    for (auto& p : predicates) {
        Predicate* predicate = p.second.get();
        os << predicate->to_string() << std::endl;
    }
}