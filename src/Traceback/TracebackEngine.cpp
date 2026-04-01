
#include <cassert>
#include <iostream>

#include "TracebackEngine.hh"
#include "Common/Exceptions.hh"
#include "Geometry/Object2.hh"
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

    // Record perp info:

    using MapNodeType = decltype(perp_directions_root_map)::node_type;
    std::vector<MapNodeType> extracted_nodes;

    for (auto it = perp_directions_root_map.begin(); it != perp_directions_root_map.end(); ) {
        if (it->first.first == src || it->first.second == src) {
            auto current_it = it++;
            auto node = perp_directions_root_map.extract(current_it);
            if (node.key().first == src) {
                node.key().first = dest;
            } else {
                node.key().second = dest;
            }
            extracted_nodes.emplace_back(std::move(node));
        } else {
            it++;
        }
    }

    for (auto& node : extracted_nodes) {
        auto __res = perp_directions_root_map.insert(std::move(node));
        if (!__res.inserted) {
            auto& dest_set = __res.position->second;
            auto& src_set = __res.node.mapped();
            dest_set.merge(src_set);
        }
    }

    // Record angle direction info:

    using MapNodeType = decltype(angle_directions_root_map)::node_type;
    std::vector<MapNodeType> extracted_angle_nodes;

    for (auto it = angle_directions_root_map.begin(); it != angle_directions_root_map.end(); ) {
        if (it->first.first == src || it->first.second == src) {
            auto current_it = it++;
            auto node = angle_directions_root_map.extract(current_it);
            if (node.key().first == src) {
                node.key().first = dest;
            } else {
                node.key().second = dest;
            }
            extracted_angle_nodes.emplace_back(std::move(node));
        } else {
            it++;
        }
    }

    for (auto& node : extracted_angle_nodes) {
        auto __res = angle_directions_root_map.insert(std::move(node));
        if (!__res.inserted) {
            auto& dest_set = __res.position->second;
            auto& src_set = __res.node.mapped();
            dest_set.merge(src_set);
        }
    }
}
void TracebackEngine::record_merge(Length* dest, Length* src) {
    for (auto [s, _] : length_segment_root_map[src]) {
        if (s->is_root() && !length_segment_root_map[dest].contains(s)) {
            length_segment_root_map[dest][s] = length_segment_root_map[src][s];
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
    for (auto [m, _] : measure_angle_root_map) {
        if (m->is_root()) {
            if (measure_angle_root_map[m].contains(src) && !measure_angle_root_map[m].contains(dest)) {
                measure_angle_root_map[m][dest] = measure_angle_root_map[m][src];
            }
        }
    }
}
void TracebackEngine::record_merge(Ratio* dest, Ratio* src) {
    for (auto [f, _] : fraction_ratio_root_map) {
        if (f->is_root()) {
            if (fraction_ratio_root_map[f].contains(src) && !fraction_ratio_root_map[f].contains(dest)) {
                fraction_ratio_root_map[f][dest] = fraction_ratio_root_map[f][src];
            }
        }
    }
}



void TracebackEngine::record_merge(Measure* dest, Measure* src) {
    for (auto [a, _] : measure_angle_root_map[src]) {
        if (a->is_root() && !measure_angle_root_map[dest].contains(a)) {
            measure_angle_root_map[dest][a] = measure_angle_root_map[src][a];
        }
    }
}
void TracebackEngine::record_merge(Fraction* dest, Fraction* src) {
    for (auto [r, _] : fraction_ratio_root_map[src]) {
        if (r->is_root() && !fraction_ratio_root_map[dest].contains(r)) {
            fraction_ratio_root_map[dest][r] = fraction_ratio_root_map[src][r];
        }
    }
}




void TracebackEngine::set_point_on(Point* p, Line* l, PredSet pred) {
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
void TracebackEngine::set_point_on(Point* p, Circle* c, PredSet pred) {
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
void TracebackEngine::set_point_as_center(Point* p, Circle* c, PredSet pred) {
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
void TracebackEngine::set_point_as_endpoint(Point* p, Segment* s, PredSet pred) {
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
void TracebackEngine::set_point_as_vertex(Point* p, Triangle* t, PredSet pred) {
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



void TracebackEngine::set_directions_perp(Direction* d1, Direction* d2, PredSet pred) {
    perp_directions[{d1, d2}] = pred;
    perp_directions[{d2, d1}] = pred;
    perp_directions_root_map[{d1, d2}].insert({d1, d2});
    perp_directions_root_map[{d2, d1}].insert({d2, d1});
}
PredSet TracebackEngine::why_directions_perp(Direction* d1, Direction* d2) {
    auto& s = perp_directions_root_map[{d1, d2}];
    auto [dc1, dc2] = *(s.begin());
    PredSet res(perp_directions[{dc1, dc2}]);
    res += TracebackUtils::why_ancestor(dc1, d1);
    res += TracebackUtils::why_ancestor(dc2, d2);
    return res;
}
void TracebackEngine::set_direction_of(Direction* d, Line* l, PredSet pred) {
    direction_of_lines[d][l] = pred;
    direction_line_root_map[d][l] = {d, l};
}
PredSet TracebackEngine::why_direction_of(Direction* d, Line* l) {
    auto [dc, lc] = direction_line_root_map[d][l];
    PredSet res(direction_of_lines[dc][lc]);
    res += TracebackUtils::why_ancestor(dc, d);
    res += TracebackUtils::why_ancestor(lc, l);
    return res;
}
void TracebackEngine::set_length_of(Length* len, Segment* s, PredSet pred) {
    length_of_segments[len][s] = pred;
    length_segment_root_map[len][s] = {len, s};
}
PredSet TracebackEngine::why_length_of(Length* len, Segment* s) {
    auto [lc, sc] = length_segment_root_map[len][s];
    PredSet res(length_of_segments[lc][sc]);
    res += TracebackUtils::why_ancestor(lc, len);
    res += TracebackUtils::why_ancestor(sc, s);
    return res;
}
void TracebackEngine::set_dimension_of(Dimension* dim, Triangle* t, PredSet pred) {
    dimension_of_triangles[dim][t] = pred;
    dimension_triangle_root_map[dim][t] = {dim, t};
}




void TracebackEngine::make_angle_with_directions(Angle* a, Direction* d1, Direction* d2) {
    directions_of_angles[{d1, d2}] = a;
    angle_directions_root_map[{d1, d2}].insert({d1, d2});
}

void TracebackEngine::make_ratio_with_lengths(Ratio* r, Length* len1, Length* len2) {
    lengths_of_ratios[{len1, len2}] = r;
    ratio_lengths_root_map[{len1, len2}].insert({len1, len2});
}




void TracebackEngine::set_measure_of(Measure* m, Angle* a, PredSet pred) {
    measure_of_angles[m][a] = pred;
    measure_angle_root_map[m][a] = {m, a};
}
PredSet TracebackEngine::why_measure_of(Measure* m, Angle* a) {
    auto [mc, ac] = measure_angle_root_map[m][a];
    PredSet res(measure_of_angles[mc][ac]);
    res += TracebackUtils::why_ancestor(mc, m);
    res += TracebackUtils::why_ancestor(ac, a);
    return res;
}
void TracebackEngine::set_fraction_of(Fraction* f, Ratio* r, PredSet pred) {
    fraction_of_ratios[f][r] = pred;
    fraction_ratio_root_map[f][r] = {f, r};
}
PredSet TracebackEngine::why_fraction_of(Fraction* f, Ratio* r) {
    auto [fc, rc] = fraction_ratio_root_map[f][r];
    PredSet res(fraction_of_ratios[fc][rc]);
    res += TracebackUtils::why_ancestor(fc, f);
    res += TracebackUtils::why_ancestor(rc, r);
    return res;
}
void TracebackEngine::set_shape_of(Shape* s, Dimension* d, PredSet pred) {
    shape_of_dimensions[s][d] = pred;
    shape_dimension_root_map[s][d] = {s, d};
}



void TracebackEngine::set_measure_val(Measure* m, Frac val, PredSet pred) {
    measure_vals[m] = {val, pred};
}
void TracebackEngine::set_fraction_val(Fraction* f, Frac val, PredSet pred) {
    fraction_vals[f] = {val, pred};
}

void TracebackEngine::set_goal(Predicate* pred) {
    goal = pred;
}






Direction* TracebackEngine::__earliest_direction_of(
    Line* l, 
    std::map<Line*, Direction*>& earliest_direction_cache
) {
    if (earliest_direction_cache.contains(l)) {
        return earliest_direction_cache[l];
    }
    Direction* earliest = nullptr;
    for (auto [d, _] : direction_line_root_map) {
        if (direction_line_root_map[d].contains(l)) {
            if (!earliest || NodeUtils::ancestor_of(earliest, d)) {
                earliest = d;
            }
        }
    }
    if (!earliest) return __earliest_direction_of(NodeUtils::get_parent(l), earliest_direction_cache);
    earliest_direction_cache[l] = earliest;
    return earliest;
}
Length* TracebackEngine::__earliest_length_of(
    Segment* s,
    std::map<Segment*, Length*>& earliest_length_cache
) {
    if (earliest_length_cache.contains(s)) {
        return earliest_length_cache[s];
    }
    Length* earliest = nullptr;
    for (auto [l, _] : length_segment_root_map) {
        if (length_segment_root_map[l].contains(s)) {
            if (!earliest || NodeUtils::ancestor_of(earliest, l)) {
                earliest = l;
            }
        }
    }
    if (!earliest) return __earliest_length_of(NodeUtils::get_parent(s), earliest_length_cache);
    earliest_length_cache[s] = earliest;
    return earliest;
}

Measure* TracebackEngine::__earliest_measure_of(
    Angle* a,
    std::map<Angle*, Measure*>& earliest_measure_cache
) {
    if (earliest_measure_cache.contains(a)) {
        return earliest_measure_cache[a];
    }
    Measure* earliest = nullptr;
    for (auto [m, _] : measure_angle_root_map) {
        if (measure_angle_root_map[m].contains(a)) {
            if (!earliest || NodeUtils::ancestor_of(earliest, m)) {
                earliest = m;
            }
        }
    }
    if (!earliest) return __earliest_measure_of(NodeUtils::get_parent(a), earliest_measure_cache);
    earliest_measure_cache[a] = earliest;
    return earliest;
}
Fraction* TracebackEngine::__earliest_fraction_of(
    Ratio* r,
    std::map<Ratio*, Fraction*>& earliest_fraction_cache
) {
    if (earliest_fraction_cache.contains(r)) {
        return earliest_fraction_cache[r];
    }
    Fraction* earliest = nullptr;
    for (auto [f, _] : fraction_ratio_root_map) {
        if (fraction_ratio_root_map[f].contains(r)) {
            if (!earliest || NodeUtils::ancestor_of(earliest, f)) {
                earliest = f;
            }
        }
    }
    if (!earliest) return __earliest_fraction_of(NodeUtils::get_parent(r), earliest_fraction_cache);
    earliest_fraction_cache[r] = earliest;
    return earliest;
}





std::tuple<std::map<Line*, PredSet>, Line*> TracebackEngine::lca_lines_and_why(
    Point* p1, Point* p2,
    std::map<std::pair<Point*, Point*>, PredSet>& why_point_ancestor_cache,
    std::map<std::pair<Line*, Line*>, PredSet>& why_line_ancestor_cache
) {    
    /* Step 1: Extract all children of p1 and p2 */
    std::array<std::set<Point*>, 2> pcs;
    int i = 0;
    for (Point* p : std::array<Point*, 2>{p1, p2}) {
        NodeUtils::all_children(p, pcs[i]);
        i++;
    }

    /* Step 2: For each child point, extract all lines it was placed on.
    It suffices to consider lines whose roots have Directions */
    std::array<std::vector<std::pair<Line*, Point*>>, 2> l2ps;
    std::array<std::set<Line*>, 2> rls;
    for (i = 0; i < 2; i++) {
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

    /* Step 3: Find a common root line to both `rl` sets */
    Line* common_root = nullptr;
    for (Line* rl : rls[0]) {
        if (rls[1].contains(rl)) {
            common_root = rl;
            break;
        }
    }

    /* Step 4: Keep those lines in `l2ps` with root `common_root`. */
    for (i=0; i<2; i++) {
        for (auto it = l2ps[i].begin(); it != l2ps[i].end(); ) {
            if (!NodeUtils::same_as(it->first, common_root)) {
                it = l2ps[i].erase(it);
            } else {
                ++it;
            }
        }
    }

    /* Step 5: Iterate over all pairs of lines in `l2ps` of `l1, l2`, identifying the `lca`
    of each pair and its associated why's. These `lca`s are the "earliest possible" candidates
    for the line `p1p2`.
    Note: There may be multiple candidates for `lca` because, if there were formerly points 
    `(q1, q2)` and `(r1, r2)`, and both `qi, ri` were merged into `pi`, it is not inconceivable 
    that there might be some `lca1` containing `(q1, q2)` and some other `lca1` containing 
    `(r1, r2)`. */
    std::map<Line*, PredSet> lcas;
    for (auto [l_p1, cp1] : l2ps[0]) {
        for (auto [l_p2, cp2] : l2ps[1]) {
            auto lca_ = TracebackUtils::lowest_common_ancestor(l_p1, l_p2);
            Line* lca = lca_.first;
            PredSet preds_ = (
                TracebackUtils::why_ancestor_with_cache(cp1, p1, why_point_ancestor_cache)
                + TracebackUtils::why_ancestor_with_cache(cp2, p2, why_point_ancestor_cache)
                + TracebackUtils::why_ancestor_with_cache(l_p1, lca, why_line_ancestor_cache)
                + TracebackUtils::why_ancestor_with_cache(l_p2, lca, why_line_ancestor_cache)
                + point_on_lines[cp1][l_p1] + point_on_lines[cp2][l_p2]
            );
            if (!lcas.contains(lca) || preds_ < lcas[lca]) {
                lcas[lca] = std::move(preds_);
            }
        }
    }

    return {lcas, common_root};
}

std::tuple<std::map<Segment*, PredSet>, Segment*> TracebackEngine::lca_segments_and_why(
    Point* p1, Point* p2,
    std::map<std::pair<Point*, Point*>, PredSet>& why_point_ancestor_cache,
    std::map<std::pair<Segment*, Segment*>, PredSet>& why_segment_ancestor_cache
) {
    /* Step 1: Extract all children of p1 and p2 */
    std::array<std::set<Point*>, 2> pcs;
    int i = 0;
    for (Point* p : std::array<Point*, 2>{p1, p2}) {
        NodeUtils::all_children(p, pcs[i]);
        i++;
    }

    /* Step 2: For each child point, extract all segments it was placed on.
    It suffices to consider segments whose roots have Lengths */
    std::array<std::vector<std::pair<Segment*, Point*>>, 2> s2ps;
    std::array<std::set<Segment*>, 2> rss;
    for (i = 0; i < 2; i++) {
        for (Point* p : pcs[i]) {
            for (auto [l, pred] : point_as_segment_endpoint[p]) {
                Segment* rl = NodeUtils::get_root(l);
                if (rl->has_length()) {
                    s2ps[i].emplace_back(l, p);
                    rss[i].insert(rl);
                }
            }
        }
    }

    /* Step 3: Identify a common root segment to all two `rss` sets. */
    Segment* common_root = nullptr;
    for (Segment* rl : rss[0]) {
        if (rss[1].contains(rl)) {
            common_root = rl;
            break;
        }
    }

    /* Step 4: Keep those segments in `s2ps` with root `common_root`. */
    for (i=0; i<2; i++) {
        for (auto it = s2ps[i].begin(); it != s2ps[i].end(); ) {
            if (!NodeUtils::same_as(it->first, common_root)) {
                it = s2ps[i].erase(it);
            } else {
                ++it;
            }
        }
    }

    /* Step 5: Iterate over all pairs of segments in `s2ps` of `s1, s2`, identifying the
    LCAs of each pair and its associated why's. */
    std::map<Segment*, PredSet> lcas;
    for (auto [s_p1, cp1] : s2ps[0]) {
        for (auto [s_p2, cp2] : s2ps[1]) {
            auto lca_ = TracebackUtils::lowest_common_ancestor(s_p1, s_p2);
            Segment* lca = lca_.first;
            PredSet preds_ = (
                TracebackUtils::why_ancestor_with_cache(cp1, p1, why_point_ancestor_cache)
                + TracebackUtils::why_ancestor_with_cache(cp2, p2, why_point_ancestor_cache)
                + TracebackUtils::why_ancestor_with_cache(s_p1, lca, why_segment_ancestor_cache)
                + TracebackUtils::why_ancestor_with_cache(s_p2, lca, why_segment_ancestor_cache)
                + point_as_segment_endpoint[cp1][s_p1] + point_as_segment_endpoint[cp2][s_p2]
            );
            if (!lcas.contains(lca) || preds_ < lcas[lca]) {
                lcas[lca] = std::move(preds_);
            }
        }
    }

    return {lcas, common_root};
}





std::pair<std::pair<Direction*, Line*>, PredSet> TracebackEngine::most_explainable_direction_of_line(
    Line* l, Direction* d,
    std::map<std::pair<Direction*, Direction*>, PredSet>& why_direction_ancestor_cache,
    std::map<std::pair<Line*, Line*>, PredSet>& why_line_ancestor_cache
) {
    /* Extract all children of `l` which were assigned directions */
    std::set<Line*> l_cs;
    std::vector<std::pair<Direction*, Line*>> d2ls;

    NodeUtils::all_children(l, l_cs);
    for (const auto& [dir, line_map] : direction_of_lines) {
        for (Line* l_c : l_cs) {
            if (line_map.contains(l_c)) {
                d2ls.emplace_back(dir, l_c);
            }
        }
    }

    /* Each child `l_c` will have been assigned some Direction `d_c`.
    The desired PredSet is the union of the following:
    - direction_of_lines[d_1][lca_1]
    - why_ancestor(lca_1, lca1)
    - why_ancestor(d_1, d1)
    Pick the `lca_1` with the lowest predicate count. Do the same thing for `lca_2`. 
    Generate the union of these two PredSets. */
    PredSet res;
    Direction* best_d = nullptr;
    Line* best_l = nullptr;
    for (auto [d_c, l_c] : d2ls) {
        PredSet res_ = (
            TracebackUtils::why_ancestor_with_cache(l_c, l, why_line_ancestor_cache)
            + TracebackUtils::why_ancestor_with_cache(d_c, d, why_direction_ancestor_cache)
            + direction_of_lines[d_c][l_c]
        );
        if (res_ < res) {
            res = std::move(res_);
            best_d = d_c;
            best_l = l_c;
        }
    }
    return {{best_d, best_l}, res};
}
std::pair<std::pair<Length*, Segment*>, PredSet> TracebackEngine::most_explainable_length_of_segment(
    Segment* s, Length* len,
    std::map<std::pair<Length*, Length*>, PredSet>& why_length_ancestor_cache,
    std::map<std::pair<Segment*, Segment*>, PredSet>& why_segment_ancestor_cache
) {
    /* Extract all children of `s` which were assigned lengths */
    std::set<Segment*> s_cs;
    std::vector<std::pair<Length*, Segment*>> l2ss;

    NodeUtils::all_children(s, s_cs);
    for (const auto& [length, segment_map] : length_of_segments) {
        for (Segment* s_c : s_cs) {
            if (segment_map.contains(s_c)) {
                l2ss.emplace_back(length, s_c);
            }
        }
    }

    /* Each child `s_c` will have been assigned some Length `len_c`.
    The desired PredSet is the union of the following:
    - length_of_segments[len_c][s_c]
    - why_ancestor(s_c, s)
    - why_ancestor(len_c, len)
    Pick the `s_c` with the lowest predicate count. */
    PredSet res;
    Length* best_len = nullptr;
    Segment* best_s = nullptr;
    for (auto [len_c, s_c] : l2ss) {
        PredSet res_ = (
            TracebackUtils::why_ancestor_with_cache(s_c, s, why_segment_ancestor_cache)
            + TracebackUtils::why_ancestor_with_cache(len_c, len, why_length_ancestor_cache)
            + length_of_segments[len_c][s_c]
        );
        if (res_ < res) {
            res = std::move(res_);
            best_len = len_c;
            best_s = s_c;
        }
    }
    return {{best_len, best_s}, res};
}
std::pair<std::pair<Measure*, Angle*>, PredSet> TracebackEngine::most_explainable_measure_of_angle(
    Angle* a, Measure* m,
    std::map<std::pair<Measure*, Measure*>, PredSet>& why_measure_ancestor_cache,
    std::map<std::pair<Angle*, Angle*>, PredSet>& why_angle_ancestor_cache
) {
    /* Extract all children of `a` which were assigned measures */
    std::set<Angle*> a_cs;
    std::vector<std::pair<Measure*, Angle*>> m2as;

    NodeUtils::all_children(a, a_cs);
    for (const auto& [measure, angle_map] : measure_of_angles) {
        for (Angle* a_c : a_cs) {
            if (angle_map.contains(a_c)) {
                m2as.emplace_back(measure, a_c);
            }
        }
    }

    /* Each child `a_c` will have been assigned some Measure `m_c`.
    The desired PredSet is the union of the following:
    - measure_of_angles[m_c][a_c]
    - why_ancestor(a_c, a)
    - why_ancestor(m_c, m)
    Pick the `a_c` with the lowest predicate count. */
    PredSet res;
    Measure* best_m = nullptr;
    Angle* best_a = nullptr;
    for (auto [m_c, a_c] : m2as) {
        PredSet res_ = (
            TracebackUtils::why_ancestor_with_cache(a_c, a, why_angle_ancestor_cache)
            + TracebackUtils::why_ancestor_with_cache(m_c, m, why_measure_ancestor_cache)
            + measure_of_angles[m_c][a_c]
        );
        if (res_ < res) {
            res = std::move(res_);
            best_m = m_c;
            best_a = a_c;
        }
    }
    return {{best_m, best_a}, res};
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
    for (auto [l1, pc1] : l2ps[0]) {
        for (auto [l2, pc2] : l2ps[1]) {
            for (auto [l3, pc3] : l2ps[2]) {
                PredSet res_ = point_on_lines[pc1][l1] + point_on_lines[pc2][l2] + point_on_lines[pc3][l3];

                std::pair<Line*, int> lca_p = TracebackUtils::lowest_common_ancestor<Line>(l1, l2, l3);
                Line* lca_ = lca_p.first;
                
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

                // std::cout << "Points " << pc1->to_string() << ", " << pc2->to_string() << ", " << pc3->to_string() 
                //     << " lie on lines " << l1->to_string() << ", " << l2->to_string() << ", " << l3->to_string()
                //     << " with LCA " << lca_->to_string() << " and res_ " << res_.to_string() << std::endl;

                if (res_ < res) {
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
    for (auto [c1, pc1] : c2ps[0]) {
        for (auto [c2, pc2] : c2ps[1]) {
            for (auto [c3, pc3] : c2ps[2]) {
                for (auto [c4, pc4] : c2ps[3]) {
                    PredSet res_ =
                        point_on_circles[pc1][c1] + point_on_circles[pc2][c2] + 
                        point_on_circles[pc3][c3] + point_on_circles[pc4][c4]
                    ;

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

                    if (res_ < res) {
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
    for (auto [c0, cc] : circ2c) {
        for (auto [c1, pc1] : p2cs[0]) {
            for (auto [c2, pc2] : p2cs[1]) {
                for (auto [c3, pc3] : p2cs[2]) {
                    PredSet res_ =
                        point_on_circles[pc1][c1] + point_on_circles[pc2][c2] + point_on_circles[pc3][c3]
                        + point_as_circle_center[cc][c0]
                    ;

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

                    if (res_ < res) {
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
    std::map<Line*, Direction*> earliest_direction_cache;

    // std::cout << "---- why_para " << p1->to_string() << " " << p2->to_string() << " " << p3->to_string() << " " << p4->to_string() << std::endl;

    /* Steps 1-5: Fetch the lca_lines of `p1p2` and `p3p4` */
    Line* common_root_12 = nullptr, *common_root_34 = nullptr;
    std::map<Line*, PredSet> lca1s, lca2s;
    std::tie(lca1s, common_root_12) = lca_lines_and_why(p1, p2, why_point_ancestor_cache, why_line_ancestor_cache);
    std::tie(lca2s, common_root_34) = lca_lines_and_why(p3, p4, why_point_ancestor_cache, why_line_ancestor_cache);

    Direction* common_root_direction = common_root_12->get_direction();
    assert(NodeUtils::same_as(common_root_direction, common_root_34->get_direction()));

    /* Now we check every possible pair of LCAs `(lca1, lca2)` */
    for (const auto& [lca1, why_ancestor_lines_points_12] : lca1s) {
        for (const auto& [lca2, why_ancestor_lines_points_34] : lca2s) {

            PredSet res_ = why_ancestor_lines_points_12 + why_ancestor_lines_points_34;
            // std::cout << lca1->to_string() << " and " << lca2->to_string() << ": " 
            //     << why_ancestor_lines_points_12.to_string() << " and " 
            //     << why_ancestor_lines_points_34.to_string() << " | ";

            /* Branch 1: If `lca1` and `lca2` are the same (call it `lca`) */
            if (lca1 == lca2) {
                Direction* d = __earliest_direction_of(lca1, earliest_direction_cache);
                assert(NodeUtils::same_as(d, common_root_direction));

                // std::cout << d->to_string() << std::endl;

                /* Steps 6-7: extract the shortest explanation for why `lca` was assigned direction `d` */
                auto [best_pair, res_0] = most_explainable_direction_of_line(
                    lca1, d, why_direction_ancestor_cache, why_line_ancestor_cache
                );

                // std::cout << "res_: " << res_.to_string() << ", res_0: " << res_0.to_string() << std::endl;
                res_ += std::move(res_0);
            }
            
            /* Branch 2: If `lca1` and `lca2` have Directions `d1, d2` (these may be the same) */
            else {
                Direction* d1 = __earliest_direction_of(lca1, earliest_direction_cache), 
                    *d2 = __earliest_direction_of(lca2, earliest_direction_cache);
                assert(NodeUtils::same_as(d1, common_root_direction));
                assert(NodeUtils::same_as(d2, common_root_direction));

                // std::cout << d1->to_string() << " and " << d2->to_string() << ", ";

                /* Step 6: Find ancestors `lca1_a` and `lca2_a` which were the same direction `d` at
                some point in time
                We should choose `d` to be the "least possible", i.e. the LCA of `d1` and `d2`
                (It could be the case that `lca1 == lca1_a` and `lca2 == lca2_a` and `d == d1 == d2`) */
                Line* lca1_a = lca1, *lca2_a = lca2;
                Direction* d = TracebackUtils::lowest_common_ancestor(d1, d2).first;
                assert(NodeUtils::same_as(d, common_root_direction));
                
                if (!NodeUtils::ancestor_of(lca1->direction, d)) {
                    while (!(lca1_a->is_root())) {
                        lca1_a = NodeUtils::get_parent(lca1_a);
                        if (NodeUtils::ancestor_of(lca1_a->direction, d)) {
                            break;
                        }
                    }
                }
                if (!NodeUtils::ancestor_of(lca2->direction, d)) {
                    while (!(lca2_a->is_root())) {
                        lca2_a = NodeUtils::get_parent(lca2_a);
                        if (NodeUtils::ancestor_of(lca2_a->direction, d)) {
                            break;
                        }
                    }
                }

                // std::cout << "(" << d->to_string() << ": " << d1->to_string() << ", " << lca1_a->to_string() << " and " 
                //         << d2->to_string() << ", " << lca2_a->to_string() << ")" << std::endl;

                /* Steps 7-8: extract the shortest explanations for why `lcai` was assigned direction `di` */
                auto [best_pair_1, res_1] = most_explainable_direction_of_line(
                    lca1, d1, why_direction_ancestor_cache, why_line_ancestor_cache
                );
                auto [best_pair_2, res_2] = most_explainable_direction_of_line(
                    lca2, d2, why_direction_ancestor_cache, why_line_ancestor_cache
                );

                // std::cout << "res_: " << res_.to_string() << ", res_1: " << res_1.to_string() << ", res_2: " << res_2.to_string() << std::endl;

                /* Step 9: Add to these PredSets the following:
                - why_ancestor(lcai, lcai_a) for each i
                - why_ancestor(di, d) for each i */
                res_ += (std::move(res_1) + std::move(res_2));
                PredSet res_a = (
                    TracebackUtils::why_ancestor_with_cache(lca1, lca1_a, why_line_ancestor_cache)
                    + TracebackUtils::why_ancestor_with_cache(lca2, lca2_a, why_line_ancestor_cache)
                    + TracebackUtils::why_ancestor_with_cache(d1, d, why_direction_ancestor_cache)
                    + TracebackUtils::why_ancestor_with_cache(d2, d, why_direction_ancestor_cache)
                );
                // std::cout << "res_a: " << res_a.to_string() << std::endl;
                res_ += std::move(res_a);
            }

            if (res_ < res) {
                res = std::move(res_);
            }
        }
    }

    return res;
}


PredSet TracebackEngine::why_perp(Point* p1, Point* p2, Point* p3, Point* p4) {
    PredSet res;
    std::map<std::pair<Point*, Point*>, PredSet> why_point_ancestor_cache;
    std::map<std::pair<Line*, Line*>, PredSet> why_line_ancestor_cache;
    std::map<std::pair<Direction*, Direction*>, PredSet> why_direction_ancestor_cache;
    std::map<Line*, Direction*> earliest_direction_cache;

    /* Steps 1-5: Fetch the lca_lines of `p1p2` and `p3p4` */
    Line* common_root_12 = nullptr, *common_root_34 = nullptr;
    std::map<Line*, PredSet> lca1s, lca2s;
    std::tie(lca1s, common_root_12) = lca_lines_and_why(p1, p2, why_point_ancestor_cache, why_line_ancestor_cache);
    std::tie(lca2s, common_root_34) = lca_lines_and_why(p3, p4, why_point_ancestor_cache, why_line_ancestor_cache);

    assert(Line::is_perp(common_root_12, common_root_34));

    Direction* rd1 = common_root_12->get_direction(), *rd2 = common_root_34->get_direction();

    /* Step 6: Identify all instances at which children `(pd1, pd2)` of `(rd1, rd2)` were set to
    be perpendicular */
    std::set<std::pair<Direction*, Direction*>> dir_pairs = perp_directions_root_map[{rd1, rd2}];
    for (const auto& [pd2, pd1] : perp_directions_root_map[{rd2, rd1}]) {
        dir_pairs.insert({pd1, pd2});
    }

    /* Now, we check every possible pair (lca1, lca2) */
    for (const auto& [lca1, why_ancestor_lines_points_12] : lca1s) {
        for (const auto& [lca2, why_ancestor_lines_points_34] : lca2s) {
            PredSet res_ = why_ancestor_lines_points_12 + why_ancestor_lines_points_34;

            Direction* d1 = __earliest_direction_of(lca1, earliest_direction_cache), 
                *d2 = __earliest_direction_of(lca2, earliest_direction_cache);

            /* Steps 7-8: extract the shortest explanations for why `lcai` was assigned direction `di` */
            auto [best_pair_1, res1] = most_explainable_direction_of_line(
                lca1, d1, why_direction_ancestor_cache, why_line_ancestor_cache
            );
            auto [best_pair_2, res2] = most_explainable_direction_of_line(
                lca2, d2, why_direction_ancestor_cache, why_line_ancestor_cache
            );

            res_ += (std::move(res1) + std::move(res2));
            
            /* Step 9: check every possible pair (pd1, pd2) and extract the pair for which the explanations
            for pd1 <-> d1, pd2 <-> d2 and perp_directions[pd1, pd2] sum to be the shortest */
            PredSet res_0;
            for (auto [pd1, pd2] : dir_pairs) {
                PredSet res_0_ = perp_directions[{pd1, pd2}];

                auto [ad1, x1] = TracebackUtils::lowest_common_ancestor(d1, pd1);
                auto [ad2, x2] = TracebackUtils::lowest_common_ancestor(d2, pd2);

                // std::cout << "lca1: " << lca1->to_string() << ", lca2: " << lca2->to_string()
                //     << ", d1: " << d1->to_string() << ", d2: " << d2->to_string() 
                //     << ", pd1: " << pd1->to_string() << ", pd2: " << pd2->to_string() 
                //     << ", ad1: " << ad1->to_string() << ", ad2: " << ad2->to_string() << std::endl;

                // std::cout << "res0_: " << res0_.to_string()
                //     << ", res_1: " << res_1.to_string()
                //     << ", res_2: " << res_2.to_string() << std::endl;

                res_0_ += (
                    TracebackUtils::why_ancestor_with_cache(d1, ad1, why_direction_ancestor_cache)
                    + TracebackUtils::why_ancestor_with_cache(pd1, ad1, why_direction_ancestor_cache)
                    + TracebackUtils::why_ancestor_with_cache(d2, ad2, why_direction_ancestor_cache)
                    + TracebackUtils::why_ancestor_with_cache(pd2, ad2, why_direction_ancestor_cache)
                );

                if (res_0_ < res_0) {
                    res_0 = std::move(res_0_);
                }
            }

            res_ += res_0;
            
            if (res_ < res) {
                res = std::move(res_);
            }
        }
    }

    return res;
}





PredSet TracebackEngine::why_cong(Point* p1, Point* p2, Point* p3, Point* p4) {
    PredSet res;
    std::map<std::pair<Point*, Point*>, PredSet> why_point_ancestor_cache;
    std::map<std::pair<Segment*, Segment*>, PredSet> why_segment_ancestor_cache;
    std::map<std::pair<Length*, Length*>, PredSet> why_length_ancestor_cache;
    std::map<Segment*, Length*> earliest_length_cache;

    /* Steps 1-5: Fetch the lca_segments of `p1p2` and `p3p4` */
    Segment* common_root_12 = nullptr, *common_root_34 = nullptr;
    std::map<Segment*, PredSet> lca1s, lca2s;
    std::tie(lca1s, common_root_12) = lca_segments_and_why(p1, p2, why_point_ancestor_cache, why_segment_ancestor_cache);
    std::tie(lca2s, common_root_34) = lca_segments_and_why(p3, p4, why_point_ancestor_cache, why_segment_ancestor_cache);

    Length* common_root_length = common_root_12->get_length();
    assert(NodeUtils::same_as(common_root_length, common_root_34->get_length()));
    
    for (const auto& [lca1, why_ancestor_segments_points_12] : lca1s) {
        for (const auto& [lca2, why_ancestor_segments_points_34] : lca2s) {
            PredSet res_ = why_ancestor_segments_points_12 + why_ancestor_segments_points_34;

            Length* len1 = __earliest_length_of(lca1, earliest_length_cache), 
                *len2 = __earliest_length_of(lca2, earliest_length_cache);

            /* Step 6: Find ancestors `lca1_a` and `lca2_a` which were the same length `len` at
            some point in time */
            Segment* lca1_a = lca1, *lca2_a = lca2;
            auto [len, _] = TracebackUtils::lowest_common_ancestor(len1, len2);
            assert(NodeUtils::same_as(len, common_root_length));
            
            if (!NodeUtils::ancestor_of(lca1->length, len)) {
                while (!(lca1_a->is_root())) {
                    lca1_a = NodeUtils::get_parent(lca1_a);
                    if (NodeUtils::ancestor_of(lca1_a->length, len)) {
                        break;
                    }
                }
            }
            if (!NodeUtils::ancestor_of(lca2->length, len)) {
                while (!(lca2_a->is_root())) {
                    lca2_a = NodeUtils::get_parent(lca2_a);
                    if (NodeUtils::ancestor_of(lca2_a->length, len)) {
                        break;
                    }
                }
            }

            /* Steps 7-8: extract the shortest explanations for why `lcai` was assigned length `len_i` */
            auto [best_pair_1, res_1] = most_explainable_length_of_segment(
                lca1, len1, why_length_ancestor_cache, why_segment_ancestor_cache
            );
            auto [best_pair_2, res_2] = most_explainable_length_of_segment(
                lca2, len2, why_length_ancestor_cache, why_segment_ancestor_cache
            );

            res_ += std::move(res_1);
            res_ += std::move(res_2);

            res_ += (
                TracebackUtils::why_ancestor_with_cache(lca1, lca1_a, why_segment_ancestor_cache)
                + TracebackUtils::why_ancestor_with_cache(lca2, lca2_a, why_segment_ancestor_cache)
                + TracebackUtils::why_ancestor_with_cache(len1, len, why_length_ancestor_cache)
                + TracebackUtils::why_ancestor_with_cache(len2, len, why_length_ancestor_cache)
            );

            if (res_ < res) {
                res = std::move(res_);
            }
        }
    }

    return res;
}

PredSet TracebackEngine::why_midp(Point* m, Point* p1, Point* p2) {
    return (
        why_cong(m, p1, m, p2)
        + why_coll(m, p1, p2)
    );
}






PredSet TracebackEngine::why_eqangle(Point* p1, Point* p2, Point* p3, Point* p4, Point* p5, Point* p6, Point* p7, Point* p8) {
    PredSet res;
    std::map<std::pair<Point*, Point*>, PredSet> why_point_ancestor_cache;
    std::map<std::pair<Line*, Line*>, PredSet> why_line_ancestor_cache;
    std::map<std::pair<Direction*, Direction*>, PredSet> why_direction_ancestor_cache;
    std::map<std::pair<Angle*, Angle*>, PredSet> why_angle_ancestor_cache;
    std::map<std::pair<Measure*, Measure*>, PredSet> why_measure_ancestor_cache;

    std::map<Line*, Direction*> earliest_direction_cache;
    std::map<Angle*, Measure*> earliest_measure_cache;

    /* Steps 1-5: Fetch the lca_lines of `p1p2`, `p3p4`, `p5p6` and `p7p8` */
    Line* common_root_12 = nullptr, *common_root_34 = nullptr, *common_root_56 = nullptr, *common_root_78 = nullptr;
    std::map<Line*, PredSet> lca1s, lca2s, lca3s, lca4s;
    std::tie(lca1s, common_root_12) = lca_lines_and_why(p1, p2, why_point_ancestor_cache, why_line_ancestor_cache);
    std::tie(lca2s, common_root_34) = lca_lines_and_why(p3, p4, why_point_ancestor_cache, why_line_ancestor_cache);
    std::tie(lca3s, common_root_56) = lca_lines_and_why(p5, p6, why_point_ancestor_cache, why_line_ancestor_cache);
    std::tie(lca4s, common_root_78) = lca_lines_and_why(p7, p8, why_point_ancestor_cache, why_line_ancestor_cache);

    Direction* rd1 = common_root_12->get_direction(), *rd2 = common_root_34->get_direction(),
        *rd3 = common_root_56->get_direction(), *rd4 = common_root_78->get_direction();

    std::set<std::pair<Direction*, Direction*>> dir_pairs_12 = angle_directions_root_map[{rd1, rd2}];
    std::set<std::pair<Direction*, Direction*>> dir_pairs_34 = angle_directions_root_map[{rd3, rd4}];

    /* Now we check every possible pair of LCAs `(lca1, lca2)` */
    for (const auto& [lca1, why_ancestor_lines_points_12] : lca1s) {
        for (const auto& [lca2, why_ancestor_lines_points_34] : lca2s) {
            for (const auto& [lca3, why_ancestor_lines_points_56] : lca3s) {
                for (const auto& [lca4, why_ancestor_lines_points_78] : lca4s) {

                    PredSet res_ = (
                        why_ancestor_lines_points_12 + why_ancestor_lines_points_34
                        + why_ancestor_lines_points_56 + why_ancestor_lines_points_78
                    );
                    Direction* d1 = __earliest_direction_of(lca1, earliest_direction_cache), 
                        *d2 = __earliest_direction_of(lca2, earliest_direction_cache),
                        *d3 = __earliest_direction_of(lca3, earliest_direction_cache), 
                        *d4 = __earliest_direction_of(lca4, earliest_direction_cache);

                    /* Steps 6-7: extract the shortest explanations for why each lcai was assigned direction di */
                    auto [best_pair_1, res1] = most_explainable_direction_of_line(
                        lca1, d1, why_direction_ancestor_cache, why_line_ancestor_cache
                    );
                    auto [best_pair_2, res2] = most_explainable_direction_of_line(
                        lca2, d2, why_direction_ancestor_cache, why_line_ancestor_cache
                    );
                    auto [best_pair_3, res3] = most_explainable_direction_of_line(
                        lca3, d3, why_direction_ancestor_cache, why_line_ancestor_cache
                    );
                    auto [best_pair_4, res4] = most_explainable_direction_of_line(
                        lca4, d4, why_direction_ancestor_cache, why_line_ancestor_cache
                    );

                    res_ += (std::move(res1) + std::move(res2) + std::move(res3) + std::move(res4));

                    /* Now, we iterate over pairs `(cd1, cd2)` and `(cd3, cd4)` of directions used to define angles */
                    PredSet res_0;
                    for (const auto& [cd1, cd2] : dir_pairs_12) {
                        for (const auto& [cd3, cd4] : dir_pairs_34) {

                            /* Step 9-10: For each pair `(cd1, cd2)` and `(cd3, cd4)`, we identify the defined angles
                            `a1, a2` as well as their earliest measures `m1, m2`, then extract the shortest explanations
                            for why each angle was assigned its corresponding measure */
                            Angle* a1 = directions_of_angles[{cd1, cd2}], *a2 = directions_of_angles[{cd3, cd4}];
                            Measure* m1 = __earliest_measure_of(a1, earliest_measure_cache), 
                                *m2 = __earliest_measure_of(a2, earliest_measure_cache);

                            auto [best_pair_a1, res_a1] = most_explainable_measure_of_angle(
                                a1, m1, why_measure_ancestor_cache, why_angle_ancestor_cache
                            );
                            auto [best_pair_a2, res_a2] = most_explainable_measure_of_angle(
                                a2, m2, why_measure_ancestor_cache, why_angle_ancestor_cache
                            );

                            PredSet res_0_ = (std::move(res_a1) + std::move(res_a2));

                            /* Step 11: Find ancestors `a1_a` and `a2_a` which had the same measure `m` at some point
                            in time
                            We should choose `m` to be the "least possible", i.e. the LCA of `m1` and `m2` */
                            Angle* a1_a = a1, *a2_a = a2;
                            Measure* m = TracebackUtils::lowest_common_ancestor(m1, m2).first;
                            
                            if (!NodeUtils::ancestor_of(a1_a->measure, m)) {
                                while (!(a1_a->is_root())) {
                                    a1_a = NodeUtils::get_parent(a1_a);
                                    if (NodeUtils::ancestor_of(a1_a->measure, m)) {
                                        break;
                                    }
                                }
                            }
                            if (!NodeUtils::ancestor_of(a2_a->measure, m)) {
                                while (!(a2_a->is_root())) {
                                    a2_a = NodeUtils::get_parent(a2_a);
                                    if (NodeUtils::ancestor_of(a2_a->measure, m)) {
                                        break;
                                    }
                                }
                            }

                            auto [ad1, x1] = TracebackUtils::lowest_common_ancestor(d1, cd1);
                            auto [ad2, x2] = TracebackUtils::lowest_common_ancestor(d2, cd2);
                            auto [ad3, x3] = TracebackUtils::lowest_common_ancestor(d3, cd3);
                            auto [ad4, x4] = TracebackUtils::lowest_common_ancestor(d4, cd4);

                            res_0_ += (
                                TracebackUtils::why_ancestor_with_cache(d1, ad1, why_direction_ancestor_cache)
                                + TracebackUtils::why_ancestor_with_cache(cd1, ad1, why_direction_ancestor_cache)
                                + TracebackUtils::why_ancestor_with_cache(d2, ad2, why_direction_ancestor_cache)
                                + TracebackUtils::why_ancestor_with_cache(cd2, ad2, why_direction_ancestor_cache)
                                + TracebackUtils::why_ancestor_with_cache(d3, ad3, why_direction_ancestor_cache)
                                + TracebackUtils::why_ancestor_with_cache(cd3, ad3, why_direction_ancestor_cache)
                                + TracebackUtils::why_ancestor_with_cache(d4, ad4, why_direction_ancestor_cache)
                                + TracebackUtils::why_ancestor_with_cache(cd4, ad4, why_direction_ancestor_cache)
                                + TracebackUtils::why_ancestor_with_cache(a1, a1_a, why_angle_ancestor_cache)
                                + TracebackUtils::why_ancestor_with_cache(a2, a2_a, why_angle_ancestor_cache)
                                + TracebackUtils::why_ancestor_with_cache(m1, m, why_measure_ancestor_cache)
                                + TracebackUtils::why_ancestor_with_cache(m2, m, why_measure_ancestor_cache)
                            );

                            std::cout << "For lines " << lca1->to_string() << ", " << lca2->to_string() << ", " 
                                << lca3->to_string() << ", " << lca4->to_string() 
                                << " with directions " << d1->to_string() << ", " << d2->to_string() << ", " 
                                << d3->to_string() << ", " << d4->to_string() 
                                << " and angle directions " << cd1->to_string() << ", " << cd2->to_string() << ", " 
                                << cd3->to_string() << ", " << cd4->to_string() 
                                << " and angles (" << a1->to_string() << ", " << a1_a->to_string() 
                                    << "), (" << a2->to_string() << ", " << a2_a->to_string() << ")"
                                << " and measures " << m1->to_string() << ", " << m2->to_string() << " | " << m->to_string()
                                << " | res_0_: " << res_0_.to_string()
                                << " | res_: " << res_.to_string() << std::endl;

                            if (res_0_ < res_0) {
                                res_0 = std::move(res_0_);
                            }
                        }
                    }

                    res_ += std::move(res_0);

                    if (res_ < res) {
                        res = std::move(res_);
                    }
                }
            }
        }
    }

    return res;
}