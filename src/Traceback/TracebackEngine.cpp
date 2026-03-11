
#include <iostream>

#include "TracebackEngine.hh"
#include "Common/Exceptions.hh"
#include "Geometry/Value.hh"


void TracebackEngine::record_merge(Point* dest, Point* src) {
    point_line_root_map[dest].merge(point_line_root_map[src]);
    point_line_root_map.erase(src);
    point_circle_root_map[dest].merge(point_circle_root_map[src]);
    point_circle_root_map.erase(src);
    point_circle_center_root_map[dest].merge(point_circle_center_root_map[src]);
    point_circle_center_root_map.erase(src);
    point_segment_endpoint_root_map[dest].merge(point_segment_endpoint_root_map[src]);
    point_segment_endpoint_root_map.erase(src);
    point_triangle_vertex_root_map[dest].merge(point_triangle_vertex_root_map[src]);
    point_triangle_vertex_root_map.erase(src);
}
void TracebackEngine::record_merge(Line* dest, Line* src) {
    for (auto it = point_line_root_map.begin(); it != point_line_root_map.end(); ++it) {
        Point* p = it->first;
        auto &line_map = it->second;
        if (line_map.contains(src)) {
            auto pair = line_map.extract(src);
            pair.key() = dest;
            line_map.insert(std::move(pair));
        }
    }
    for (auto it = direction_line_root_map.begin(); it != direction_line_root_map.end(); ++it) {
        Direction* d = it->first;
        auto &line_map = it->second;
        if (line_map.contains(src)) {
            auto pair = line_map.extract(src);
            pair.key() = dest;
            line_map.insert(std::move(pair));
        }
    }
}
void TracebackEngine::record_merge(Circle* dest, Circle* src) {
    for (auto it = point_circle_root_map.begin(); it != point_circle_root_map.end(); ++it) {
        Point* p = it->first;
        auto &circle_map = it->second;
        if (circle_map.contains(src)) {
            auto pair = circle_map.extract(src);
            pair.key() = dest;
            circle_map.insert(std::move(pair));
        }
    }
    for (auto it = point_circle_center_root_map.begin(); it != point_circle_center_root_map.end(); ++it) {
        Point* p = it->first;
        auto &circle_map = it->second;
        if (circle_map.contains(src)) {
            auto pair = circle_map.extract(src);
            pair.key() = dest;
            circle_map.insert(std::move(pair));
        }
    }
}
void TracebackEngine::record_merge(Segment* dest, Segment* src) {
    for (auto it = point_segment_endpoint_root_map.begin(); it != point_segment_endpoint_root_map.end(); ++it) {
        Point* p = it->first;
        auto &segment_map = it->second;
        if (segment_map.contains(src)) {
            auto pair = segment_map.extract(src);
            pair.key() = dest;
            segment_map.insert(std::move(pair));
        }
    }
    for (auto it = length_segment_root_map.begin(); it != length_segment_root_map.end(); ++it) {
        Length* len = it->first;
        auto &segment_map = it->second;
        if (segment_map.contains(src)) {
            auto pair = segment_map.extract(src);
            pair.key() = dest;
            segment_map.insert(std::move(pair));
        }
    }
}
void TracebackEngine::record_merge(Triangle* dest, Triangle* src) {
    for (auto it = point_triangle_vertex_root_map.begin(); it != point_triangle_vertex_root_map.end(); ++it) {
        Point* p = it->first;
        auto &triangle_map = it->second;
        if (triangle_map.contains(src)) {
            auto pair = triangle_map.extract(src);
            pair.key() = dest;
            triangle_map.insert(std::move(pair));
        }
    }
    for (auto it = dimension_triangle_root_map.begin(); it != dimension_triangle_root_map.end(); ++it) {
        Dimension* dim = it->first;
        auto &triangle_map = it->second;
        if (triangle_map.contains(src)) {
            auto pair = triangle_map.extract(src);
            pair.key() = dest;
            triangle_map.insert(std::move(pair));
        }
    }
}



void TracebackEngine::record_merge(Direction* dest, Direction* src) {
    // Skip the perpendicular direction stuff for now, TODO later

    direction_line_root_map[dest].merge(direction_line_root_map[src]);
    direction_line_root_map.erase(src);

    // Do we need to record direction_of_angles as well?
}


void TracebackEngine::set_point_on(Point* p, Line* l, Predicate* pred) {
    point_on_lines[p][l] = pred;
    point_line_root_map[p][l] = {p, l};
}
PredSet TracebackEngine::why_on(Point* p, Line* l) {
    auto [pc, lc] = point_line_root_map[p][l];
    PredSet res(point_on_lines[pc][lc]);
    res += TracebackUtils::why_ancestor(pc, p);
    res += TracebackUtils::why_ancestor(lc, l);
    return res;
}
void TracebackEngine::set_point_on(Point* p, Circle* c, Predicate* pred) {
    point_on_circles[p][c] = pred;
    point_circle_root_map[p][c] = {p, c};
}
PredSet TracebackEngine::why_on(Point* p, Circle* c) {
    auto [pc, cc] = point_circle_root_map[p][c];
    PredSet res(point_on_circles[pc][cc]);
    res += TracebackUtils::why_ancestor(pc, p);
    res += TracebackUtils::why_ancestor(cc, c);
    return res;
}
void TracebackEngine::set_point_as_center(Point* p, Circle* c, Predicate* pred) {
    point_as_circle_center[p][c] = pred;
    point_circle_center_root_map[p][c] = {p, c};
}
PredSet TracebackEngine::why_center(Point* p, Circle* c) {
    auto [pc, cc] = point_circle_center_root_map[p][c];
    PredSet res(point_as_circle_center[pc][cc]);
    res += TracebackUtils::why_ancestor(pc, p);
    res += TracebackUtils::why_ancestor(cc, c);
    return res;
}
void TracebackEngine::set_point_as_endpoint(Point* p, Segment* s, Predicate* pred) {
    point_as_segment_endpoint[p][s] = pred;
    point_segment_endpoint_root_map[p][s] = {p, s};
}
PredSet TracebackEngine::why_endpoint(Point* p, Segment* s) {
    auto [pc, sc] = point_segment_endpoint_root_map[p][s];
    PredSet res(point_as_segment_endpoint[pc][sc]);
    res += TracebackUtils::why_ancestor(pc, p);
    res += TracebackUtils::why_ancestor(sc, s);
    return res;
}
void TracebackEngine::set_point_as_vertex(Point* p, Triangle* t, Predicate* pred) {
    point_as_triangle_vertex[p][t] = pred;
    point_triangle_vertex_root_map[p][t] = {p, t};
}
PredSet TracebackEngine::why_vertex(Point* p, Triangle* t) {
    auto [pc, tc] = point_triangle_vertex_root_map[p][t];
    PredSet res(point_as_triangle_vertex[pc][tc]);
    res += TracebackUtils::why_ancestor(pc, p);
    res += TracebackUtils::why_ancestor(tc, t);
    return res;
}



void TracebackEngine::set_directions_perp(Direction* d1, Direction* d2, Predicate* pred) {
    perp_directions[{d1, d2}] = pred;
    perp_directions[{d2, d1}] = pred;
}
void TracebackEngine::set_direction_of(Direction* d, Line* l, Predicate* pred) {
    direction_of_lines[d][l] = pred;
    direction_line_root_map[d][l] = {d, l};
}
void TracebackEngine::set_length_of(Length* len, Segment* s, Predicate* pred) {
    length_of_segments[len][s] = pred;
    length_segment_root_map[len][s] = {len, s};
}
void TracebackEngine::set_dimension_of(Dimension* dim, Triangle* t, Predicate* pred) {
    dimension_of_triangles[dim][t] = pred;
    dimension_triangle_root_map[dim][t] = {dim, t};
}
void TracebackEngine::set_measure_of(Measure* m, Angle* a, Predicate* pred) {
    measure_of_angles[m][a] = pred;
    measure_angle_root_map[m][a] = {m, a};
}
void TracebackEngine::set_fraction_of(Fraction* f, Ratio* r, Predicate* pred) {
    fraction_of_ratios[f][r] = pred;
    fraction_ratio_root_map[f][r] = {f, r};
}
void TracebackEngine::set_shape_of(Shape* s, Dimension* d, Predicate* pred) {
    shape_of_dimensions[s][d] = pred;
    shape_dimension_root_map[s][d] = {s, d};
}
void TracebackEngine::set_measure_val(Measure* m, Frac val, Predicate* pred) {
    measure_vals[m] = {val, pred};
}
void TracebackEngine::set_fraction_val(Fraction* f, Frac val, Predicate* pred) {
    fraction_vals[f] = {val, pred};
}

void TracebackEngine::set_goal(Predicate* pred) {
    goal = pred;
}






PredSet TracebackEngine::why_coll(Point* p1, Point* p2, Point* p3) {
    PredSet res;

    const std::array<Point*, 3> ps{p1, p2, p3};

    /* Step 1: Extract all children of p1, p2 and p3 */
    std::array<std::set<Point*>, 3> pcs;
    int i = 0;
    for (Point* p : ps) {
        NodeUtils::all_children(p, pcs[i]);
        i++;
    }

    /* Step 2: For each child point, extract all lines it was placed on. This is
    stored in `l2ps`. We also store the root versions of these lines in `rls`. */
    std::array<std::map<Line*, Point*>, 3> l2ps;
    std::array<std::set<Line*>, 3> rls;
    for (i = 0; i < 3; i++) {
        for (Point* p : pcs[i]) {
            for (auto [l, pred] : point_on_lines[p]) {
                l2ps[i].emplace(l, p);
                rls[i].insert(NodeUtils::get_root(l));
            }
        }
    }

    /* Step 3: Identify a common root line to all three `rls` sets. */
    Line* common_root = nullptr;
    for (Line* rl : rls[0]) {
        if (rls[1].contains(rl) && rls[2].contains(rl)) {
            common_root = rl;
            break;
        }
    }
    if (!(common_root)) {
        throw TracebackInternalError("TracebackEngine::why_coll(): No common line found");
    }

    /* Step 4: Keep those lines in `l2ps` with root `common_root`. */
    for (i = 0; i < 3; i++) {
        for (auto it = l2ps[i].begin(); it != l2ps[i].end(); ) {
            if (!NodeUtils::same_as(it->first, common_root)) {
                it = l2ps[i].erase(it);
            } else {
                ++it;
            }
        }
    }

    /* Step 5: Iterate over all triplets of lines in the `l2ps` of `p1, p2, p3`. 
    Each line `li` will have a corresponding child point `pci` of `pi`. Identify
    the lowest common parent of the `li`s, then calculate the cumulative distance
    from the three line nodes to this common parent node. Add to that the distances
    from each `pci` node to the `pi` node.
    Pick the triplet with the lowest predicate count. */
    int min = 1e9; Line* lca = nullptr; 
    std::array<Point*, 3> pts; std::array<Line*, 3> lines;
    for (auto [l1, pc1] : l2ps[0]) {
        for (auto [l2, pc2] : l2ps[1]) {
            for (auto [l3, pc3] : l2ps[2]) {
                PredSet res_{point_on_lines[pc1][l1], point_on_lines[pc2][l2], point_on_lines[pc3][l3]};

                std::pair<Line*, int> lca_p = TracebackUtils::lowest_common_ancestor<Line>(l1, l2, l3);
                Line* lca_ = lca_p.first;
                
                // Figure out the whys
                std::vector<std::pair<Point*, Point*>> pc2ps{{
                    {pc1, p1}, {pc2, p2}, {pc3, p3}
                }};
                for (auto [pci, pi] : pc2ps) {
                    res_ += TracebackUtils::why_ancestor(pci, pi);
                }
                
                for (Line* li : {l1, l2, l3}) {
                    res_ += TracebackUtils::why_ancestor(li, lca_);
                }

                int sz = res_.size();
                if (sz < min) {
                    min = sz;
                    lca = lca_;
                    pts = {pc1, pc2, pc3};
                    lines = {l1, l2, l3};
                    res = std::move(res_);
                }
            }
        }
    }

    std::cout << "Analysing why_coll(" << p1->to_string() << ", " << p2->to_string() << ", " << p3->to_string() 
        << "):" << std::endl;
    std::cout << "Identified LCP " << lca->to_string() << " for lines "
        << lines[0]->to_string() << ", " << lines[1]->to_string() << ", " << lines[2]->to_string() << std::endl;
    std::cout << "The relevant points are " << pts[0]->to_string() << ", " << pts[1]->to_string() << ", "
        << pts[2]->to_string() << std::endl;
    
    return res;
}

