
#include <iostream>

#include "NumEngine.hh"
#include "Common/Exceptions.hh"
#include "Numerics/Cartesian.hh"

#define DEBUG_NUMENGINE 1

#if DEBUG_NUMENGINE
    #define LOG(x) do {std::cout << x << std::endl;} while(0)
#else 
    #define LOG(x)
#endif

Numeric* NumEngine::insert_numeric(std::unique_ptr<Numeric> &&num) {
    numerics.emplace_back(std::move(num));
    Numeric* n = numerics.back().get();
    for (Point* p : n->outs) {
        point_status[p] = ComputationStatus::UNCOMPUTED;
    }
    return n;
}

Generator<CartesianPoint> NumEngine::compute_free(Numeric* num) {
    co_yield Cartesian::random_point();
    co_return;
}
Generator<CartesianPoint> NumEngine::compute_segment(Numeric* num) {
    co_yield Cartesian::random_point();
    co_yield Cartesian::random_point();
    co_return;
}
Generator<CartesianPoint> NumEngine::compute_triangle(Numeric* num) {
    co_yield Cartesian::random_point();
    co_yield Cartesian::random_point();
    co_yield Cartesian::random_point();
    co_return;
}



bool NumEngine::compute_one(Numeric* num) {

    for (Point* p : num->args) {
        if (point_status[p] < ComputationStatus::RESOLVED) {
            LOG("Point argument " << p->name << " not yet resolved computation.");
            return false;
        }
    }
    for (Point* p : num->outs) {
        if (point_status[p] >= ComputationStatus::RESOLVED) {
            LOG("Point output " << p->name << " already resolved computation.");
            return false;
        }
    }
    num_t name = num->name;

    int i = 0, l = num->outs.size();
    if (compute_function_map_point.contains(name)) {
        auto gen = (this->*compute_function_map_point[name])(num);
        while (gen) {
            point_to_cartesian[num->outs[i++]].emplace_back(gen());
        }
    } else if (compute_function_map_line.contains(name)) {
        auto gen = (this->*compute_function_map_line[name])(num);
        while (gen) {
            point_to_cartesian_objs[num->outs[i++]].emplace_back(gen());
        }
    } else if (compute_function_map_circle.contains(name)) {
        auto gen = (this->*compute_function_map_circle[name])(num);
        while (gen) {
            point_to_cartesian_objs[num->outs[i++]].emplace_back(gen());
        }
    } else {
        LOG("No compute function for numeric " << Utils::to_num_str(name));
    }
    return true;
}

void NumEngine::compute() {
    auto it = numerics.begin();
    bool computing = true;

    while (it != numerics.end()) {
        Numeric* num = it->get();
        if (compute_one(num)) {
            computing = true;
            it++;
        } else {
            if (!computing) {
                throw NumericsInternalError("Could not compute numeric " + Utils::to_num_str(num->name) + " due to unresolved dependencies.");
                return;
            }
            computing = false;
            if (!resolve()) {
                LOG("Could not resolve all points needed for numeric " << Utils::to_num_str(num->name));
            }
        }
    }
    resolve();
}



bool NumEngine::resolve_one(Point* p) {

    switch(point_to_cartesian_objs[p].size()) {
        case 0: 
        case 1: {
            // do nothing
        }
        break;
        default: {
            CartesianObject obj1 = point_to_cartesian_objs[p][0];
            for (auto it = point_to_cartesian_objs[p].begin() + 1; it != point_to_cartesian_objs[p].end(); ++it) {
                CartesianObject obj2 = *it;
                auto intersect_gen = Cartesian::intersect(obj1, obj2);
                while (intersect_gen) {
                    CartesianPoint cp = intersect_gen();
                    bool found_match = false;
                    for (auto& p0 : point_to_cartesian[p]) {
                        if (CartesianPoint::is_same(cp, p0)) {
                            found_match = true;
                            break;
                        }
                    }
                    if (!found_match) {
                        point_to_cartesian[p].emplace_back(cp);
                    }
                }
            }
        }
        break;
    }

    switch(point_to_cartesian[p].size()) {
        case 0: 
            throw NumericsInternalError("Insufficient information to resolve point " + p->name + " to Cartesian coordinates.");
        case 1:
            return true;
        default:
            return false;
    }
}
bool NumEngine::resolve() {
    bool res = true;
    for (auto& [p, status] : point_status) {
        if (status == ComputationStatus::COMPUTING) {
            if (resolve_one(p)) {
                status = ComputationStatus::RESOLVED;
            } else {
                status = ComputationStatus::RESOLVED_WITH_DISCREPANCY;
                res = false;
            }
        }
    }
    return res;
}



void NumEngine::reset_computation() {
    point_to_cartesian.clear();
    for (auto & [p, _] : point_status) {
        point_status[p] = ComputationStatus::UNCOMPUTED;
    }
}
void NumEngine::reset_problem() {
    numerics.clear();
    point_to_cartesian.clear();
    point_status.clear();
}