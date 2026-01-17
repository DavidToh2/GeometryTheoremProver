
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
        if (!point_to_cartesian.contains(p)) {
            point_to_cartesian[p] = {};
        }
        if (!point_to_cartesian_objs.contains(p)) {
            point_to_cartesian_objs[p] = {};
        }
        point_status[p] = ComputationStatus::UNCOMPUTED;
    }
    return n;
}

Generator<CartesianPoint> NumEngine::compute_free(Numeric* num) {
    co_yield Cartesian::random_point();
    co_return;
}
// TODO: Make these random generators better
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
Generator<CartesianPoint> NumEngine::compute_iso_triangle(Numeric* num) {
    double angle = NumUtils::urand(M_PI / 20, 9 * M_PI / 20);
    CartesianPoint b(0, 0);
    CartesianPoint a(1, std::tan(angle));
    CartesianPoint c(2, 0);
    Cartesian::random_affine(a, b, c);
    co_yield a;
    co_yield b;
    co_yield c;
    co_return;
}
Generator<CartesianPoint> NumEngine::compute_r_triangle(Numeric* num) {
    double angle = NumUtils::urand(M_PI / 20, 9 * M_PI / 20);
    CartesianPoint a(0, 0);
    CartesianPoint b(1, 0);
    CartesianPoint c(0, std::tan(angle));
    Cartesian::random_affine(a, b, c);
    co_yield a;
    co_yield b;
    co_yield c;
    co_return;
}
Generator<CartesianPoint> NumEngine::compute_riso_triangle(Numeric* num) {
    CartesianPoint a(0, 0);
    CartesianPoint b(1, 0);
    CartesianPoint c(0, 1);
    Cartesian::random_affine(a, b, c);
    co_yield a;
    co_yield b;
    co_yield c;
    co_return;
}
Generator<CartesianPoint> NumEngine::compute_equi_triangle(Numeric* num) {
    CartesianPoint a(0, 0);
    CartesianPoint b(1, 0);
    CartesianPoint c(0.5, std::sqrt(3) / 2);
    Cartesian::random_affine(a, b, c);
    co_yield a;
    co_yield b;
    co_yield c;
    co_return;
}
Generator<CartesianPoint> NumEngine::compute_r_triangle_p(Numeric* num) {
    CartesianPoint a = get_arg_cartesian(num, 0);
    CartesianPoint b = get_arg_cartesian(num, 1);
    // The vector b-a=(x,y) becomes (-y,x).
    co_yield a + Cartesian::perp_vec_p(a, b);
    co_return;
}
Generator<CartesianPoint> NumEngine::compute_r_triangle_n(Numeric* num) {
    CartesianPoint a = get_arg_cartesian(num, 0);
    CartesianPoint b = get_arg_cartesian(num, 1);
    // The vector b-a=(x,y) becomes (y,-x).
    co_yield a + Cartesian::perp_vec_n(a, b);
    co_return;
}
Generator<CartesianPoint> NumEngine::compute_equi_triangle_p(Numeric* num) {
    CartesianPoint b = get_arg_cartesian(num, 0);
    CartesianPoint c = get_arg_cartesian(num, 1);
    // The vector c-b=(x,y) becomes ((x-sqrt(3)y)/2, (sqrt(3)x+y)/2).
    CartesianPoint v = c - b;
    co_yield b + CartesianPoint((v.x - std::sqrt(3) * v.y) / 2,
                                (std::sqrt(3) * v.x + v.y) / 2);  
    co_return;
}
Generator<CartesianPoint> NumEngine::compute_quadrilateral(Numeric* num) {
    double angle1 = NumUtils::urand(M_PI / 10, 9 * M_PI / 10);
    double angle2 = NumUtils::urand(M_PI / 10, 9 * M_PI / 10);
    CartesianPoint a(0, 0);
    CartesianPoint b(1, 0);
    CartesianPoint c0(-std::tan(M_PI_2 - angle1), 1);
    CartesianPoint d0(std::tan(M_PI_2 - angle2), 1);
    double hc, hd;
    if (angle1 + angle2 < M_PI) {
        double max_height = std::tan(M_PI_2 - angle1) + std::tan(M_PI_2 - angle2);
        max_height = std::min(max_height, 1.75);
        hc = NumUtils::urand(0.25, max_height);
        hd = NumUtils::urand(0.25, max_height);
    } else {
        hc = NumUtils::urand(0.25, 1.75);
        hd = NumUtils::urand(0.25, 1.75);
    }
    CartesianPoint c = b + c0 * hc;
    CartesianPoint d = a + d0 * hd;
    Cartesian::random_affine(a, b, c, d);
    co_yield a;
    co_yield b;
    co_yield c;
    co_yield d;
    co_return;
}
Generator<CartesianPoint> NumEngine::compute_rectangle(Numeric* num) {
    double width = NumUtils::urand(0.5, 1.5);
    double height = NumUtils::urand(0.5, 1.5);
    CartesianPoint a(0, 0);
    CartesianPoint b(width, 0);
    CartesianPoint c(width, height);
    CartesianPoint d(0, height);
    Cartesian::random_affine(a, b, c, d);
    co_yield a;
    co_yield b;
    co_yield c;
    co_yield d;
    co_return;
}
Generator<CartesianPoint> NumEngine::compute_square(Numeric* num) {
    double side = NumUtils::urand(0.5, 1.5);
    CartesianPoint a(0, 0);
    CartesianPoint b(side, 0);
    CartesianPoint c(side, side);
    CartesianPoint d(0, side);
    Cartesian::random_affine(a, b, c, d);
    co_yield a;
    co_yield b;
    co_yield c;
    co_yield d;
    co_return;
}
Generator<CartesianPoint> NumEngine::compute_square_off_p(Numeric* num) {
    CartesianPoint a = get_arg_cartesian(num, 0);
    CartesianPoint b = get_arg_cartesian(num, 1);
    CartesianPoint v = b - a;
    CartesianPoint v0(-v.y, v.x);
    co_yield b + v0;
    co_yield a + v0;
    co_return;
}
Generator<CartesianPoint> NumEngine::compute_trapezoid(Numeric* num) {
    double base1 = NumUtils::urand(0.5, 1.5);
    double base2 = NumUtils::urand(0.5, 1.5);
    double e = NumUtils::urand(-0.5, 0.5);
    double height = NumUtils::urand(0.5, 1.5);
    CartesianPoint a(0, 0);
    CartesianPoint b(base1, 0);
    CartesianPoint c((base1 + base2) / 2 + e, height);
    CartesianPoint d((base1 - base2) / 2 + e, height);
    Cartesian::random_affine(a, b, c, d);
    co_yield a;
    co_yield b;
    co_yield c;
    co_yield d;
    co_return;
}
Generator<CartesianPoint> NumEngine::compute_eq_trapezoid(Numeric* num) {
    double base1 = NumUtils::urand(0.5, 1.5);
    double base2 = NumUtils::urand(0.5, base1);
    double height = NumUtils::urand(0.5, 1.5);
    CartesianPoint a(0, 0);
    CartesianPoint b(base1, 0);
    CartesianPoint c((base1 + base2) / 2, height);
    CartesianPoint d((base1 - base2) / 2, height);
    Cartesian::random_affine(a, b, c, d);
    co_yield a;
    co_yield b;
    co_yield c;
    co_yield d;
    co_return;
}


Generator<CartesianLine> NumEngine::compute_line(Numeric* num) {
    CartesianPoint a = get_arg_cartesian(num, 0);
    CartesianPoint b = get_arg_cartesian(num, 1);
    co_yield CartesianLine(a, b);
    co_return;
}
Generator<CartesianLine> NumEngine::compute_line_at_angle(Numeric* num) {
    CartesianPoint a = get_arg_cartesian(num, 0);
    CartesianPoint b = get_arg_cartesian(num, 1);
    CartesianPoint c = get_arg_cartesian(num, 2);
    CartesianPoint d = get_arg_cartesian(num, 3);
    CartesianPoint e = get_arg_cartesian(num, 4);
    double angle = Cartesian::angle_between(c, d, e);
    double base_angle = Cartesian::angle_of(a, b);
    double new_angle = base_angle + angle;
    CartesianPoint dir(std::cos(new_angle), std::sin(new_angle));
    // TODO: This should be a CartesianRay
    co_yield CartesianLine(a, a + dir);
    co_return;
}
Generator<CartesianLine> NumEngine::compute_line_bisect(Numeric* num) {
    CartesianPoint a = get_arg_cartesian(num, 0);
    CartesianPoint b = get_arg_cartesian(num, 1);
    co_yield Cartesian::perp_bisect(a, b);
    co_return;
}
Generator<CartesianLine> NumEngine::compute_line_para(Numeric* num) {
    CartesianPoint a = get_arg_cartesian(num, 0);
    CartesianPoint b = get_arg_cartesian(num, 1);
    CartesianPoint c = get_arg_cartesian(num, 2);
    co_yield Cartesian::para_line(c, CartesianLine(a, b));
    co_return;
}
Generator<CartesianLine> NumEngine::compute_line_perp(Numeric* num) {
    CartesianPoint a = get_arg_cartesian(num, 0);
    CartesianPoint b = get_arg_cartesian(num, 1);
    CartesianPoint c = get_arg_cartesian(num, 2);
    co_yield Cartesian::perp_line(c, CartesianLine(a, b));
    co_return;
}


Generator<CartesianCircle> NumEngine::compute_circle(Numeric* num) {
    CartesianPoint c = get_arg_cartesian(num, 0);
    CartesianPoint r1 = get_arg_cartesian(num, 1);
    CartesianPoint r2 = get_arg_cartesian(num, 2);
    double r = Cartesian::distance(r1, r2);
    co_yield CartesianCircle(c, r);
    co_return;
}
Generator<CartesianCircle> NumEngine::compute_circum(Numeric* num) {
    CartesianPoint a = get_arg_cartesian(num, 0);
    CartesianPoint b = get_arg_cartesian(num, 1);
    CartesianPoint c = get_arg_cartesian(num, 2);
    co_yield CartesianCircle(a, b, c);
    co_return;
}
Generator<CartesianCircle> NumEngine::compute_diameter(Numeric* num) {
    CartesianPoint a = get_arg_cartesian(num, 0);
    CartesianPoint b = get_arg_cartesian(num, 1);
    CartesianPoint c = Cartesian::midpoint(a, b);
    double r = Cartesian::distance(a, b) / 2;
    co_yield CartesianCircle(c, r);
    co_return;
}


Generator<CartesianPoint> NumEngine::compute_midpoint(Numeric* num) {
    CartesianPoint a = get_arg_cartesian(num, 0);
    CartesianPoint b = get_arg_cartesian(num, 1);
    co_yield Cartesian::midpoint(a, b);
    co_return;
}
Generator<CartesianPoint> NumEngine::compute_trisegment(Numeric* num) {
    CartesianPoint a = get_arg_cartesian(num, 0);
    CartesianPoint b = get_arg_cartesian(num, 1);
    CartesianPoint v = (b - a) / 3;
    co_yield a + v;
    co_yield b - v;
    co_return;
}
Generator<CartesianPoint> NumEngine::compute_mirror(Numeric* num) {
    CartesianPoint a = get_arg_cartesian(num, 0);
    CartesianPoint b = get_arg_cartesian(num, 1);
    co_yield (2 * b - a);
    co_return;
}
Generator<CartesianPoint> NumEngine::compute_reflect(Numeric* num) {
    CartesianPoint a = get_arg_cartesian(num, 0);
    CartesianPoint b = get_arg_cartesian(num, 1);
    CartesianPoint c = get_arg_cartesian(num, 2);
    co_yield Cartesian::reflect(a, CartesianLine(b, c));
    co_return;
}


Generator<CartesianPoint> NumEngine::compute_angle_eq2(Numeric* num) {
    CartesianPoint a = get_arg_cartesian(num, 0);
    CartesianPoint b = get_arg_cartesian(num, 1);
    CartesianPoint c = get_arg_cartesian(num, 2);
    double k = NumUtils::urand(0, 1);
    if (k < 0.5) {
        // map k=[0, 0.5] to k=[0.1, 0.9]
        k = 0.1 + k * 1.6;
    } else {
        // map k=[0.5, 1] to k=[1.1, 2.9]
        k = 1.1 + (k - 0.5) * 3.6;
    }
    double k1 = k * Cartesian::distance(b, c) / Cartesian::distance(b, a);
    CartesianPoint e = b + k * (c - b);
    CartesianPoint f = b + k1 * (a - b);
    auto x = Cartesian::intersect(CartesianLine(a, e), CartesianLine(b, f));
    if (x) co_yield x.value();
    co_return;
}
Generator<CartesianCircle> NumEngine::compute_angle_eq3(Numeric* num) {
    CartesianPoint a = get_arg_cartesian(num, 0);
    CartesianPoint b = get_arg_cartesian(num, 1);
    CartesianPoint d = get_arg_cartesian(num, 2);
    CartesianPoint e = get_arg_cartesian(num, 3);
    CartesianPoint f = get_arg_cartesian(num, 4);
    double angle = Cartesian::angle_between(e, d, f);
    double D = Cartesian::distance(a, b);
    double r = D / std::sin(angle) / 2;
    double h = D * std::tan(M_PI_2 - angle) / 2;
    // The circle c has to be located so that the points a, b, c are oriented in anticlockwise order
    CartesianPoint c = Cartesian::midpoint(a, b) + h * Cartesian::perp_vec_p(a, b);
    co_yield CartesianCircle(c, r);
    co_return;
}
Generator<CartesianLine> NumEngine::compute_angle_mirror(Numeric* num) {
    CartesianPoint a = get_arg_cartesian(num, 0);
    CartesianPoint b = get_arg_cartesian(num, 1);
    CartesianPoint c = get_arg_cartesian(num, 2);
    // TODO: Should be CartesianRay
    co_yield CartesianLine(b, Cartesian::reflect(a, CartesianLine(b, c)));
    co_return;
}
Generator<CartesianLine> NumEngine::compute_angle_bisect(Numeric* num) {
    CartesianPoint a = get_arg_cartesian(num, 0);
    CartesianPoint b = get_arg_cartesian(num, 1);
    CartesianPoint c = get_arg_cartesian(num, 2);
    CartesianPoint a0 = b + (a - b) * Cartesian::distance(b, c) / Cartesian::distance(b, a);
    co_yield CartesianLine(b, Cartesian::midpoint(a0, c));
    co_return;
}
Generator<CartesianLine> NumEngine::compute_angle_trisect(Numeric* num) {
    CartesianPoint a = get_arg_cartesian(num, 0);
    CartesianPoint b = get_arg_cartesian(num, 1);
    CartesianPoint c = get_arg_cartesian(num, 2);
    double angle1 = Cartesian::angle_of(b, a);
    double angle2 = Cartesian::angle_of(b, c);

    bool swap = false;
    if (angle1 > angle2) {
        std::swap(angle1, angle2);
        swap = !swap;
    }
    if (angle2 - angle1 > M_PI) {
        std::swap(angle1, angle2);
        angle2 += 2 * M_PI;
        swap = !swap;
    }

    double angle_x = angle1 + (angle2 - angle1) / 3;
    double angle_y = angle1 + 2 * (angle2 - angle1) / 3;
    CartesianPoint x = b + Cartesian::from_polar(1, angle_x);
    CartesianPoint y = b + Cartesian::from_polar(1, angle_y);

    if (swap) {
        co_yield CartesianLine(b, y);
        co_yield CartesianLine(b, x);
    } else {
        co_yield CartesianLine(b, x);
        co_yield CartesianLine(b, y);
    }
    co_return;
}


Generator<CartesianPoint> NumEngine::compute_tangents(Numeric* num) {
    CartesianPoint a = get_arg_cartesian(num, 0);
    CartesianPoint o = get_arg_cartesian(num, 1);
    CartesianPoint b = get_arg_cartesian(num, 2);
    double r = Cartesian::distance(o, b);
    double angle = std::acos(r / Cartesian::distance(a, o)); // return value in [0, pi/2]
    double base_angle = Cartesian::angle_of(o, a);
    double angle1 = base_angle + angle, angle2 = base_angle - angle;
    co_yield o + Cartesian::from_polar(r, angle1);
    co_yield o + Cartesian::from_polar(r, angle2);
    co_return;
}
Generator<CartesianPoint> NumEngine::compute_common_tangent(Numeric* num) {
    CartesianPoint o = get_arg_cartesian(num, 0);
    CartesianPoint a = get_arg_cartesian(num, 1);
    CartesianPoint i = get_arg_cartesian(num, 2);
    CartesianPoint b = get_arg_cartesian(num, 3);
    double r1 = Cartesian::distance(o, a);
    double r2 = Cartesian::distance(i, b);

    if (NumUtils::is_close(r1, r2)) {
        CartesianLine oi(o, i);
        CartesianCircle oa(o, r1);
        CartesianLine o_diam = Cartesian::perp_line(o, oi);
        auto [_x, _z] = Cartesian::intersect(o_diam, oa);
        if (_x) {
            CartesianPoint x = _x.value();
            co_yield x;
            co_yield x + (i - o);
        }
        co_return;
    }

    // Ensure that circle o (oa = r1) has the larger radius
    bool swap = r2 > r1;
    if (swap) {
        std::swap(o, i);
        std::swap(a, b);
        std::swap(r1, r2);
    }

    // Point q is the external homothetic center of the two circles
    CartesianPoint q = o + (i - o) * r1 / (r1 - r2);
    double angle = std::acos(r1 / Cartesian::distance(o, q)); // return value in [0, pi/2]
    double base_angle = Cartesian::angle_of(q, o);
    double angle1 = base_angle + angle;
    if (swap) {
        co_yield o + Cartesian::from_polar(r1, angle1);
        co_yield i + Cartesian::from_polar(r2, angle1);
    } else {
        co_yield o + Cartesian::from_polar(r1, angle1);
        co_yield i + Cartesian::from_polar(r2, angle1);
    }
    co_return;
}
Generator<CartesianPoint> NumEngine::compute_common_tangent2(Numeric* num) {
    CartesianPoint o = get_arg_cartesian(num, 0);
    CartesianPoint a = get_arg_cartesian(num, 1);
    CartesianPoint i = get_arg_cartesian(num, 2);
    CartesianPoint b = get_arg_cartesian(num, 3);
    double r1 = Cartesian::distance(o, a);
    double r2 = Cartesian::distance(i, b);

    if (NumUtils::is_close(r1, r2)) {
        CartesianLine oi(o, i);
        CartesianCircle oa(o, r1);
        CartesianLine o_diam = Cartesian::perp_line(o, oi);
        auto [_x, _z] = Cartesian::intersect(o_diam, oa);
        if (_x && _z) {
            CartesianPoint x = _x.value();
            CartesianPoint z = _z.value();
            co_yield x;
            co_yield x + (i - o);
            co_yield z;
            co_yield z + (i - o);
        }
        co_return;
    }

    // Ensure that circle o (oa = r1) has the larger radius
    bool swap = r2 > r1;
    if (swap) {
        std::swap(o, i);
        std::swap(a, b);
        std::swap(r1, r2);
    }

    // Point q is the external homothetic center of the two circles
    CartesianPoint q = o + (i - o) * r1 / (r1 - r2);
    double angle = std::acos(r1 / Cartesian::distance(o, q)); // return value in [0, pi/2]
    double base_angle = Cartesian::angle_of(q, o);
    double angle1 = base_angle + angle, angle2 = base_angle - angle;
    if (swap) {
        co_yield o + Cartesian::from_polar(r1, angle2);
        co_yield i + Cartesian::from_polar(r2, angle2);
        co_yield o + Cartesian::from_polar(r1, angle1);
        co_yield i + Cartesian::from_polar(r2, angle1);
    } else {
        co_yield o + Cartesian::from_polar(r1, angle1);
        co_yield i + Cartesian::from_polar(r2, angle1);
        co_yield o + Cartesian::from_polar(r1, angle2);
        co_yield i + Cartesian::from_polar(r2, angle2);
    }
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




void NumEngine::update_resolved_radius(CartesianPoint cp) {
    double dist = Cartesian::distance(cp, sum_of_resolved_points / num_resolved);
    max_dist = std::max(max_dist, dist);
    num_resolved++;
    sum_of_resolved_points += cp;
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
            if (point_to_cartesian_objs[p].size() > 0) {
                // We should only reach here if we only had one CartesianObject to work with
                // Simply pick a random point off this CartesianObject
                point_to_cartesian[p].emplace_back(
                    Cartesian::get_random_point(
                        point_to_cartesian_objs[p][0],
                        sum_of_resolved_points / num_resolved,
                        max_dist
                    )
                );
                update_resolved_radius(point_to_cartesian[p][0]);
                return true;
            }
            throw NumericsInternalError("Insufficient information to resolve point " + p->name + " to Cartesian coordinates.");
        case 1:
            update_resolved_radius(point_to_cartesian[p][0]);
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
            recently_resolved.emplace_back(p);
        }
    }
    return res;
}



void NumEngine::reset_computation() {
    for (auto & [p, _] : point_status) {
        point_to_cartesian[p].clear();
        point_to_cartesian_objs[p].clear();
        point_status[p] = ComputationStatus::UNCOMPUTED;
    }
    recently_resolved.clear();
}
void NumEngine::reset_problem() {
    numerics.clear();
    point_to_cartesian.clear();
    point_to_cartesian_objs.clear();
    point_status.clear();
}