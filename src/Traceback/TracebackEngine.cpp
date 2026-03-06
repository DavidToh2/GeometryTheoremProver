
#include <iostream>

#include "TracebackEngine.hh"
#include "Common/Exceptions.hh"


void TracebackEngine::set_point_on(Point* p, Line* l, Predicate* pred) {
    point_on_lines[p][l] = pred;
}
void TracebackEngine::set_point_on(Point* p, Circle* c, Predicate* pred) {
    point_on_circles[p][c] = pred;
}
void TracebackEngine::set_point_as_center(Point* p, Circle* c, Predicate* pred) {
    point_as_circle_center[p][c] = pred;
}
void TracebackEngine::set_point_as_endpoint(Point* p, Segment* s, Predicate* pred) {
    point_as_segment_endpoint[p][s] = pred;
}
void TracebackEngine::set_point_as_vertex(Point* p, Triangle* t, Predicate* pred) {
    point_as_triangle_vertex[p][t] = pred;
}
void TracebackEngine::set_directions_perp(Direction* d1, Direction* d2, Predicate* pred) {
    perp_directions[{d1, d2}] = pred;
    perp_directions[{d2, d1}] = pred;
}
void TracebackEngine::set_direction_of(Direction* d, Line* l, Predicate* pred) {
    direction_of_lines[d][l] = pred;
}
void TracebackEngine::set_length_of(Length* len, Segment* s, Predicate* pred) {
    length_of_segments[len][s] = pred;
}
void TracebackEngine::set_dimension_of(Dimension* dim, Triangle* t, Predicate* pred) {
    dimension_of_triangles[dim][t] = pred;
}
void TracebackEngine::set_measure_of(Measure* m, Angle* a, Predicate* pred) {
    measure_of_angles[m][a] = pred;
}
void TracebackEngine::set_fraction_of(Fraction* f, Ratio* r, Predicate* pred) {
    fraction_of_ratios[f][r] = pred;
}
void TracebackEngine::set_shape_of(Shape* s, Dimension* d, Predicate* pred) {
    shape_of_dimensions[s][d] = pred;
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
    Pick the triplet with the lowest distance sum. */
    int min = 1e9; Line* lca = nullptr; 
    std::array<Point*, 3> pts; std::array<Line*, 3> lines;
    for (auto [l1, pc1] : l2ps[0]) {
        for (auto [l2, pc2] : l2ps[1]) {
            for (auto [l3, pc3] : l2ps[2]) {
                std::pair<Line*, int> lca_ = TracebackUtils::lowest_common_ancestor(l1, l2, l3);
                int counter = lca_.second;
                std::vector<std::pair<Point*, Point*>> pc2ps{{
                    {pc1, p1}, {pc2, p2}, {pc3, p3}
                }};
                for (auto [pci, pi] : pc2ps) {
                    Point* pt = pci;
                    while (pt != pi) {
                        counter++;
                        pt = NodeUtils::get_parent(pt);
                    }
                }
                if (counter < min) {
                    min = counter;
                    lca = lca_.first;
                    pts = {pc1, pc2, pc3};
                    lines = {l1, l2, l3};
                }
            }
        }
    }

    std::cout << "Identified LCP " << lca->to_string() << " for lines "
        << lines[0]->to_string() << ", " << lines[1]->to_string() << ", " << lines[2]->to_string() << std::endl;
    std::cout << "The relevant points are " << pts[0]->to_string() << ", " << pts[1]->to_string() << ", "
        << pts[2]->to_string() << std::endl;

    /* Step 6: Extract all relevant why's */
    for (i = 0; i < 3; i++) {
        res += point_on_lines[pts[i]][lines[i]];
        while (lines[i] != lca) {
            res += lines[i]->parent_why;
            lines[i] = NodeUtils::get_parent(lines[i]);
        }
        while (pts[i] != ps[i]) {
            res += pts[i]->parent_why;
            pts[i] = NodeUtils::get_parent(pts[i]);
        }
    }
    
    return res;
}

