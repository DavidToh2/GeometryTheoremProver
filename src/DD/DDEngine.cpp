
#include <map>
#include <memory>
#include <iostream>
#include <ostream>
#include <string>

#include "DD/DDEngine.hh"
#include "Common/Exceptions.hh"
#include "Common/Arg.hh"
#include "Geometry/Object.hh"
#include "Predicate.hh"
#include "Common/Generator.hh"
#include "Common/Constants.hh"
#include "Geometry/GeometricGraph.hh"

#include "Common/Debug.hh"
#if DEBUG_DDENGINE
    #define LOG(x) do {std::cout << x << std::endl;} while(0)
#else 
    #define LOG(x)
#endif


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

void DDEngine::add_theorem_template_from_text(const std::string s) { 

    std::unique_ptr<Theorem> _thr = std::make_unique<Theorem>(s);
    std::string name = _thr.get()->name;
    std::string cache_name = name;
    int i = 0;
    while (theorems.contains(cache_name)) {
        i++;
        cache_name = name + "_" + std::to_string(i);
    }
    theorems.insert({cache_name, std::move(_thr)});
}

void DDEngine::add_construction_template_from_texts(const std::tuple<std::string, std::string, std::string, std::string> v) { 

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




Predicate* DDEngine::insert_predicate(std::unique_ptr<Predicate> &&predicate) {
    Predicate* p = predicate.get();
    std::string hash = p->hash;
    if (has_predicate_by_hash(hash)) {
        predicate.reset();
        return predicates.at(hash).get();
    }
    predicates.insert({hash, std::move(predicate)});
    return p;
}

Predicate* DDEngine::insert_new_predicate(std::unique_ptr<Predicate> &&predicate) {
    Predicate* p = predicate.get();
    std::string hash = p->hash;
    if (hash == "contri e m b c m e") {
        int i = 1;
    }
    if (has_predicate_by_hash(hash)) {
        predicate.reset();
        return predicates.at(hash).get();
    }
    predicates.insert({hash, std::move(predicate)});
    recent_predicates.emplace_back(p);
    return p;
}

bool DDEngine::has_predicate_by_hash(const std::string hash) {
    return predicates.contains(hash);
}

Generator<Predicate*> DDEngine::get_recent_predicates() {
    while (!recent_predicates.empty()) {
        Predicate* p = recent_predicates.back();
        recent_predicates.pop_back();
        co_yield p;
    }
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
                auto gen_point_pairs = l1->all_point_pairs_ordered();
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
                auto gen_point_triples = l1->all_point_triples_ordered();
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
    bool a = pred_template->args[0]->filled();
    bool b = pred_template->args[1]->filled();
    bool c = pred_template->args[2]->filled();
    bool d = pred_template->args[3]->filled();
    bool s[4] = {a, b, c, d};
    int unsets[4];
    int k=0;
    for (int i=0; i<4; i++) {
        if (!s[i]) unsets[k++] = i;
    }
    Point* points[4];
    int j=0;
    for (int i=0; i<4; i++) {
        if (s[i]) {
            points[j++] = pred_template->args[i]->get_point();
        }
    }
    Circle* c1;

    // Split cases based on how many args are set
    switch(k) {
        case 0: { // All args are set
            if (ggraph.check_cyclic(points[0], points[1], points[2], points[3])) {
                co_yield true;
            }
            co_return;
        } break;
        case 1: { // Three args are set
            c1 = ggraph.try_get_circle(points[0], points[1], points[2]);
            if (c1) {
                auto gen_points = c1->all_points();
                while (gen_points) {
                    Point* pt = gen_points();
                    if (pt != points[0] && pt != points[1] && pt != points[2]) {
                        pred_template->set_arg(unsets[0], pt);
                        co_yield true;
                        pred_template->clear_arg(unsets[0]);
                    }
                }
            } 
            co_return;
        } break;
        case 2: { // Two args are set
            auto gen_circles = Circle::all_circles_through(points[0], points[1]);
            while (gen_circles) {
                c1 = gen_circles();
                auto gen_points = c1->all_point_pairs_ordered();
                while (gen_points) {
                    auto [pt2, pt3] = gen_points();
                    if (pt2 != points[0] && pt2 != points[1] && pt3 != points[0] && pt3 != points[1]) {
                        pred_template->set_arg(unsets[0], pt2);
                        pred_template->set_arg(unsets[1], pt3);
                        co_yield true;
                        pred_template->clear_arg(unsets[0]);
                        pred_template->clear_arg(unsets[1]);
                    }
                }
            }
            co_return;
        } break;
        case 3: { // One arg is set
            auto gen_circles = points[0]->on_circles();
            while (gen_circles) {
                c1 = gen_circles();
                auto gen_points = c1->all_point_triples_ordered();
                while (gen_points) {
                    auto [pt1, pt2, pt3] = gen_points();
                    if (pt1 != points[0] && pt2 != points[0] && pt3 != points[0]) {
                        pred_template->set_arg(unsets[0], pt1);
                        pred_template->set_arg(unsets[1], pt2);
                        pred_template->set_arg(unsets[2], pt3);
                        co_yield true;
                        pred_template->clear_arg(unsets[0]);
                        pred_template->clear_arg(unsets[1]);
                        pred_template->clear_arg(unsets[2]);
                    }
                }
            }
            co_return;
        } break;
        case 4: {
            for (Circle* c1 : ggraph.root_circles) {
                if (c1->points.size() < 4) {
                    continue;
                }
                auto gen_point_quads = c1->all_point_quads_ordered();
                while (gen_point_quads) {
                    auto [pt1, pt2, pt3, pt4] = gen_point_quads();
                    pred_template->set_arg(0, pt1);
                    pred_template->set_arg(1, pt2);
                    pred_template->set_arg(2, pt3);
                    pred_template->set_arg(3, pt4);
                    co_yield true;
                    pred_template->clear_arg(0);
                    pred_template->clear_arg(1);
                    pred_template->clear_arg(2);
                    pred_template->clear_arg(3);
                }
            }
            co_return;
        } break;
    }

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
                        auto gen_lines = dir->all_para_pairs_ordered();
                        while (gen_lines) {
                            auto [l1, l2] = gen_lines();
                            auto gen_point_pairs1 = l1->all_point_pairs_ordered();
                            while (gen_point_pairs1) {
                                auto [pt1, pt2] = gen_point_pairs1();
                                pred_template->set_arg(0, pt1);
                                pred_template->set_arg(1, pt2);

                                // Iterate through all point pairs on l2.
                                // This could be made more efficent by using `pred_template->arg_empty()` and
                                // `pred_template->get_arg_point()` to skip redundant cases. For clarity of 
                                // code, we leave this out for now.

                                auto gen_point_pairs2 = l2->all_point_pairs_ordered();
                                while (gen_point_pairs2) {
                                    auto [pt3, pt4] = gen_point_pairs2();
                                    char c2 = pred_template->set_arg(2, pt3);
                                    if (c2 != Arg::UNSUCCESSFUL_SET) {
                                        char c3 = pred_template->set_arg(3, pt4);
                                        if (c3 != Arg::UNSUCCESSFUL_SET) co_yield true;
                                        if (c3 == Arg::SUCCESSFUL_SET) pred_template->clear_arg(3);
                                    }
                                    if (c2 == Arg::SUCCESSFUL_SET) pred_template->clear_arg(2);
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
                            auto gen_point_pairs1 = l1->all_point_pairs_ordered();
                            while (gen_point_pairs1) {
                                auto [pt1, pt2] = gen_point_pairs1();
                                pred_template->set_arg(0, pt1);
                                pred_template->set_arg(1, pt2);
                                
                                auto gen_point_2 = l2->all_points();
                                while (gen_point_2) {
                                    auto pt3 = gen_point_2();
                                    if (pt3 != p3) {
                                        char c2 = pred_template->set_arg(unsets[2], pt3);
                                        if (c2 != Arg::UNSUCCESSFUL_SET) co_yield true;
                                        if (c2 == Arg::SUCCESSFUL_SET) pred_template->clear_arg(unsets[2]);
                                    }
                                }

                                pred_template->clear_arg(1);
                                pred_template->clear_arg(0);
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
                            auto gen_point_pairs1 = l1->all_point_pairs_ordered();
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
                                auto gen_point_pairs2 = l2->all_point_pairs_ordered();
                                while (gen_point_pairs2) {
                                    auto [pt3, pt4] = gen_point_pairs2();
                                    char c2 = pred_template->set_arg(2, pt3);
                                    if (c2 != Arg::UNSUCCESSFUL_SET) {
                                        char c3 = pred_template->set_arg(3, pt4);
                                        if (c3 != Arg::UNSUCCESSFUL_SET) co_yield true;
                                        if (c3 == Arg::SUCCESSFUL_SET) pred_template->clear_arg(3);
                                    }
                                    if (c2 == Arg::SUCCESSFUL_SET) pred_template->clear_arg(2);
                                }
                            }
                        } break;
                        case 0b01:
                        case 0b10: {
                            auto gen_l2 = p3->on_lines();
                            while (gen_l2) {
                                l2 = gen_l2();
                                if (ggraph.check_para(l1, l2) && l1 != l2) {
                                    auto gen_point_2 = l2->all_points();
                                    while (gen_point_2) {
                                        auto pt3 = gen_point_2();
                                        if (pt3 != p3) {
                                            char c2 = pred_template->set_arg(unsets[2], pt3);
                                            if (c2 != Arg::UNSUCCESSFUL_SET) co_yield true;
                                            if (c2 == Arg::SUCCESSFUL_SET) pred_template->clear_arg(unsets[2]);
                                        }
                                    }
                                }
                            }
                        } break;
                        case 0b11: {
                            l2 = ggraph.try_get_line(p3, p4);
                            if (l2) {
                                if (ggraph.check_para(l1, l2) && l1 != l2) {
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
                            auto gen_point_pairs2 = l2->all_point_pairs_ordered();
                            while (gen_point_pairs2) {
                                auto [pt1, pt2] = gen_point_pairs2();
                                pred_template->set_arg(2, pt1);
                                pred_template->set_arg(3, pt2);
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
                            if (ggraph.check_para(l1, l2) && l1 != l2) {
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
                            if (ggraph.check_para(l1, l2) && l1 != l2) {
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
                        auto gen_lines = dir->all_perp_pairs_ordered();
                        while (gen_lines) {
                            auto [l1, l2] = gen_lines();
                            auto gen_point_pairs1 = l1->all_point_pairs_ordered();
                            while (gen_point_pairs1) {
                                auto [pt1, pt2] = gen_point_pairs1();
                                pred_template->set_arg(0, pt1);
                                pred_template->set_arg(1, pt2);
                                auto gen_point_pairs2 = l2->all_point_pairs_ordered();
                                while (gen_point_pairs2) {
                                    auto [pt3, pt4] = gen_point_pairs2();
                                    char c2 = pred_template->set_arg(2, pt3);
                                    if (c2 != Arg::UNSUCCESSFUL_SET) {
                                        char c3 = pred_template->set_arg(3, pt4);
                                        if (c3 != Arg::UNSUCCESSFUL_SET) co_yield true;
                                        if (c3 == Arg::SUCCESSFUL_SET) pred_template->clear_arg(3);
                                    }
                                    if (c2 == Arg::SUCCESSFUL_SET) pred_template->clear_arg(2);
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
                            auto gen_point_pairs1 = l1->all_point_pairs_ordered();
                            while (gen_point_pairs1) {
                                auto [pt1, pt2] = gen_point_pairs1();
                                pred_template->set_arg(0, pt1);
                                pred_template->set_arg(1, pt2);
                                auto gen_point_2 = l2->all_points();
                                while (gen_point_2) {
                                    auto pt3 = gen_point_2();
                                    if (pt3 != p3) {
                                        char c2 = pred_template->set_arg(unsets[2], pt3);
                                        if (c2 != Arg::UNSUCCESSFUL_SET) co_yield true;
                                        if (c2 == Arg::SUCCESSFUL_SET) pred_template->clear_arg(unsets[2]);
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
                            auto gen_point_pairs1 = l1->all_point_pairs_ordered();
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
                if (!l1->has_direction()) continue;
                Direction* d1 = l1->get_direction();
                auto gen_point_1 = l1->all_points();
                while (gen_point_1) {
                    auto pt1 = gen_point_1();
                    if (pt1 == p1) continue;
                    pred_template->set_arg(unsets[0], pt1);
                    
                    switch(k2) {
                        case 0b00: {
                            for (Line* l2 : d1->root_objs) {
                                if (l2 == l1) continue;
                                auto gen_point_pairs2 = l2->all_point_pairs_ordered();
                                while (gen_point_pairs2) {
                                    auto [pt3, pt4] = gen_point_pairs2();
                                    char c2 = pred_template->set_arg(2, pt3);
                                    if (c2 != Arg::UNSUCCESSFUL_SET) {
                                        char c3 = pred_template->set_arg(3, pt4);
                                        if (c3 != Arg::UNSUCCESSFUL_SET) co_yield true;
                                        if (c3 == Arg::SUCCESSFUL_SET) pred_template->clear_arg(3);
                                    }
                                    if (c2 == Arg::SUCCESSFUL_SET) pred_template->clear_arg(2);
                                }
                            }
                        } break;
                        case 0b01:
                        case 0b10: {
                            auto gen_l2 = p3->on_lines();
                            while (gen_l2) {
                                l2 = gen_l2();
                                if (ggraph.check_perp(l1, l2) && l1 != l2) {
                                    auto gen_point_2 = l2->all_points();
                                    while (gen_point_2) {
                                        auto pt3 = gen_point_2();
                                        if (pt3 != p3) {
                                            char c2 = pred_template->set_arg(unsets[2], pt3);
                                            if (c2 != Arg::UNSUCCESSFUL_SET) co_yield true;
                                            if (c2 == Arg::SUCCESSFUL_SET) pred_template->clear_arg(unsets[2]);
                                        }
                                    }
                                }
                            }
                        } break;
                        case 0b11: {
                            l2 = ggraph.try_get_line(p3, p4);
                            if (l2) {
                                if (ggraph.check_perp(l1, l2)) {
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
                        if (!l1->has_direction()) break;
                        Direction* d1 = l1->get_direction();
                        if (!d1->has_perp()) break;
                        d1 = d1->get_perp();
                        for (Line* l2 : d1->root_objs) {
                            auto gen_point_pairs2 = l2->all_point_pairs_ordered();
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
                            if (ggraph.check_perp(l1, l2) && l1 != l2) {
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
                            if (ggraph.check_perp(l1, l2) && l1 != l2) {
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

Generator<bool> DDEngine::match_cong(PredicateTemplate* pred_template, GeometricGraph &ggraph) {
    char a = pred_template->args[0]->filled();
    char b = pred_template->args[1]->filled();
    char c = pred_template->args[2]->filled();
    char d = pred_template->args[3]->filled();
    char k1 = (a << 1) | b, k2 = (c << 1) | d;
    Point* p1 = pred_template->args[0]->get_point();
    Point* p2 = pred_template->args[1]->get_point();
    Point* p3 = pred_template->args[2]->get_point();
    Point* p4 = pred_template->args[3]->get_point();
    Segment* s1, *s2;
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

    switch(k1) {
        case 0b00: {
            switch(k2) {
                case 0b00: {
                    for (Length* l : ggraph.root_lengths) {
                        auto gen_cong_pairs = l->all_cong_pairs_ordered();
                        while (gen_cong_pairs) {
                            auto [s1, s2] = gen_cong_pairs();
                            auto [p1, p2] = s1->endpoints;
                            auto [p3, p4] = s2->endpoints;

                            std::array<std::array<Point*, 2>, 2> 
                                s1p{{{p1, p2}, {p2, p1}}}, s2p{{{p3, p4}, {p4, p3}}};

                            for (auto [pt1, pt2] : s1p) {
                                for (auto [pt3, pt4] : s2p) {
                                    pred_template->set_arg(0, pt1);
                                    pred_template->set_arg(1, pt2);
                                    
                                    char c2 = pred_template->set_arg(2, pt3);
                                    if (c2 != Arg::UNSUCCESSFUL_SET) {
                                        char c3 = pred_template->set_arg(3, pt4);
                                        if (c3 != Arg::UNSUCCESSFUL_SET) co_yield true;
                                        if (c3 == Arg::SUCCESSFUL_SET) pred_template->clear_arg(3);
                                    }
                                    if (c2 == Arg::SUCCESSFUL_SET) pred_template->clear_arg(2);

                                    pred_template->clear_arg(0);
                                    pred_template->clear_arg(1);
                                }
                            }
                            
                        }
                    }
                } break;
                case 0b01:
                case 0b10: {
                    auto gen_s2 = p3->endpoint_of_segments();
                    while (gen_s2) {
                        Segment* s2 = gen_s2();
                        if (!s2->has_length()) continue;
                        Point* pt4 = s2->other_endpoint(p3);
                        Length* l = s2->get_length();
                        for (Segment* s1 : l->root_objs) {
                            if (s1 == s2) continue;
                            auto [p1, p2] = s1->endpoints;
                            std::array<std::array<Point*, 2>, 2> s1p{{{p1, p2}, {p2, p1}}};
                            for (auto [pt1, pt2] : s1p) {
                                pred_template->set_arg(0, pt1);
                                pred_template->set_arg(1, pt2);

                                char c2 = pred_template->set_arg(unsets[2], pt4);
                                if (c2 != Arg::UNSUCCESSFUL_SET) co_yield true;
                                if (c2 == Arg::SUCCESSFUL_SET) pred_template->clear_arg(unsets[2]);

                                pred_template->clear_arg(0);
                                pred_template->clear_arg(1);
                            }
                        }
                    }
                } break;
                case 0b11: {
                    Segment* s2 = ggraph.try_get_segment(p3, p4);
                    if (s2 && s2->has_length()) {
                        Length* l = s2->get_length();
                        for (Segment* s1 : l->root_objs) {
                            if (s1 == s2) continue;
                            auto [p1, p2] = s1->endpoints;
                            std::array<std::array<Point*, 2>, 2> s1p{{{p1, p2}, {p2, p1}}};
                            for (auto [pt1, pt2] : s1p) {
                                pred_template->set_arg(0, pt1);
                                pred_template->set_arg(1, pt2);
                                co_yield true;
                                pred_template->clear_arg(0);
                                pred_template->clear_arg(1);
                            }
                        }
                    }
                } break;
            }
        } break;
        case 0b01:
        case 0b10: {
            auto gen_s1 = p1->endpoint_of_segments();
            while (gen_s1) {
                Segment* s1 = gen_s1();
                if (!s1->has_length()) continue;
                Point* pt2 = s1->other_endpoint(p1);
                pred_template->set_arg(unsets[0], pt2);
                Length* l = s1->get_length();
                switch(k2) {
                    case 0b00: {
                        for (Segment* s2 : l->root_objs) {
                            if (s2 == s1) continue;
                            auto [p3, p4] = s2->endpoints;
                            std::array<std::array<Point*, 2>, 2> s2p{{{p3, p4}, {p4, p3}}};
                            for (auto [pt3, pt4] : s2p) {
                                char c2 = pred_template->set_arg(2, pt3);
                                if (c2 != Arg::UNSUCCESSFUL_SET) {
                                    char c3 = pred_template->set_arg(3, pt4);
                                    if (c3 != Arg::UNSUCCESSFUL_SET) co_yield true;
                                    if (c3 == Arg::SUCCESSFUL_SET) pred_template->clear_arg(3);
                                }
                                if (c2 == Arg::SUCCESSFUL_SET) pred_template->clear_arg(2);
                            }
                        }
                    } break;
                    case 0b01:
                    case 0b10: {
                        for (Segment* s2 : l->root_objs) {
                            if (s2 == s1) continue;
                            if (Point* pt4 = s2->other_endpoint(p3)) {
                                char c2 = pred_template->set_arg(unsets[2], pt4);
                                if (c2 != Arg::UNSUCCESSFUL_SET) co_yield true;
                                if (c2 == Arg::SUCCESSFUL_SET) pred_template->clear_arg(unsets[2]);
                            }
                        }
                    } break;
                    case 0b11: {
                        s2 = ggraph.try_get_segment(p3, p4);
                        if (s2) {
                            if (s2 != s1 && ggraph.check_cong(s1, s2)) {
                                co_yield true;
                            }
                        }
                    } break;
                }
                pred_template->clear_arg(unsets[0]);
            }
        } break;
        case 0b11: {
            Segment* s1 = ggraph.try_get_segment(p1, p2);
            if (s1) {
                if (!s1->has_length()) co_return;
                Length* l = s1->get_length();
                switch(k2) {
                    case 0b00: {
                        for (Segment* s2 : l->root_objs) {
                            if (s2 == s1) continue;
                            auto [p3, p4] = s2->endpoints;
                            std::array<std::array<Point*, 2>, 2> s2p{{{p3, p4}, {p4, p3}}};
                            for (auto [pt3, pt4] : s2p) {
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
                        for (Segment* s2 : l->root_objs) {
                            if (s2 == s1) continue;
                            if (Point* pt4 = s2->other_endpoint(p3)) {
                                pred_template->set_arg(unsets[2], pt4);
                                co_yield true;
                                pred_template->clear_arg(unsets[2]);
                            }
                        }
                    } break;
                    case 0b11: {
                        Segment* s2 = ggraph.try_get_segment(p3, p4);
                        if (s2) {
                            if (s2 != s1 && ggraph.check_cong(s1, s2)) {
                                co_yield true;
                            }
                        }
                    } break;
                }
            }
        } break;
    }
}

Generator<bool> DDEngine::__match_eqangle(PredicateTemplate* pred_template, GeometricGraph &ggraph, int i, std::array<Direction*, 4> &ds) {
    if (i == 4) {
        co_yield true;
        co_return;
    }
    Direction* d = ds[i];
    Point* p1 = pred_template->args[i*2]->get_point();
    Point* p2 = pred_template->args[i*2 + 1]->get_point();
    char a = pred_template->args[i*2]->filled();
    char b = pred_template->args[i*2 + 1]->filled();
    char k = (a << 1) | b;

    switch(k) {
        case 0b11: {
            for (auto l : d->root_objs) {
                if (l->contains(p1) && l->contains(p2)) {
                    auto rec = __match_eqangle(pred_template, ggraph, i + 1, ds);
                    while (rec) {
                        if (rec()) {
                            co_yield true;
                        }
                    }
                    break;
                }
            }
            co_return;
        } break;
        case 0b10: {
            for (auto l : d->root_objs) {
                if (l->contains(p1)) {
                    auto gen_points = l->all_points();
                    while (gen_points) {
                        Point* pt = gen_points();
                        if (pt != p1) {
                            pred_template->set_arg(i*2 + 1, pt);
                            auto rec = __match_eqangle(pred_template, ggraph, i + 1, ds);
                            while (rec) {
                                if (rec()) {
                                    co_yield true;
                                }
                            }
                            pred_template->clear_arg(i*2 + 1);
                        }
                    }
                    break;
                }
            }
            co_return;
        } break;
        case 0b01: {
            for (auto l : d->root_objs) {
                if (l->contains(p2)) {
                    auto gen_points = l->all_points();
                    while (gen_points) {
                        Point* pt = gen_points();
                        if (pt != p2) {
                            pred_template->set_arg(i*2, pt);
                            auto rec = __match_eqangle(pred_template, ggraph, i + 1, ds);
                            while (rec) {
                                if (rec()) {
                                    co_yield true;
                                }
                            }
                            pred_template->clear_arg(i*2);
                        }
                    }
                }
            }
            co_return;
        } break;
        case 0b00: {
            for (auto l : d->root_objs) {
                auto gen_point_pairs = l->all_point_pairs_ordered();
                while (gen_point_pairs) {
                    auto [pt1, pt2] = gen_point_pairs();
                    pred_template->set_arg(i*2, pt1);
                    pred_template->set_arg(i*2 + 1, pt2);   
                    auto rec = __match_eqangle(pred_template, ggraph, i + 1, ds);
                    while (rec) {
                        if (rec()) {
                            co_yield true;
                        }
                    }
                    pred_template->clear_arg(i*2);
                    pred_template->clear_arg(i*2 + 1);
                }
            }
            co_return;
        } break;
    }
    co_return;
}

Generator<bool> DDEngine::match_eqangle(PredicateTemplate* pred_template, GeometricGraph &ggraph) {

    if (pred_template->args_filled()) {

        Point* p[8] = {
            pred_template->get_arg_point(0),
            pred_template->get_arg_point(1),
            pred_template->get_arg_point(2),
            pred_template->get_arg_point(3),
            pred_template->get_arg_point(4),
            pred_template->get_arg_point(5),
            pred_template->get_arg_point(6),
            pred_template->get_arg_point(7)
        };
        Line* l[4] = {
            ggraph.try_get_line(p[0], p[1]),
            ggraph.try_get_line(p[2], p[3]),
            ggraph.try_get_line(p[4], p[5]),
            ggraph.try_get_line(p[6], p[7])
        };
        if (!l[0] || !l[1] || !l[2] || !l[3]) {
            co_return;
        }
        if (!l[0]->has_direction() || !l[1]->has_direction() || !l[2]->has_direction() || !l[3]->has_direction()) {
            co_return;
        }
        Direction* d[4] = {
            l[0]->get_direction(),
            l[1]->get_direction(),
            l[2]->get_direction(),
            l[3]->get_direction()
        };
        Angle* a[2] = {
            ggraph.try_get_angle(d[0], d[1]),
            ggraph.try_get_angle(d[2], d[3])
        };
        if (!a[0] || !a[1]) {
            co_return;
        }
        if (ggraph.check_eqangle(a[0], a[1])) {
            co_yield true;
        }
        co_return;

    } else {

        if (ggraph.root_measures.size() > 0) {
            for (Measure* m : ggraph.root_measures) {
                auto gen_angle_pairs = m->all_eq_pairs_ordered();
                while (gen_angle_pairs) {
                    auto [angle1, angle2] = gen_angle_pairs();

                    // Match points for the first angle
                    Direction* d1 = angle1->direction1, *d2 = angle1->direction2, *d3 = angle2->direction1, *d4 = angle2->direction2;
                    std::array<std::array<Direction*, 4>, 4> dss = {
                        std::array<Direction*, 4>{d1, d2, d3, d4},
                        std::array<Direction*, 4>{d3, d4, d1, d2},
                        std::array<Direction*, 4>{d2, d1, d4, d3},
                        std::array<Direction*, 4>{d4, d3, d2, d1}
                    };
                    for (std::array<Direction*, 4> &ds : dss) {
                        auto gen =  __match_eqangle(pred_template, ggraph, 0, ds);
                        while (gen) {
                            if (gen()) {
                                co_yield true;
                            }
                        }
                    }
                }
            }
        } 
        // else if (ggraph.root_angles.size() > 0) {
        //     for (Angle* a : ggraph.root_angles) {
        //         Direction* d1 = a->direction1, *d2 = a->direction2; 
        //         std::array<std::array<Direction*, 4>, 2> dss = { 
        //             std::array<Direction*, 4>{d1, d2, d1, d2},  
        //             std::array<Direction*, 4>{d2, d1, d2, d1} 
        //         }; 
        //         for (std::array<Direction*, 4> &ds : dss) { 
        //             auto gen = __match_eqangle(pred_template, ggraph, 0, ds); 
        //             while (gen) { 
        //                 if (gen()) { 
        //                     co_yield true; 
        //                 } 
        //             } 
        //         }
        //     }
        // }
        
    }
    co_return;
}

Generator<bool> DDEngine::__match_eqratio(PredicateTemplate* pred_template, GeometricGraph &ggraph, int i, std::array<Length*, 4> &ls) {
    if (i == 4) {
        co_yield true;
        co_return;
    }
    Length* l = ls[i];
    Point* p1 = pred_template->args[i*2]->get_point();
    Point* p2 = pred_template->args[i*2 + 1]->get_point();
    char a = pred_template->args[i*2]->filled();
    char b = pred_template->args[i*2 + 1]->filled();
    char k = (a << 1) | b;

    switch(k) {
        case 0b11: {
            for (auto s : l->root_objs) {
                if (s->has_endpoints(p1, p2)) {
                    auto rec = __match_eqratio(pred_template, ggraph, i + 1, ls);
                    while (rec) {
                        if (rec()) {
                            co_yield true;
                        }
                    }
                }
            }
            co_return;
        } break;
        case 0b10: {
            for (auto s : l->root_objs) {
                if (Point* pt = s->other_endpoint(p1)) {
                    pred_template->set_arg(i*2 + 1, pt);
                    auto rec = __match_eqratio(pred_template, ggraph, i + 1, ls);
                    while (rec) {
                        if (rec()) {
                            co_yield true;
                        }
                    }
                    pred_template->clear_arg(i*2 + 1);
                }
            }
            co_return;
        } break;
        case 0b01: {
            for (auto s : l->root_objs) {
                if (Point* pt = s->other_endpoint(p2)) {
                    pred_template->set_arg(i*2, pt);
                    auto rec = __match_eqratio(pred_template, ggraph, i + 1, ls);
                    while (rec) {
                        if (rec()) {
                            co_yield true;
                        }
                    }
                    pred_template->clear_arg(i*2);
                }
            }
            co_return;
        } break;
        case 0b00: {
            for (auto s : l->root_objs) {
                auto [p1, p2] = s->endpoints;
                std::array<std::array<Point*, 2>, 2> sp{{{p1, p2}, {p2, p1}}};
                for (auto [pt1, pt2] : sp) {
                    pred_template->set_arg(i*2, pt1);
                    pred_template->set_arg(i*2 + 1, pt2);
                    auto rec = __match_eqratio(pred_template, ggraph, i + 1, ls);
                    while (rec) {
                        if (rec()) {
                            co_yield true;
                        }
                    }
                    pred_template->clear_arg(i*2 + 1);
                    pred_template->clear_arg(i*2);
                }
            }
            co_return;
        } break;
    }
    co_return;
}

Generator<bool> DDEngine::match_eqratio(PredicateTemplate* pred_template, GeometricGraph &ggraph) {

    if (pred_template->args_filled()) {

        Point* p[8] = {
            pred_template->get_arg_point(0),
            pred_template->get_arg_point(1),
            pred_template->get_arg_point(2),
            pred_template->get_arg_point(3),
            pred_template->get_arg_point(4),
            pred_template->get_arg_point(5),
            pred_template->get_arg_point(6),
            pred_template->get_arg_point(7)
        };
        Segment* s[4] = {
            ggraph.try_get_segment(p[0], p[1]),
            ggraph.try_get_segment(p[2], p[3]),
            ggraph.try_get_segment(p[4], p[5]),
            ggraph.try_get_segment(p[6], p[7])
        };
        if (!s[0] || !s[1] || !s[2] || !s[3]) {
            co_return;
        }
        if (!s[0]->has_length() || !s[1]->has_length() || !s[2]->has_length() || !s[3]->has_length()) {
            co_return;
        }
        Length* l[4] = {
            s[0]->get_length(),
            s[1]->get_length(),
            s[2]->get_length(),
            s[3]->get_length()
        };
        if (!ggraph.check_eqratio(l[0], l[1], l[2], l[3])) {
            co_return;
        }
        Ratio* r[2] = {
            ggraph.try_get_ratio(l[0], l[1]),
            ggraph.try_get_ratio(l[2], l[3])
        };
        if (!r[0] || !r[1]) {
            co_return;
        }
        if (ggraph.check_eqratio(r[0], r[1])) {
            co_yield true;
        }
        co_return;

    } else {

        for (Fraction* f : ggraph.root_fractions) {
            auto gen_segment_pairs = f->all_eq_pairs_ordered();
            while (gen_segment_pairs) {
                auto [ratio1, ratio2] = gen_segment_pairs();

                // Match points for the first ratio
                Length* l1 = ratio1->length1, *l2 = ratio1->length2, *l3 = ratio2->length1, *l4 = ratio2->length2;
                std::array<std::array<Length*, 4>, 8> lss = {
                    std::array<Length*, 4>{l1, l2, l3, l4},
                    std::array<Length*, 4>{l1, l3, l2, l4},

                    std::array<Length*, 4>{l2, l1, l4, l3},
                    std::array<Length*, 4>{l2, l4, l1, l3},

                    std::array<Length*, 4>{l3, l4, l1, l2},
                    std::array<Length*, 4>{l3, l1, l4, l2},

                    std::array<Length*, 4>{l4, l3, l2, l1},
                    std::array<Length*, 4>{l4, l2, l3, l1}
                };
                for (std::array<Length*, 4> &ls : lss) {
                    auto gen =  __match_eqratio(pred_template, ggraph, 0, ls);
                    while (gen) {
                        if (gen()) {
                            co_yield true;
                        }
                    }
                }
            }
        }
        
    }
    co_return;
}

Generator<bool> DDEngine::match_midp(PredicateTemplate* pred_template, GeometricGraph &ggraph) {
    char a = pred_template->args[0]->filled();
    char b = pred_template->args[1]->filled();
    char c = pred_template->args[2]->filled();
    char k = (b << 1) | c;
    Point* m = pred_template->args[0]->get_point();
    Point* p1 = pred_template->args[1]->get_point();
    Point* p2 = pred_template->args[2]->get_point();
    Segment* s1, *s2;
    int unsets;
    
    if (k == 0b01) {
        std::swap(p1, p2);
        unsets = 1;
    } else if (k == 0b10) {
        unsets = 2;
    }

    if (a) {
        switch(k) {
            case 0b00: {
                auto gen_seg_pairs = m->endpoint_of_segment_pairs_ordered();
                while (gen_seg_pairs) {
                    auto [s1, s2] = gen_seg_pairs();
                    if (!ggraph.check_coll(s1, s2)) continue;
                    if (!ggraph.check_cong(s1, s2)) continue;
                    Point* pt1 = s1->other_endpoint(m);
                    Point* pt2 = s2->other_endpoint(m);

                    pred_template->set_arg(1, pt1);
                    pred_template->set_arg(2, pt2);
                    co_yield true;
                    pred_template->clear_arg(1);
                    pred_template->clear_arg(2);
                }
            } break;
            case 0b01:
            case 0b10: {
                Segment* s1 = ggraph.try_get_segment(m, p1);
                if (s1) {
                    Length* l1 = s1->get_length();
                    for (Segment* s2 : l1->root_objs) {
                        if (s1 == s2) continue;
                        if (Point* pt = s2->other_endpoint(m)) {
                            if (!ggraph.check_coll(s1, s2)) continue;
                            pred_template->set_arg(unsets, pt);
                            co_yield true;
                            pred_template->clear_arg(unsets);
                        }
                    }
                }
            } break;
            case 0b11: {
                if (ggraph.check_midp(m, p1, p2)) {
                    co_yield true;
                }
            } break;
        }
    } else {
        switch(k) {
            case 0b00: {
                for (Length* l : ggraph.root_lengths) {
                    auto gen_seg_pairs = l->all_cong_pairs_ordered();
                    while (gen_seg_pairs) {
                        auto [s1, s2] = gen_seg_pairs();
                        if (!ggraph.check_coll(s1, s2)) continue;
                        auto [pt1, pt2] = s1->endpoints;
                        if (Point* pt3 = s2->other_endpoint(pt2)) {
                            pred_template->set_arg(0, pt2);
                            pred_template->set_arg(1, pt1);
                            pred_template->set_arg(2, pt3);
                            co_yield true;
                            pred_template->clear_arg(0);
                            pred_template->clear_arg(1);
                            pred_template->clear_arg(2);
                        }
                        if (Point* pt3 = s2->other_endpoint(pt1)) {
                            pred_template->set_arg(0, pt1);
                            pred_template->set_arg(1, pt2);
                            pred_template->set_arg(2, pt3);
                            co_yield true;
                            pred_template->clear_arg(0);
                            pred_template->clear_arg(1);
                            pred_template->clear_arg(2);
                        }
                    }
                }
            } break;
            case 0b10: {
                auto gen_seg_1 = p1->endpoint_of_segments();
                while (gen_seg_1) {
                    s1 = gen_seg_1();
                    Point* pm = s1->other_endpoint(p1);
                    Length* l1 = s1->get_length();
                    for (Segment* s2 : l1->root_objs) {
                        if (s1 == s2) continue;
                        if (!ggraph.check_coll(s1, s2)) continue;
                        if (Point* pt2 = s2->other_endpoint(pm)) {
                            pred_template->set_arg(0, pm);
                            pred_template->set_arg(unsets, pt2);
                            co_yield true;
                            pred_template->clear_arg(0);
                            pred_template->clear_arg(unsets);
                        }
                    }
                }
            } break;
            case 0b11: {
                auto gen_seg_1 = p1->endpoint_of_segments();
                while (gen_seg_1) {
                    s1 = gen_seg_1();
                    Point* pm = s1->other_endpoint(p1);
                    s2 = ggraph.try_get_segment(pm, p2);
                    if (!s2) continue;
                    if (!ggraph.check_coll(s1, s2)) continue;
                    if (ggraph.check_cong(s1, s2)) {
                        pred_template->set_arg(0, pm);
                        co_yield true;
                        pred_template->clear_arg(0);
                    }
                }
            } break;
        }
    }

    co_return;
}

Generator<bool> DDEngine::match_circle(PredicateTemplate* pred_template, GeometricGraph &ggraph) {
    char a = pred_template->args[0]->filled();
    char b = pred_template->args[1]->filled();
    char c = pred_template->args[2]->filled();
    char d = pred_template->args[3]->filled();
    char k = (b << 2) | (c << 1) | d;
    Point* cp = pred_template->args[0]->get_point();
    Point* p1 = pred_template->args[1]->get_point();
    Point* p2 = pred_template->args[2]->get_point();
    Point* p3 = pred_template->args[3]->get_point();
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

    if (a) {
        switch(k) {
            case 0b000: {

            } break;
            case 0b001:
            case 0b010:
            case 0b100: {

            } break;
            case 0b011:
            case 0b101:
            case 0b110: {

            } break;
            case 0b111: {

            } break;
        }
    } else {
        switch(k) {
            case 0b000: {
                for (Circle* c : ggraph.root_circles) {
                    if (!c->has_center()) continue;
                    if (c->points.size() < 3) continue;
                    cp = c->get_center();
                    pred_template->set_arg(0, cp);

                    auto gen_point_triples = c->all_point_triples_ordered();
                    while (gen_point_triples) {
                        auto [pt1, pt2, pt3] = gen_point_triples();
                        pred_template->set_arg(1, pt1);
                        pred_template->set_arg(2, pt2);
                        pred_template->set_arg(3, pt3);
                        co_yield true;
                        pred_template->clear_arg(1);
                        pred_template->clear_arg(2);
                        pred_template->clear_arg(3);
                    }
                    pred_template->clear_arg(0);
                }
            } break;
            case 0b001:
            case 0b010:
            case 0b100: {
                auto gen_circ = p1->on_circles();
                while (gen_circ) {
                    Circle* c = gen_circ();
                    cp = c->get_center();
                    pred_template->set_arg(0, cp);

                    auto gen_point_pairs = c->all_point_pairs_ordered();
                    while (gen_point_pairs) {
                        auto [pt2, pt3] = gen_point_pairs();
                        if (pt2 != p1 && pt3 != p1) {
                            pred_template->set_arg(unsets[0], pt2);
                            pred_template->set_arg(unsets[1], pt3);
                            co_yield true;
                            pred_template->clear_arg(unsets[0]);
                            pred_template->clear_arg(unsets[1]);
                        }
                    }
                    pred_template->clear_arg(0);
                }
            } break;
            case 0b011:
            case 0b101:
            case 0b110: {
                auto gen_circ = p1->on_circles();
                while (gen_circ) {
                    Circle* c = gen_circ();
                    if (!c->has_center()) continue;
                    if (c->contains(p2)) {
                        cp = c->get_center();
                        pred_template->set_arg(0, cp);

                        auto gen_p3 = c->all_points();
                        while (gen_p3) {
                            Point* pt3 = gen_p3();
                            if (pt3 != p1 && pt3 != p2) {
                                pred_template->set_arg(unsets[0], pt3);
                                co_yield true;
                                pred_template->clear_arg(unsets[0]);
                            }
                        }
                        pred_template->clear_arg(0);
                    }
                }
            } break;
            case 0b111: {
                Circle* c = ggraph.try_get_circle(p1, p2, p3);
                if (c && c->has_center() && c->contains(p2) && c->contains(p3)) {
                    cp = c->get_center();
                    pred_template->set_arg(0, cp);
                    co_yield true;
                    pred_template->clear_arg(0);
                }
            } break;
        }
    }
    co_return;
}

Generator<bool> DDEngine::match_diff(PredicateTemplate* pred_template, GeometricGraph &ggraph) {
    if (!pred_template->args_filled()) {
        throw DDInternalError("DIFF predicate requires all arguments to be set for matching.");
    }
    std::set<Point*> pts;
    for (Arg* arg : pred_template->args) {
        Point* p = arg->get_point();
        if (pts.contains(p)) co_return;
        pts.insert(p);
    }
    co_yield ggraph.check_diff(pts);
    co_return;
}

Generator<bool> DDEngine::match_ncoll(PredicateTemplate* pred_template, GeometricGraph &ggraph) {
    if (!pred_template->args_filled()) {
        throw DDInternalError("NCOLL predicate requires all arguments to be set for matching.");
    }
    int i = 0;
    std::set<Point*> pts;
    for (Arg* arg : pred_template->args) {
        Point* p = arg->get_point();
        if (i < 2) {
            auto [_, res] = pts.insert(p);
            if (res) i += 1;
            else co_return;
            continue;
        }
        if (pts.contains(p)) co_return;
        for (auto it = pts.begin(); it != pts.end(); ++it) {
            for (auto it2 = std::next(it); it2 != pts.end(); ++it2) {
                Point* p1 = *it, *p2 = *it2;
                Line* l = ggraph.try_get_line(p1, p2);
                if (!l) continue;
                if (l->contains(p)) {
                    co_return;
                }
            }
        }
        pts.insert(p);
    }
    co_yield ggraph.check_ncoll(pts);
    co_return;
}

Generator<bool> DDEngine::match_npara(PredicateTemplate* pred_template, GeometricGraph &ggraph) {
    if (!pred_template->args[0]->filled() || !pred_template->args[1]->filled() || !pred_template->args[2]->filled() || !pred_template->args[3]->filled()) {
        throw DDInternalError("NPARA predicate requires all four arguments to be set for matching.");
    }
    Point* p1 = pred_template->args[0]->get_point();
    Point* p2 = pred_template->args[1]->get_point();
    Point* p3 = pred_template->args[2]->get_point();
    Point* p4 = pred_template->args[3]->get_point();
    Line* l1 = ggraph.try_get_line(p1, p2);
    Line* l2 = ggraph.try_get_line(p3, p4);
    if (!l1 || !l2) {
        co_yield true;
        co_return;
    }
    co_yield !(ggraph.check_para(l1, l2));
    co_return;
}

Generator<bool> DDEngine::match_sameclock(PredicateTemplate* pred_template, GeometricGraph &ggraph) {
    if (!pred_template->args_filled()) {
        throw DDInternalError("SAMECLOCK predicate requires all arguments to be set for matching.");
    }
    Point* p1 = pred_template->args[0]->get_point();
    Point* p2 = pred_template->args[1]->get_point();
    Point* p3 = pred_template->args[2]->get_point();
    Point* p4 = pred_template->args[3]->get_point();
    Point* p5 = pred_template->args[4]->get_point();
    Point* p6 = pred_template->args[5]->get_point();
    co_yield ggraph.check_same_orientation(p1, p2, p3, p4, p5, p6);
    co_return;
}

Generator<bool> DDEngine::match_diffclock(PredicateTemplate* pred_template, GeometricGraph &ggraph) {
    if (!pred_template->args_filled()) {
        throw DDInternalError("DIFFCLOCK predicate requires all arguments to be set for matching.");
    }
    Point* p1 = pred_template->args[0]->get_point();
    Point* p2 = pred_template->args[1]->get_point();
    Point* p3 = pred_template->args[2]->get_point();
    Point* p4 = pred_template->args[3]->get_point();
    Point* p5 = pred_template->args[4]->get_point();
    Point* p6 = pred_template->args[5]->get_point();
    co_yield !(ggraph.check_same_orientation(p1, p2, p3, p4, p5, p6));
    co_return;
}

Generator<bool> DDEngine::match(Theorem* theorem, int i, int n, GeometricGraph &ggraph) {
    
    if (i == n) {
        if (!ggraph.check(theorem->postcondition.get())) {

            std::unique_ptr<Predicate> pred_ = theorem->instantiate_postcondition();
            Predicate* pred = pred_.get();

            auto whys_ = theorem->instantiate_preconditions();
            while (whys_) {
                Predicate* why = insert_predicate(std::move(whys_()));
                pred->why += why;
            }

            insert_new_predicate(std::move(pred_));

            co_yield true;

        }
        co_return;
    }
    PredicateTemplate* pred_template = theorem->preconditions.predicates[i].get();
    pred_t ptype = pred_template->name;

    if (!match_function_map.contains(ptype)) {
        co_return;
    }
    Generator<bool> pred_matcher = (this->*match_function_map[ptype])(pred_template, ggraph);
    
    while (pred_matcher) {
        if (pred_matcher()) {
            // Skip over matches where the postcondition is already known
            if (ggraph.check(theorem->postcondition.get())) continue;

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
        if (thr.first == "cong_eqangle_eqangle_ncoll_contri") {
            int i = 1;
        }
        int matches = 0;
        Theorem* theorem = thr.second.get();
        int n = theorem->preconditions.predicates.size();
        Generator<bool> gen = match(theorem, 0, n, ggraph);
        while (gen) {
            if (gen()) {
                matches += 1;
            }
        }
        LOG("Matches for theorem " << theorem->to_string_with_placeholders() << ": " << matches);
        theorem->__clear_args();
    }

}



bool DDEngine::check_postcondition_exact(PredicateTemplate* postcondition) {
    return (
        postcondition->args_filled() &&
        predicates.contains(postcondition->to_hash_with_args())
    );
}

bool DDEngine::check_conclusion(GeometricGraph &ggraph) {
    PredicateTemplate* conc = conclusion.get();
    return ggraph.check(conc);
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



void DDEngine::reset_problem() {
    predicates.clear();
    predicate2s.clear();

    recent_predicates.clear();
    for (auto& [pt, pt_set] : predicates_by_type) {
        pt_set.clear();
    }

    conclusion.reset();
    conclusion_args.clear();
}