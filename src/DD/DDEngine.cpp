
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
    int i = 0;
    pred_t pt;
    while (pt != pred_t::LAST) {
        pt = static_cast<pred_t>(i);
        predicates_by_type.insert({pt, std::set<Predicate*>()});
        i++;
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

void DDEngine::set_conclusion(std::unique_ptr<Predicate> predicate) {
    conclusion_args.clear();
    for (Node* node : predicate->args) {
        conclusion_args.emplace_back(std::make_unique<Arg>(node));
    }
    conclusion = std::make_unique<PredicateTemplate>(predicate.get(), conclusion_args);
}





void DDEngine::insert_predicate(std::unique_ptr<Predicate> &&predicate) {
    Predicate* p = predicate.get();
    predicates.insert({p->hash, std::move(predicate)});
    recent_predicates.emplace_back(p);
}

bool DDEngine::has_predicate_by_hash(const std::string hash) {
    return predicates.find(hash) != predicates.end();
}

Generator<Predicate*> DDEngine::get_recent_predicates() {
    for (Predicate* p : recent_predicates) {
        co_yield p;
    }
    recent_predicates.clear();
    co_return;
}





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

    // Split cases based on how many args are set
    switch(k) {
        case 0b111: { // All args are set
            if (ggraph.check_coll(p1, p2, p3)) {
                co_yield true;
            }
            co_return;
        } break;
        case 0b011:
        case 0b101:
        case 0b110: { // Two args are set
            l1 = ggraph.try_get_line(p1, p2);
            if (l1) {
                auto gen_point_l1 = l1->all_points();
                while (gen_point_l1) {
                    Point* pt = gen_point_l1();
                    if (pt != p1 && pt != p2) {
                        pred_template->set_arg(unsets[0], pt);
                        co_yield true;
                        pred_template->clear_arg(unsets[0]);
                    }
                }
            }
            co_return;
        } break;
        case 0b001:
        case 0b010:
        case 0b100: { // One arg is set
            auto gen_lines = p1->on_lines();
            while (gen_lines) {
                l1 = gen_lines();
                auto gen_point_pairs = l1->all_point_pairs();
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
        } break;
        case 0b000: { // No args are set
            for (Line* l1 : ggraph.root_lines) {
                if (l1->points.size() < 3) {
                    continue;
                }
                auto gen_point_triples = l1->all_point_triples();
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
        } break;
    }
    co_return;
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
    char k1 = (a << 1) | b, k2 = (c << 1) | d;
    Point* p1 = pred_template->args[0]->get_point();
    Point* p2 = pred_template->args[1]->get_point();
    Point* p3 = pred_template->args[2]->get_point();
    Point* p4 = pred_template->args[3]->get_point();
    Line* l1, *l2;
    int unsets[4];
    
    // Rearrange the arguments based on which are set
    switch(k1) {
        case 0b00:
            unsets[0] = 0;
            unsets[1] = 1;
            break;
        case 0b01:
            std::swap(p1, p2);
            unsets[0] = 0;
            break;
        case 0b10:
            unsets[0] = 1;
            break;
    }
    switch(k2) {
        case 0b00:
            unsets[2] = 2;
            unsets[3] = 3;
            break;
        case 0b01:
            std::swap(p3, p4);
            unsets[2] = 2;
            break;
        case 0b10:
            unsets[2] = 3;
            break;
    }

    // Split cases based on how many args are set
    // Note: We only consider matches where l1 and l2 are known to be distinct (they may however have a point in common).

    switch(k1) {
        case 0b00: {
            switch(k2) {
                case 0b00: {
                    for (Direction* dir : ggraph.root_directions) {
                        auto gen_lines = dir->all_para_pairs();
                        while (gen_lines) {
                            auto [l1, l2] = gen_lines();
                            auto gen_point_pairs1 = l1->all_point_pairs();
                            while (gen_point_pairs1) {
                                auto [pt1, pt2] = gen_point_pairs1();
                                pred_template->set_arg(0, pt1);
                                pred_template->set_arg(1, pt2);
                                auto gen_point_pairs2 = l2->all_point_pairs();
                                while (gen_point_pairs2) {
                                    auto [pt3, pt4] = gen_point_pairs2();
                                    pred_template->set_arg(2, pt3);
                                    pred_template->set_arg(3, pt4);
                                    co_yield true;
                                    pred_template->clear_arg(2);
                                    pred_template->clear_arg(3);
                                }
                                pred_template->clear_arg(0);
                                pred_template->clear_arg(1);
                            }
                        }
                    }
                    co_return;
                } break;
                case 0b01:
                case 0b10: {
                    auto gen_l2 = p3->on_lines();
                    while (gen_l2) {
                        l2 = gen_l2();
                        Direction* d2 = l2->direction;
                        for (Line* l1 : d2->root_objs) {
                            if (l1 == l2) continue;
                            auto gen_point_pairs1 = l1->all_point_pairs();
                            while (gen_point_pairs1) {
                                auto [pt1, pt2] = gen_point_pairs1();
                                pred_template->set_arg(0, pt1);
                                pred_template->set_arg(1, pt2);
                                auto gen_point_2 = l2->all_points();
                                while (gen_point_2) {
                                    auto pt3 = gen_point_2();
                                    if (pt3 != p3) {
                                        pred_template->set_arg(unsets[2], pt3);
                                        co_yield true;
                                        pred_template->clear_arg(unsets[2]);
                                    }
                                }
                                pred_template->clear_arg(0);
                                pred_template->clear_arg(1);
                            }
                        }
                    }
                    co_return;
                } break;
                case 0b11: {
                    l2 = ggraph.try_get_line(p3, p4);
                    if (l2) {
                        Direction* d2 = l2->direction;
                        for (Line* l1 : d2->root_objs) {
                            if (l1 == l2) continue;
                            auto gen_point_pairs1 = l1->all_point_pairs();
                            while (gen_point_pairs1) {
                                auto [pt1, pt2] = gen_point_pairs1();
                                pred_template->set_arg(0, pt1);
                                pred_template->set_arg(1, pt2);
                                co_yield true;
                                pred_template->clear_arg(0);
                                pred_template->clear_arg(1);
                            }
                        }
                    }
                    co_return;
                } break;
            }
        } break;
        case 0b01:
        case 0b10: {
            auto gen_l1 = p1->on_lines();
            while (gen_l1) {
                l1 = gen_l1();
                Direction* d1 = l1->direction;
                auto gen_point_1 = l1->all_points();
                while (gen_point_1) {
                    auto pt1 = gen_point_1();
                    if (pt1 == p1) continue;
                    pred_template->set_arg(unsets[0], pt1);
                    
                    switch(k2) {
                        case 0b00: {
                            for (Line* l2 : d1->root_objs) {
                                if (l2 == l1) continue;
                                auto gen_point_pairs2 = l2->all_point_pairs();
                                while (gen_point_pairs2) {
                                    auto [pt3, pt4] = gen_point_pairs2();
                                    pred_template->set_arg(2, pt3);
                                    pred_template->set_arg(3, pt4);
                                    co_yield true;
                                    pred_template->clear_arg(2);
                                    pred_template->clear_arg(3);
                                }
                            }
                        } break;
                        case 0b01:
                        case 0b10: {
                            auto gen_l2 = p3->on_lines();
                            while (gen_l2) {
                                l2 = gen_l2();
                                if (Line::is_para(l1, l2) && l1 != l2) {
                                    auto gen_point_2 = l2->all_points();
                                    while (gen_point_2) {
                                        auto pt3 = gen_point_2();
                                        if (pt3 != p3) {
                                            pred_template->set_arg(unsets[2], pt3);
                                            co_yield true;
                                            pred_template->clear_arg(unsets[2]);
                                        }
                                    }
                                }
                            }
                        } break;
                        case 0b11: {
                            l2 = ggraph.try_get_line(p3, p4);
                            if (l2) {
                                if (Line::is_para(l1, l2) && l1 != l2) {
                                    co_yield true;
                                }
                            }
                        } break;
                    
                    }
                    pred_template->clear_arg(unsets[0]);
                }
                
            }
            co_return;
        } break;
        case 0b11: {
            l1 = ggraph.try_get_line(p1, p2);
            if (l1) {
                switch(k2) {
                    case 0b00: {
                        Direction* d1 = l1->direction;
                        for (Line* l2 : d1->root_objs) {
                            if (l2 == l1) continue;
                            auto gen_point_pairs2 = l2->all_point_pairs();
                            while (gen_point_pairs2) {
                                auto [pt1, pt2] = gen_point_pairs2();
                                pred_template->set_arg(0, pt1);
                                pred_template->set_arg(1, pt2);
                                co_yield true;
                                pred_template->clear_arg(0);
                                pred_template->clear_arg(1);
                            }
                        }
                    } break;
                    case 0b01:
                    case 0b10: {
                        auto gen_l2 = p3->on_lines();
                        while (gen_l2) {
                            l2 = gen_l2();
                            if (Line::is_para(l1, l2) && l1 != l2) {
                                auto gen_point_2 = l2->all_points();
                                while (gen_point_2) {
                                    auto pt3 = gen_point_2();
                                    if (pt3 != p3) {
                                        pred_template->set_arg(unsets[2], pt3);
                                        co_yield true;
                                        pred_template->clear_arg(unsets[2]);
                                    }
                                }
                            }
                        }
                    } break;
                    case 0b11: {
                        l2 = ggraph.try_get_line(p3, p4);
                        if (l2) {
                            if (Line::is_para(l1, l2) && l1 != l2) {
                                co_yield true;
                            }
                        }
                    } break;
                }
            }
            co_return;
        } break;
    }
    co_return;
}

Generator<bool> DDEngine::match_perp(PredicateTemplate* pred_template, GeometricGraph &ggraph) {
    char a = pred_template->args[0]->filled();
    char b = pred_template->args[1]->filled();
    char c = pred_template->args[2]->filled();
    char d = pred_template->args[3]->filled();
    char k1 = (a << 1) | b, k2 = (c << 1) | d;
    Point* p1 = pred_template->args[0]->get_point();
    Point* p2 = pred_template->args[1]->get_point();
    Point* p3 = pred_template->args[2]->get_point();
    Point* p4 = pred_template->args[3]->get_point();
    Line* l1, *l2;
    int unsets[4];
    
    // Rearrange the arguments based on which are set
    switch(k1) {
        case 0b00:
            unsets[0] = 0;
            unsets[1] = 1;
            break;
        case 0b01:
            std::swap(p1, p2);
            unsets[0] = 0;
            break;
        case 0b10:
            unsets[0] = 1;
            break;
    }
    switch(k2) {
        case 0b00:
            unsets[2] = 2;
            unsets[3] = 3;
            break;
        case 0b01:
            std::swap(p3, p4);
            unsets[2] = 2;
            break;
        case 0b10:
            unsets[2] = 3;
            break;
    }

    // Split cases based on how many args are set
    // Note: We only consider matches where l1 and l2 are known to be distinct (they may however have a point in common).

    switch(k1) {
        case 0b00: {
            switch(k2) {
                case 0b00: {
                    for (Direction* dir : ggraph.root_directions) {
                        auto gen_lines = dir->all_perp_pairs();
                        while (gen_lines) {
                            auto [l1, l2] = gen_lines();
                            auto gen_point_pairs1 = l1->all_point_pairs();
                            while (gen_point_pairs1) {
                                auto [pt1, pt2] = gen_point_pairs1();
                                pred_template->set_arg(0, pt1);
                                pred_template->set_arg(1, pt2);
                                auto gen_point_pairs2 = l2->all_point_pairs();
                                while (gen_point_pairs2) {
                                    auto [pt3, pt4] = gen_point_pairs2();
                                    pred_template->set_arg(2, pt3);
                                    pred_template->set_arg(3, pt4);
                                    co_yield true;
                                    pred_template->clear_arg(2);
                                    pred_template->clear_arg(3);
                                }
                                pred_template->clear_arg(0);
                                pred_template->clear_arg(1);
                            }
                        }
                    }
                    co_return;
                } break;
                case 0b01:
                case 0b10: {
                    auto gen_l2 = p3->on_lines();
                    while (gen_l2) {
                        l2 = gen_l2();
                        Direction* d2 = l2->direction;
                        if (!d2->has_perp()) continue;
                        d2 = d2->get_perp();
                        for (Line* l1 : d2->root_objs) {
                            auto gen_point_pairs1 = l1->all_point_pairs();
                            while (gen_point_pairs1) {
                                auto [pt1, pt2] = gen_point_pairs1();
                                pred_template->set_arg(0, pt1);
                                pred_template->set_arg(1, pt2);
                                auto gen_point_2 = l2->all_points();
                                while (gen_point_2) {
                                    auto pt3 = gen_point_2();
                                    if (pt3 != p3) {
                                        pred_template->set_arg(unsets[2], pt3);
                                        co_yield true;
                                        pred_template->clear_arg(unsets[2]);
                                    }
                                }
                                pred_template->clear_arg(0);
                                pred_template->clear_arg(1);
                            }
                        }
                    }
                    co_return;
                } break;
                case 0b11: {
                    l2 = ggraph.try_get_line(p3, p4);
                    if (l2) {
                        Direction* d2 = l2->direction;
                        for (Line* l1 : d2->root_objs) {
                            if (l1 == l2) continue;
                            auto gen_point_pairs1 = l1->all_point_pairs();
                            while (gen_point_pairs1) {
                                auto [pt1, pt2] = gen_point_pairs1();
                                pred_template->set_arg(0, pt1);
                                pred_template->set_arg(1, pt2);
                                co_yield true;
                                pred_template->clear_arg(0);
                                pred_template->clear_arg(1);
                            }
                        }
                    }
                    co_return;
                } break;
            }
        } break;
        case 0b01:
        case 0b10: {
            auto gen_l1 = p1->on_lines();
            while (gen_l1) {
                l1 = gen_l1();
                Direction* d1 = l1->direction;
                auto gen_point_1 = l1->all_points();
                while (gen_point_1) {
                    auto pt1 = gen_point_1();
                    if (pt1 == p1) continue;
                    pred_template->set_arg(unsets[0], pt1);
                    
                    switch(k2) {
                        case 0b00: {
                            for (Line* l2 : d1->root_objs) {
                                if (l2 == l1) continue;
                                auto gen_point_pairs2 = l2->all_point_pairs();
                                while (gen_point_pairs2) {
                                    auto [pt3, pt4] = gen_point_pairs2();
                                    pred_template->set_arg(2, pt3);
                                    pred_template->set_arg(3, pt4);
                                    co_yield true;
                                    pred_template->clear_arg(2);
                                    pred_template->clear_arg(3);
                                }
                            }
                        } break;
                        case 0b01:
                        case 0b10: {
                            auto gen_l2 = p3->on_lines();
                            while (gen_l2) {
                                l2 = gen_l2();
                                if (Line::is_para(l1, l2) && l1 != l2) {
                                    auto gen_point_2 = l2->all_points();
                                    while (gen_point_2) {
                                        auto pt3 = gen_point_2();
                                        if (pt3 != p3) {
                                            pred_template->set_arg(unsets[2], pt3);
                                            co_yield true;
                                            pred_template->clear_arg(unsets[2]);
                                        }
                                    }
                                }
                            }
                        } break;
                        case 0b11: {
                            l2 = ggraph.try_get_line(p3, p4);
                            if (l2) {
                                if (Line::is_perp(l1, l2)) {
                                    co_yield true;
                                }
                            }
                        } break;
                    
                    }
                    pred_template->clear_arg(unsets[0]);
                }
            }
            co_return;
        } break;
        case 0b11: {
            l1 = ggraph.try_get_line(p1, p2);
            if (l1) {
                switch(k2) {
                    case 0b00: {
                        Direction* d1 = l1->get_direction();
                        if (!d1->has_perp()) break;
                        d1 = d1->get_perp();
                        for (Line* l2 : d1->root_objs) {
                            auto gen_point_pairs2 = l2->all_point_pairs();
                            while (gen_point_pairs2) {
                                auto [pt3, pt4] = gen_point_pairs2();
                                pred_template->set_arg(2, pt3);
                                pred_template->set_arg(3, pt4);
                                co_yield true;
                                pred_template->clear_arg(2);
                                pred_template->clear_arg(3);
                            }
                        }
                    } break;
                    case 0b01:
                    case 0b10: {
                        auto gen_l2 = p3->on_lines();
                        while (gen_l2) {
                            l2 = gen_l2();
                            if (Line::is_perp(l1, l2) && l1 != l2) {
                                auto gen_point_2 = l2->all_points();
                                while (gen_point_2) {
                                    auto pt3 = gen_point_2();
                                    if (pt3 != p3) {
                                        pred_template->set_arg(unsets[2], pt3);
                                        co_yield true;
                                        pred_template->clear_arg(unsets[2]);
                                    }
                                }
                            }
                        }
                    } break;
                    case 0b11: {
                        l2 = ggraph.try_get_line(p3, p4);
                        if (l2) {
                            if (Line::is_perp(l1, l2) && l1 != l2) {
                                co_yield true;
                            }
                        }
                    } break;
                }
            }
            co_return;
        } break;
    }
    co_return;
}


Generator<bool> DDEngine::match(Theorem* theorem, int i, int n, GeometricGraph &ggraph) {
    if (i == n) {
        insert_predicate(theorem->instantiate_postcondition());
        std::cout << "Matched theorem: " << theorem->to_string() << std::endl;
        co_yield true;
        co_return;
    }
    PredicateTemplate* pred_template = theorem->preconditions.predicates[i].get();
    pred_t ptype = pred_template->name;
    if (ptype == pred_t::NCOLL || ptype == pred_t::NPARA) {
        co_yield true;
        co_return;
    }
    if (match_function_map.find(ptype) == match_function_map.end()) {
        co_return;
    }
    Generator<bool> pred_matcher = (this->*match_function_map[ptype])(pred_template, ggraph);
    
    while (pred_matcher) {
        if (pred_matcher()) {
            Generator<bool> rec = match(theorem, i + 1, n, ggraph);
            while (rec) {
                if (rec()) {
                    co_yield true;
                }
            }
        }
    }
    co_return;
}





void DDEngine::search(GeometricGraph &ggraph) {

    for (auto& thr : theorems) {
        int matches = 0;
        Theorem* theorem = thr.second.get();
        int n = theorem->preconditions.predicates.size();
        Generator<bool> gen = match(theorem, 0, n, ggraph);
        while (gen) {
            gen();
            matches += 1;
        }
        std::cout << "Matches for theorem " << theorem->to_string_with_placeholders() << ": " << matches << std::endl;
        theorem->__clear_args();
    }

}



bool DDEngine::check_conclusion(GeometricGraph &ggraph) {
    PredicateTemplate* conc = conclusion.get();
    // Just reuse the match functions lol
    Generator<bool> conc_matcher = (this->*match_function_map[conc->name])(conc, ggraph);
    while (conc_matcher) {
        if (conc_matcher()) {
            return true;
        }
    }
    return false;
}



void DDEngine::__print_theorems(std::ostream& os) {
    for (auto& thr : theorems) {
        Theorem* theorem = thr.second.get();
        os << theorem->to_string_with_placeholders() << std::endl;
    }
}

void DDEngine::__print_constructions(std::ostream& os) {
    for (auto& c : constructions) {
        Construction* construction = c.second.get();
        os << construction->to_string_with_placeholders() << std::endl;
    }
}

void DDEngine::__print_predicates(std::ostream& os) {
    for (auto& p : predicates) {
        Predicate* predicate = p.second.get();
        os << predicate->to_string() << std::endl;
    }
}

void DDEngine::__print_conclusion(std::ostream& os) {
    os << "Conclusion: " << conclusion->to_string() << std::endl;
}