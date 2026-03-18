
#include <cassert>
#include <iostream>

#include "TracebackEngine.hh"
#include "Common/Exceptions.hh"
#include "Geometry/Value.hh"


void TracebackEngine::record_merge(Point* dest, Point* src) {
    for (auto [l, _] : point_line_root_map[src]) {
        if (l->is_root() && !point_line_root_map[dest].contains(l)) {
            point_line_root_map[dest][l] = point_line_root_map[src][l];
        }
    }
    for (auto [c, _] : point_circle_root_map[src]) {
        if (c->is_root() && !point_circle_root_map[dest].contains(c)) {
            point_circle_root_map[dest][c] = point_circle_root_map[src][c];
        }
    }
    for (auto [c, _] : point_circle_center_root_map[src]) {
        if (c->is_root() && !point_circle_center_root_map[dest].contains(c)) {
            point_circle_center_root_map[dest][c] = point_circle_center_root_map[src][c];
        }
    }
    for (auto [s, _] : point_segment_endpoint_root_map[src]) {
        if (s->is_root() && !point_segment_endpoint_root_map[dest].contains(s)) {
            point_segment_endpoint_root_map[dest][s] = point_segment_endpoint_root_map[src][s];
        }
    }
    for (auto [t, _] : point_triangle_vertex_root_map[src]) {
        if (t->is_root() && !point_triangle_vertex_root_map[dest].contains(t)) {
            point_triangle_vertex_root_map[dest][t] = point_triangle_vertex_root_map[src][t];
        }
    }
}
void TracebackEngine::record_merge(Line* dest, Line* src) {
    for (auto [p, _] : point_line_root_map) {
        if (p->is_root()) {
            if (point_line_root_map[p].contains(src) && !point_line_root_map[p].contains(dest)) {
                point_line_root_map[p][dest] = point_line_root_map[p][src];
            }
        }
    }
    for (auto [d, _] : direction_line_root_map) {
        if (d->is_root()) {
            if (direction_line_root_map[d].contains(src) && !direction_line_root_map[d].contains(dest)) {
                direction_line_root_map[d][dest] = direction_line_root_map[d][src];
            }
        }
    }
}
void TracebackEngine::record_merge(Circle* dest, Circle* src) {
    for (auto [p, _] : point_circle_root_map) {
        if (p->is_root()) {
            if (point_circle_root_map[p].contains(src) && !point_circle_root_map[p].contains(dest)) {
                point_circle_root_map[p][dest] = point_circle_root_map[p][src];
            }
        }
    }
    for (auto [p, _] : point_circle_center_root_map) {
        if (p->is_root()) {
            if (point_circle_center_root_map[p].contains(src) && !point_circle_center_root_map[p].contains(dest)) {
                point_circle_center_root_map[p][dest] = point_circle_center_root_map[p][src];
            }
        }
    }
}
void TracebackEngine::record_merge(Segment* dest, Segment* src) {
    for (auto [p, _] : point_segment_endpoint_root_map) {
        if (p->is_root()) {
            if (point_segment_endpoint_root_map[p].contains(src) && !point_segment_endpoint_root_map[p].contains(dest)) {
                point_segment_endpoint_root_map[p][dest] = point_segment_endpoint_root_map[p][src];
            }
        }
    }
    for (auto [l, _] : length_segment_root_map) {
        if (l->is_root()) {
            if (length_segment_root_map[l].contains(src) && !length_segment_root_map[l].contains(dest)) {
                length_segment_root_map[l][dest] = length_segment_root_map[l][src];
            }
        }
    }
}
void TracebackEngine::record_merge(Triangle* dest, Triangle* src) {
    for (auto [p, _] : point_triangle_vertex_root_map) {
        if (p->is_root()) {
            if (point_triangle_vertex_root_map[p].contains(src) && !point_triangle_vertex_root_map[p].contains(dest)) {
                point_triangle_vertex_root_map[p][dest] = point_triangle_vertex_root_map[p][src];
            }
        }
    }
    for (auto [d, _] : dimension_triangle_root_map) {
        if (d->is_root()) {
            if (dimension_triangle_root_map[d].contains(src) && !dimension_triangle_root_map[d].contains(dest)) {
                dimension_triangle_root_map[d][dest] = dimension_triangle_root_map[d][src];
            }
        }
    }
}



void TracebackEngine::record_merge(Direction* dest, Direction* src) {
    for (auto [l, _] : direction_line_root_map[src]) {
        if (l->is_root() && !direction_line_root_map[dest].contains(l)) {
            direction_line_root_map[dest][l] = direction_line_root_map[src][l];
        }
    }
    // No need to manually record perps since GeometricGraph::set_directions_para() takes care of that
    
    for (auto [a, _] : direction_angle_root_map) {
        if (direction_angle_root_map[a].contains(src) && !direction_angle_root_map[a].contains(dest)) {
            direction_angle_root_map[a][dest] = direction_angle_root_map[a][src];
        }
    }
}
void TracebackEngine::record_merge(Length* dest, Length* src) {
    for (auto [s, _] : length_segment_root_map[src]) {
        if (s->is_root() && !length_segment_root_map[dest].contains(s)) {
            length_segment_root_map[dest][s] = length_segment_root_map[src][s];
        }
    }

    for (auto [r, _] : length_ratio_root_map) {
        if (length_ratio_root_map[r].contains(src) && !length_ratio_root_map[r].contains(dest)) {
            length_ratio_root_map[r][dest] = length_ratio_root_map[r][src];
        }
    }
}
void TracebackEngine::record_merge(Dimension* dest, Dimension* src) {
    for (auto [t, _] : dimension_triangle_root_map[src]) {
        if (t->is_root() && !dimension_triangle_root_map[dest].contains(t)) {
            dimension_triangle_root_map[dest][t] = dimension_triangle_root_map[src][t];
        }
    }
}



void TracebackEngine::record_merge(Angle* dest, Angle* src) {
    for (auto [d, _] : direction_angle_root_map[src]) {
        if (d->is_root() && !direction_angle_root_map[dest].contains(d)) {
            direction_angle_root_map[dest][d] = direction_angle_root_map[src][d];
        }
    }
}
void TracebackEngine::record_merge(Ratio* dest, Ratio* src) {
    for (auto [l, _] : length_ratio_root_map[src]) {
        if (l->is_root() && !length_ratio_root_map[dest].contains(l)) {
            length_ratio_root_map[dest][l] = length_ratio_root_map[src][l];
        }
    }
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
PredSet TracebackEngine::why_directions_perp(Direction* d1, Direction* d2) {
    PredSet res(perp_directions[{d1, d2}]);
    res += TracebackUtils::why_ancestor(d1, d1);
    res += TracebackUtils::why_ancestor(d2, d2);
    return res;
}
void TracebackEngine::set_direction_of(Direction* d, Line* l, Predicate* pred) {
    direction_of_lines[d][l] = pred;
    direction_line_root_map[d][l] = {d, l};
}
PredSet TracebackEngine::why_direction_of(Direction* d, Line* l) {
    auto [dc, lc] = direction_line_root_map[d][l];
    std::cout << d->to_string() << ", " << l->to_string() << " map to " << dc->to_string() << ", " << lc->to_string() << std::endl;
    PredSet res(direction_of_lines[dc][lc]);
    res += TracebackUtils::why_ancestor(dc, d);
    res += TracebackUtils::why_ancestor(lc, l);
    return res;
}
Direction* TracebackEngine::__earliest_direction_of(Line* l) {
    Direction* earliest = nullptr;
    for (auto [d, _] : direction_line_root_map) {
        if (direction_line_root_map[d].contains(l)) {
            if (!earliest || NodeUtils::ancestor_of(earliest, d)) {
                earliest = d;
            }
        }
    }
    return earliest;
}
void TracebackEngine::set_length_of(Length* len, Segment* s, Predicate* pred) {
    length_of_segments[len][s] = pred;
    length_segment_root_map[len][s] = {len, s};
}
void TracebackEngine::set_dimension_of(Dimension* dim, Triangle* t, Predicate* pred) {
    dimension_of_triangles[dim][t] = pred;
    dimension_triangle_root_map[dim][t] = {dim, t};
}



PredSet TracebackEngine::why_direction_of(Direction* d, Angle* a) {
    auto [ac, dc] = direction_angle_root_map[a][d];
    return TracebackUtils::why_ancestor(dc, d) + TracebackUtils::why_ancestor(ac, a);
}
PredSet TracebackEngine::why_length_of(Length* len, Ratio* r) {
    auto [rc, lc] = length_ratio_root_map[r][len];
    return TracebackUtils::why_ancestor(lc, len) + TracebackUtils::why_ancestor(rc, r);
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
    std::map<std::pair<Point*, Point*>, PredSet> why_point_ancestor_cache;
    std::map<std::pair<Line*, Line*>, PredSet> why_line_ancestor_cache;

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
    std::array<std::vector<std::pair<Line*, Point*>>, 3> l2ps;
    std::array<std::set<Line*>, 3> rls;
    for (i = 0; i < 3; i++) {
        for (Point* p : pcs[i]) {
            for (auto [l, pred] : point_on_lines[p]) {
                l2ps[i].emplace_back(l, p);
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
    Each line `li` will have a corresponding child point `pci` of `pi`. Identify the 
    lowest common ancestor `lca` of the `li`s.
    The desired PredSet is the union of the following:
    - `point_on_lines[pci][li]` for each `i`
    - `why_ancestor(li, lca)` for each `i`
    - `why_ancestor(pci, pi)` for each `i`
    Pick the triplet with the lowest predicate count. */
    int min = 1e9; Line* lca = nullptr; 
    std::array<Point*, 3> pts; std::array<Line*, 3> lines;
    for (auto [l1, pc1] : l2ps[0]) {
        for (auto [l2, pc2] : l2ps[1]) {
            for (auto [l3, pc3] : l2ps[2]) {
                PredSet res_{point_on_lines[pc1][l1], point_on_lines[pc2][l2], point_on_lines[pc3][l3]};

                std::pair<Line*, int> lca_p = TracebackUtils::lowest_common_ancestor<Line>(l1, l2, l3);
                Line* lca_ = lca_p.first;

                // std::cout << "Points " << pc1->to_string() << ", " << pc2->to_string() << ", " << pc3->to_string() 
                //     << " lie on lines " << l1->to_string() << ", " << l2->to_string() << ", " << l3->to_string()
                //     << " with LCA " << lca_->to_string() << " and pre-res_ " << res_.to_string() << std::endl;
                
                // Figure out the whys
                std::vector<std::pair<Point*, Point*>> pc2ps{{
                    {pc1, p1}, {pc2, p2}, {pc3, p3}
                }};
                for (auto [pci, pi] : pc2ps) {
                    res_ += TracebackUtils::why_ancestor_with_cache(pci, pi, why_point_ancestor_cache);
                }
                
                for (Line* li : {l1, l2, l3}) {
                    res_ += TracebackUtils::why_ancestor_with_cache(li, lca_, why_line_ancestor_cache);
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

    /* std::cout << "Analysing why_coll(" << p1->to_string() << ", " << p2->to_string() << ", " << p3->to_string() 
        << "):" << std::endl;
    std::cout << "Identified LCP " << lca->to_string() << " for lines "
        << lines[0]->to_string() << ", " << lines[1]->to_string() << ", " << lines[2]->to_string() << std::endl;
    std::cout << "The relevant points are " << pts[0]->to_string() << ", " << pts[1]->to_string() << ", "
        << pts[2]->to_string() << std::endl; */
    
    return res;
}


PredSet TracebackEngine::why_cyclic(Point* p1, Point* p2, Point* p3, Point* p4) {
    PredSet res;
    std::map<std::pair<Point*, Point*>, PredSet> why_point_ancestor_cache;
    std::map<std::pair<Circle*, Circle*>, PredSet> why_circle_ancestor_cache;

    const std::array<Point*, 4> ps{p1, p2, p3, p4};

    /* Step 1: Extract all children of p1, p2, p3, p4 */ 
    std::array<std::set<Point*>, 4> pcs;
    int i = 0;
    for (Point* p : ps) {
        NodeUtils::all_children(p, pcs[i]);
        i++;
    }

    /* Step 2: For each child point, extract all circles it was placed on */
    std::array<std::vector<std::pair<Circle*, Point*>>, 4> c2ps;
    std::array<std::set<Circle*>, 4> rcs;
    for (i = 0; i < 4; i++) {
        for (Point* p : pcs[i]) {
            for (auto [c, pred] : point_on_circles[p]) {
                c2ps[i].emplace_back(c, p);
                rcs[i].insert(NodeUtils::get_root(c));
            }
        }
    }

    /* Step 3: Identify a common root circle to all four `rcs` sets */
    Circle* common_root = nullptr;
    for (Circle* rc : rcs[0]) {
        if (rcs[1].contains(rc) && rcs[2].contains(rc) && rcs[3].contains(rc)) {
            common_root = rc;
            break;
        }
    }
    if (!(common_root)) {
        throw TracebackInternalError("TracebackEngine::why_cyclic(): No common circle found");
    }

    /* Step 4: Keep those circles in `c2ps` with root `common_root` */
    for (i = 0; i < 4; i++) {
        for (auto it = c2ps[i].begin(); it != c2ps[i].end(); ) {
            if (!NodeUtils::same_as(it->first, common_root)) {
                it = c2ps[i].erase(it);
            } else {
                ++it;
            }
        }
    }

    /* Step 5: Iterate over all quadruplets of circles in the `l2ps` of `p1, p2, p3, p4`. 
    Each circle `ci` will have a corresponding child point `pci` of `pi`. Identify the 
    lowest common ancestor `lca` of the `ci`s.
    The desired PredSet is the union of the following:
    - `point_on_circles[pci][ci]` for each `i`
    - `why_ancestor(ci, lca)` for each `i`
    - `why_ancestor(pci, pi)` for each `i`
    Pick the quadruplet with the lowest predicate count. */
    int min = 1e9; Circle* lca = nullptr;
    std::array<Point*, 4> pts; std::array<Circle*, 4> circles;
    for (auto [c1, pc1] : c2ps[0]) {
        for (auto [c2, pc2] : c2ps[1]) {
            for (auto [c3, pc3] : c2ps[2]) {
                for (auto [c4, pc4] : c2ps[3]) {
                    PredSet res_{
                        point_on_circles[pc1][c1], point_on_circles[pc2][c2], 
                        point_on_circles[pc3][c3], point_on_circles[pc4][c4]
                    };

                    std::pair<Circle*, int> lca_p = TracebackUtils::lowest_common_ancestor(c1, c2, c3, c4);
                    Circle* lca_ = lca_p.first;

                    // Figure out the whys
                    std::vector<std::pair<Point*, Point*>> pc2ps{{
                        {pc1, p1}, {pc2, p2}, {pc3, p3}, {pc4, p4}
                    }};
                    for (auto [pci, pi] : pc2ps) {
                        res_ += TracebackUtils::why_ancestor_with_cache(pci, pi, why_point_ancestor_cache);
                    }
                    for (Circle* ci : {c1, c2, c3, c4}) {
                        res_ += TracebackUtils::why_ancestor_with_cache(ci, lca_, why_circle_ancestor_cache);
                    }

                    int sz = res_.size();
                    if (sz < min) {
                        min = sz;
                        lca = lca_;
                        pts = {pc1, pc2, pc3, pc4};
                        circles = {c1, c2, c3, c4};
                        res = std::move(res_);
                    }
                }
            }
        }
    }
    return res;
}



PredSet TracebackEngine::why_circle(Point* c, Point* p1, Point* p2, Point* p3) {
    PredSet res;
    std::map<std::pair<Point*, Point*>, PredSet> why_point_ancestor_cache;
    std::map<std::pair<Circle*, Circle*>, PredSet> why_circle_ancestor_cache;

    const std::array<Point*, 3> ps{p1, p2, p3};

    /* Step 1: Extract all children of c, p1, p2 and p3 */
    std::set<Point*> ccs; 
    std::array<std::set<Point*>, 3> pcs;
    NodeUtils::all_children(c, ccs);
    int i = 0;
    for (Point* p : ps) {
        NodeUtils::all_children(p, pcs[i]);
        i++;
    }

    /* Step 2: Extract all circles for which a child of c was set as center */
    std::vector<std::pair<Circle*, Point*>> circ2c;
    std::set<Circle*> root_circ_cs;
    for (Point* cc : ccs) {
        for (auto [circ, pred] : point_as_circle_center[cc]) {
            circ2c.emplace_back(circ, cc);
            root_circ_cs.insert(NodeUtils::get_root(circ));
        }
    }

    /* Step 3: Keep only the root circle that contains all of p1, p2 and p3
    (when they were still roots) */
    Circle* common_root = nullptr;
    for (Circle* rc : root_circ_cs) {
        if (rc->contains(p1) && rc->contains(p2) && rc->contains(p3)) {
            common_root = rc;
            break;
        }
    }
    if (!(common_root)) {
        throw TracebackInternalError("TracebackEngine::why_circle(): No common circle found");
    }

    /* Step 4: Keep those circles in circ2c with root common_root */
    for (auto it = circ2c.begin(); it != circ2c.end(); ) {
        if (!NodeUtils::same_as(it->first, common_root)) {
            it = circ2c.erase(it);
        } else {
            ++it;
        }
    }

    /* Step 5: For each pi, extract all children of common_root which it was
    placed on */
    std::array<std::map<Circle*, Point*>, 3> p2cs;
    for (i = 0; i < 3; i++) {
        for (Point* p : pcs[i]) {
            for (auto [c, pred] : point_on_circles[p]) {
                if (NodeUtils::same_as(NodeUtils::get_root(c), common_root)) {
                    p2cs[i].emplace(c, p);
                }
            }
        }
    }

    /* Step 6: Iterate over all triplets of circles in the `p2cs` of `p1, p2, p3`, as
    well as all circles `c0` in `circ2c`.
    Each circle `ci` will have a corresponding child point `pci` of `pi`, and each `c0`
    has a corresponding child point `cc` of `c`. Identify the lowest common ancestor 
    `lca` of the `ci`s.
    The desired PredSet is the union of the following:
    - `point_on_circles[pci][ci]` for each `i`
    - `why_center(cc, c0)`
    - `why_ancestor(ci, lca)` for each `i`
    - `why_ancestor(pci, pi)` for each `i`
    - `why_ancestor(cc, c)`
    Pick the triplet with the lowest predicate count. */
    int min = 1e9;
    for (auto [c0, cc] : circ2c) {
        for (auto [c1, pc1] : p2cs[0]) {
            for (auto [c2, pc2] : p2cs[1]) {
                for (auto [c3, pc3] : p2cs[2]) {
                    PredSet res_{
                        point_on_circles[pc1][c1], point_on_circles[pc2][c2], point_on_circles[pc3][c3],
                        point_as_circle_center[cc][c0]
                    };

                    std::pair<Circle*, int> lca_p = TracebackUtils::lowest_common_ancestor(c0, c1, c2, c3);
                    Circle* lca_ = lca_p.first;

                    // Figure out the whys
                    std::vector<std::pair<Point*, Point*>> pc2ps{{
                        {pc1, p1}, {pc2, p2}, {pc3, p3}
                    }};
                    for (auto [pci, pi] : pc2ps) {
                        res_ += TracebackUtils::why_ancestor_with_cache(pci, pi, why_point_ancestor_cache);
                    }
                    res_ += TracebackUtils::why_ancestor(cc, c);
                    for (Circle* ci : {c0, c1, c2, c3}) {
                        res_ += TracebackUtils::why_ancestor_with_cache(ci, lca_, why_circle_ancestor_cache);
                    }

                    int sz = res_.size();
                    if (sz < min) {
                        min = sz;
                        res = std::move(res_);
                    }
                }
            }
        }
    }

    return res;
}



PredSet TracebackEngine::why_para(Point* p1, Point* p2, Point* p3, Point* p4) {
    PredSet res;
    std::map<std::pair<Point*, Point*>, PredSet> why_point_ancestor_cache;
    std::map<std::pair<Line*, Line*>, PredSet> why_line_ancestor_cache;
    std::map<std::pair<Direction*, Direction*>, PredSet> why_direction_ancestor_cache;

    std::cout << "---- why_para " << p1->to_string() << " " << p2->to_string() << " " << p3->to_string() << " " << p4->to_string() << std::endl;

    /* Step 1: Extract all children of p1, p2, p3 and p4 */
    std::array<std::set<Point*>, 4> pcs;
    int i = 0;
    for (Point* p : std::array<Point*, 4>{p1, p2, p3, p4}) {
        NodeUtils::all_children(p, pcs[i]);
        i++;
    }

    /* Step 2: For each child point, extract all lines it was placed on.
    It suffices to consider lines whose roots have Directions */
    std::array<std::vector<std::pair<Line*, Point*>>, 4> l2ps;
    std::array<std::set<Line*>, 4> rls;
    for (i = 0; i < 4; i++) {
        for (Point* p : pcs[i]) {
            for (auto [l, pred] : point_on_lines[p]) {
                Line* rl = NodeUtils::get_root(l);
                if (rl->has_direction()) {
                    l2ps[i].emplace_back(l, p);
                    rls[i].insert(rl);
                }
            }
        }
    }

    /* Step 3: Find a pair of common_roots, corresponding to `p1p2` and `p3p4`, that
    have the same Direction (which would also be a root) */
    std::map<Direction*, std::vector<Line*>> root_dir_to_root_lines;
    Line* common_root_12 = nullptr; Line* common_root_34 = nullptr; Direction* common_root_direction = nullptr;
    for (Line* rl12 : rls[0]) {
        if (rls[1].contains(rl12)) {
            root_dir_to_root_lines[rl12->get_direction()].emplace_back(rl12);
        }
    }
    for (Line* rl34 : rls[2]) {
        if (rls[3].contains(rl34)) {
            if (root_dir_to_root_lines.contains(rl34->get_direction())) {
                common_root_12 = root_dir_to_root_lines[rl34->get_direction()][0];
                common_root_34 = rl34;
                common_root_direction = rl34->get_direction();
                break;
            }
        }
    }
    if (!(common_root_direction)) {
        throw TracebackInternalError("TracebackEngine::why_para(): No common direction found");
    }
    root_dir_to_root_lines.clear();

    /* Step 4: From these common_roots, extract the LCA lines containing `(p1, p2)` and 
    `(p3, p4)` respectively. These are stored in `lca1s` and `lca2s`. We only consider `lca`s
    which have directions.
    Note: There may be multiple candidates for `lca1` and `lca2`. This is because if there
    were formerly points `(q1, q2)` and `(r1, r2)`, and both `qi, ri` were merged into `pi`,
    it is not inconceivable that there might be some `lca1_q` containing `(q1, q2)` and some
    other `lca1_r` containing `(r1, r2)`. */
    for (i=0; i<2; i++) {
        for (auto it = l2ps[i].begin(); it != l2ps[i].end(); ) {
            if (!NodeUtils::same_as(it->first, common_root_12)) {
                it = l2ps[i].erase(it);
            } else {
                ++it;
            }
        }
    }
    std::map<Line*, PredSet> lca1s;
    for (auto [l_p1, cp1] : l2ps[0]) {
        for (auto [l_p2, cp2] : l2ps[1]) {
            auto lca1_ = TracebackUtils::lowest_common_ancestor(l_p1, l_p2);
            Line* lca1 = lca1_.first;
            PredSet preds_ = (
                TracebackUtils::why_ancestor_with_cache(cp1, p1, why_point_ancestor_cache)
                + TracebackUtils::why_ancestor_with_cache(cp2, p2, why_point_ancestor_cache)
                + TracebackUtils::why_ancestor_with_cache(l_p1, lca1, why_line_ancestor_cache)
                + TracebackUtils::why_ancestor_with_cache(l_p2, lca1, why_line_ancestor_cache)
                + point_on_lines[cp1][l_p1] + point_on_lines[cp2][l_p2]
            );
            if (!lca1s.contains(lca1) || preds_.size() < lca1s[lca1].size()) {
                lca1s[lca1] = std::move(preds_);
            }
        }
    }

    for (i=2; i<4; i++) {
        for (auto it = l2ps[i].begin(); it != l2ps[i].end(); ) {
            if (!NodeUtils::same_as(it->first, common_root_34)) {
                it = l2ps[i].erase(it);
            } else {
                ++it;
            }
        }
    }
    std::map<Line*, PredSet> lca2s;
    for (auto [l_p3, cp3] : l2ps[2]) {
        for (auto [l_p4, cp4] : l2ps[3]) {
            auto lca2_ = TracebackUtils::lowest_common_ancestor(l_p3, l_p4);
            Line* lca2 = lca2_.first;
            PredSet preds_ = (
                TracebackUtils::why_ancestor_with_cache(cp3, p3, why_point_ancestor_cache)
                + TracebackUtils::why_ancestor_with_cache(cp4, p4, why_point_ancestor_cache)
                + TracebackUtils::why_ancestor_with_cache(l_p3, lca2, why_line_ancestor_cache)
                + TracebackUtils::why_ancestor_with_cache(l_p4, lca2, why_line_ancestor_cache)
                + point_on_lines[cp3][l_p3] + point_on_lines[cp4][l_p4]
            );
            if (!lca2s.contains(lca2) || preds_.size() < lca2s[lca2].size()) {
                lca2s[lca2] = std::move(preds_);
            }
        }
    }

    /* Now we check every possible pair of LCAs `(lca1, lca2)` */
    int min = 1e9;
    for (const auto& [lca1, why_ancestor_lines_points_12] : lca1s) {
        for (const auto& [lca2, why_ancestor_lines_points_34] : lca2s) {

            PredSet res_ = why_ancestor_lines_points_12 + why_ancestor_lines_points_34;
            std::cout << lca1->to_string() << " and " << lca2->to_string() << ": " 
                << why_ancestor_lines_points_12.to_string() << " and " 
                << why_ancestor_lines_points_34.to_string() << " | ";

            /* Branch 1: If `lca1` and `lca2` are the same (call it `lca`) */
            if (lca1 == lca2) {
                Direction* d = __earliest_direction_of(lca1);
                assert(NodeUtils::same_as(d, common_root_direction));

                std::cout << d->to_string() << std::endl;

                /* Step 5: Extract all children of `lca` that were assigned directions */
                std::set<Line*> lca_cs;
                std::vector<std::pair<Direction*, Line*>> d2ls;
                NodeUtils::all_children(lca1, lca_cs);
                for (const auto& [dir, line_map] : direction_of_lines) {
                    for (Line* lca_c : lca_cs) {
                        if (line_map.contains(lca_c)) {
                            d2ls.emplace_back(dir, lca_c);
                        }
                    }
                }

                /* Step 6: Each child `lca_c` will have some direction `d_c` 
                The desired PredSet is the union of the following:
                - direction_of_lines[d_c][lca_c]
                - why_ancestor(lca_c, lca) 
                - why_ancestor(d_c, d)
                Pick the `lca_c` with the lowest predicate count */
                int min_ = 1e9;
                PredSet res_1;
                for (auto [d_c, lca_c] : d2ls) {
                    PredSet res_1_ = (
                        TracebackUtils::why_ancestor_with_cache(lca_c, lca1, why_line_ancestor_cache)
                        + TracebackUtils::why_ancestor_with_cache(d_c, d, why_direction_ancestor_cache)
                        + direction_of_lines[d_c][lca_c]
                    );
                    if (res_1_.size() < min_) {
                        min_ = res_1_.size();
                        res_1 = std::move(res_1_);
                    }
                }
                res_ += std::move(res_1);
            }
            
            /* Branch 2: If `lca1` and `lca2` have Directions `d1, d2` (these may be the same) */
            else {
                Direction* d1 = __earliest_direction_of(lca1), *d2 = __earliest_direction_of(lca2);
                assert(NodeUtils::same_as(d1, common_root_direction));
                assert(NodeUtils::same_as(d2, common_root_direction));

                std::cout << d1->to_string() << " and " << d2->to_string() << ", ";

                /* Step 5: Find ancestors `lca1_a` and `lca2_a` which were the same direction `d` at
                some point in time
                We should choose `d` to be the "least possible", i.e. the LCA of `d1` and `d2`
                (It could be the case that `lca1 == lca1_a` and `lca2 == lca2_a` and `d == d1 == d2`) */
                Line* lca1_a = lca1, *lca2_a = lca2;
                auto d_ = TracebackUtils::lowest_common_ancestor(d1, d2);
                Direction* d = d_.first;
                assert(NodeUtils::same_as(d, common_root_direction));
                
                if (!NodeUtils::ancestor_of(lca1->__get_direction(), d)) {
                    while (!(lca1_a->is_root())) {
                        lca1_a = NodeUtils::get_parent(lca1_a);
                        if (NodeUtils::ancestor_of(lca1_a->__get_direction(), d)) {
                            break;
                        }
                    }
                }
                if (!NodeUtils::ancestor_of(lca2->__get_direction(), d)) {
                    while (!(lca2_a->is_root())) {
                        lca2_a = NodeUtils::get_parent(lca2_a);
                        if (NodeUtils::ancestor_of(lca2_a->__get_direction(), d)) {
                            break;
                        }
                    }
                }

                /* Step 6: Extract all children of `lca1` and `lca2` which were assigned directions */
                std::array<Line*, 2> lcas{lca1, lca2};
                std::array<std::set<Line*>, 2> lca_cs;
                std::array<std::vector<std::pair<Direction*, Line*>>, 2> d2ls;

                for (i=0; i<2; i++) {
                    NodeUtils::all_children(lcas[i], lca_cs[i]);
                    for (const auto& [dir, line_map] : direction_of_lines) {
                        for (Line* lca_c : lca_cs[i]) {
                            if (line_map.contains(lca_c)) {
                                d2ls[i].emplace_back(dir, lca_c);
                            }
                        }
                    }
                }

                std::cout << "lca1_a: " << lca1_a->to_string() << ", lca2_a: " << lca2_a->to_string() << ", d: " << d->to_string() << std::endl;

                /* Step 7: Each child `lca_1` will have been assigned some Direction `d_1`.
                The desired PredSet is the union of the following:
                - direction_of_lines[d_1][lca_1]
                - why_ancestor(lca_1, lca1)
                - why_ancestor(d_1, d1)
                Pick the `lca_1` with the lowest predicate count. Do the same thing for `lca_2`. 
                Generate the union of these two PredSets. */
                int min_1 = 1e9, min_2 = 1e9;
                PredSet res_1, res_2;
                for (auto [d_1, lca_1] : d2ls[0]) {
                    PredSet res_1_ = (
                        TracebackUtils::why_ancestor_with_cache(lca_1, lca1, why_line_ancestor_cache)
                        + TracebackUtils::why_ancestor_with_cache(d_1, d1, why_direction_ancestor_cache)
                        + direction_of_lines[d_1][lca_1]
                    );
                    if (res_1_.size() < min_1) {
                        min_1 = res_1_.size();
                        res_1 = std::move(res_1_);
                    }
                }
                for (auto [d_2, lca_2] : d2ls[1]) {
                    PredSet res_2_ = (
                        TracebackUtils::why_ancestor_with_cache(lca_2, lca2, why_line_ancestor_cache)
                        + TracebackUtils::why_ancestor_with_cache(d_2, d2, why_direction_ancestor_cache)
                        + direction_of_lines[d_2][lca_2]
                    );
                    if (res_2_.size() < min_2) {
                        min_2 = res_2_.size();
                        res_2 = std::move(res_2_);
                    }
                }

                /* Step 8: Add to these PredSets the following:
                - why_ancestor(lcai, lcai_a) for each i
                - why_ancestor(di, d) for each i */
                res_ += (res_1 + res_2);
                res_ += (
                    TracebackUtils::why_ancestor_with_cache(lca1, lca1_a, why_line_ancestor_cache)
                    + TracebackUtils::why_ancestor_with_cache(lca2, lca2_a, why_line_ancestor_cache)
                    + TracebackUtils::why_ancestor_with_cache(d1, d, why_direction_ancestor_cache)
                    + TracebackUtils::why_ancestor_with_cache(d2, d, why_direction_ancestor_cache)
                );
            }

            if (res_.size() < min) {
                min = res_.size();
                res = std::move(res_);
            }
        }
    }

    return res;
}