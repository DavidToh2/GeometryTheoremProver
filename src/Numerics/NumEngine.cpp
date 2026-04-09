
#include <cassert>
#include <cmath>
#include <iostream>

#include "NumEngine.hh"
#include "Common/Exceptions.hh"
#include "Numerics/Cartesian.hh"
#include "Numerics/Numerics.hh"

#include "Common/Debug.hh"
#if DEBUG_NUMENGINE
    #define LOG(x) do {std::cout << x << std::endl;} while(0)
#else 
    #define LOG(x)
#endif

Numeric* NumEngine::insert_numeric(std::unique_ptr<Numeric> &&num) {
    numerics.emplace_back(std::move(num));
    Numeric* n = numerics.back().get();
    for (Point* p : n->args) {
        all_points.insert(p);
    }
    for (Point* p : n->outs) {
        all_points.insert(p);
    }
    return n;
}

void NumEngine::compute_free(NumInstance& inst, Numeric* num) {
    CartesianPoint a = Cartesian::random_point();
    inst.record_out(num, 0, a);
    inst.record_params({a.x, a.y});
}
// TODO: Make these random generators better
void NumEngine::compute_segment(NumInstance& inst, Numeric* num) {
    CartesianPoint a = Cartesian::random_point(), b = Cartesian::random_point();
    inst.record_out(num, 0, a);
    inst.record_out(num, 1, b);
    inst.record_params({a.x, a.y, b.x, b.y});

}
void NumEngine::compute_triangle(NumInstance& inst, Numeric* num) {
    CartesianPoint a = Cartesian::random_point(), b = Cartesian::random_point(), c = Cartesian::random_point();
    inst.record_out(num, 0, a);
    inst.record_out(num, 1, b);
    inst.record_out(num, 2, c);
    inst.record_params({a.x, a.y, b.x, b.y, c.x, c.y});
}
void NumEngine::compute_iso_triangle(NumInstance& inst, Numeric* num) {
    double angle = NumUtils::urand(M_PI / 20, 9 * M_PI / 20);
    CartesianPoint b(0, 0);
    CartesianPoint a(1, std::tan(angle));
    CartesianPoint c(2, 0);
    auto [aff_a, aff_sc, aff_shx, aff_shy] = Cartesian::random_affine(a, b, c);
    inst.record_out(num, 0, a);
    inst.record_out(num, 1, b);
    inst.record_out(num, 2, c);
    inst.record_params({angle, aff_a, aff_sc, aff_shx, aff_shy});
}
void NumEngine::compute_r_triangle(NumInstance& inst, Numeric* num) {
    double angle = NumUtils::urand(M_PI / 20, 9 * M_PI / 20);
    CartesianPoint a(0, 0);
    CartesianPoint b(1, 0);
    CartesianPoint c(0, std::tan(angle));
    auto [aff_a, aff_sc, aff_shx, aff_shy] = Cartesian::random_affine(a, b, c);
    inst.record_out(num, 0, a);
    inst.record_out(num, 1, b);
    inst.record_out(num, 2, c);
    inst.record_params({angle, aff_a, aff_sc, aff_shx, aff_shy});
}
void NumEngine::compute_riso_triangle(NumInstance& inst, Numeric* num) {
    CartesianPoint a(0, 0);
    CartesianPoint b(1, 0);
    CartesianPoint c(0, 1);
    auto [aff_a, aff_sc, aff_shx, aff_shy] = Cartesian::random_affine(a, b, c);
    inst.record_out(num, 0, a);
    inst.record_out(num, 1, b);
    inst.record_out(num, 2, c);
    inst.record_params({aff_a, aff_sc, aff_shx, aff_shy});
}
void NumEngine::compute_equi_triangle(NumInstance& inst, Numeric* num) {
    CartesianPoint a(0, 0);
    CartesianPoint b(1, 0);
    CartesianPoint c(0.5, std::sqrt(3) / 2);
    auto [aff_a, aff_sc, aff_shx, aff_shy] = Cartesian::random_affine(a, b, c);
    inst.record_out(num, 0, a);
    inst.record_out(num, 1, b);
    inst.record_out(num, 2, c);
    inst.record_params({aff_a, aff_sc, aff_shx, aff_shy});
}
void NumEngine::compute_r_triangle_p(NumInstance& inst, Numeric* num) {
    for (CartesianPoint a : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint b : inst.get_arg_coords(num, 1)) {  
            // The vector b-a=(x,y) becomes (-y,x).
            inst.record_out(num, 0, a + Cartesian::perp_vec_p(a, b));
        }
    }
}
void NumEngine::compute_r_triangle_n(NumInstance& inst, Numeric* num) {
    for (CartesianPoint a : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint b : inst.get_arg_coords(num, 1)) {  
            // The vector b-a=(x,y) becomes (y,-x).
            inst.record_out(num, 0, a + Cartesian::perp_vec_n(a, b));
        }
    }
}
void NumEngine::compute_equi_triangle_p(NumInstance& inst, Numeric* num) {
    for (CartesianPoint b : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint c : inst.get_arg_coords(num, 1)) {  
            // The vector c-b=(x,y) becomes ((x-sqrt(3)y)/2, (sqrt(3)x+y)/2).
            CartesianPoint v = c - b;
            inst.record_out(num, 0, b + CartesianPoint((v.x - std::sqrt(3) * v.y) / 2,
                                                        (std::sqrt(3) * v.x + v.y) / 2));
        }
    }
}
void NumEngine::compute_quadrilateral(NumInstance& inst, Numeric* num) {
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
    auto [aff_a, aff_sc, aff_shx, aff_shy] = Cartesian::random_affine(a, b, c, d);
    inst.record_out(num, 0, a);
    inst.record_out(num, 1, b);
    inst.record_out(num, 2, c);
    inst.record_out(num, 3, d);
    inst.record_params({angle1, angle2, hc, hd, aff_a, aff_sc, aff_shx, aff_shy});
}
void NumEngine::compute_cyclic_quad(NumInstance& inst, Numeric* num) {
    double angle_c = NumUtils::urand(3 * M_PI / 10, M_PI);
    double angle_b = NumUtils::urand(M_PI / 10, angle_c - M_PI / 10);
    double angle_d = NumUtils::urand(M_PI / 10, 19 * M_PI / 10 - angle_c);
    CartesianPoint a = Cartesian::from_polar(1, 0);
    CartesianPoint b = Cartesian::from_polar(1, angle_b);
    CartesianPoint c = Cartesian::from_polar(1, angle_c);
    CartesianPoint d = Cartesian::from_polar(1, -angle_d);
    auto [aff_a, aff_sc, aff_shx, aff_shy] = Cartesian::random_affine(a, b, c, d);
    inst.record_out(num, 0, a);
    inst.record_out(num, 1, b);
    inst.record_out(num, 2, c);
    inst.record_out(num, 3, d);
    inst.record_params({angle_b, angle_c, angle_d, aff_a, aff_sc, aff_shx, aff_shy});
}
void NumEngine::compute_rectangle(NumInstance& inst, Numeric* num) {
    double width = NumUtils::urand(0.5, 1.5);
    double height = NumUtils::urand(0.5, 1.5);
    CartesianPoint a(0, 0);
    CartesianPoint b(width, 0);
    CartesianPoint c(width, height);
    CartesianPoint d(0, height);
    auto [aff_a, aff_sc, aff_shx, aff_shy] = Cartesian::random_affine(a, b, c, d);
    inst.record_out(num, 0, a);
    inst.record_out(num, 1, b);
    inst.record_out(num, 2, c);
    inst.record_out(num, 3, d);
    inst.record_params({width, height, aff_a, aff_sc, aff_shx, aff_shy});
}
void NumEngine::compute_square(NumInstance& inst, Numeric* num) {
    double side = NumUtils::urand(0.5, 1.5);
    CartesianPoint a(0, 0);
    CartesianPoint b(side, 0);
    CartesianPoint c(side, side);
    CartesianPoint d(0, side);
    auto [aff_a, aff_sc, aff_shx, aff_shy] = Cartesian::random_affine(a, b, c, d);
    inst.record_out(num, 0, a);
    inst.record_out(num, 1, b);
    inst.record_out(num, 2, c);
    inst.record_out(num, 3, d);
    inst.record_params({side, aff_a, aff_sc, aff_shx, aff_shy});
}
void NumEngine::compute_square_off_p(NumInstance& inst, Numeric* num) {
    for (CartesianPoint a : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint b : inst.get_arg_coords(num, 1)) {
            CartesianPoint v = b - a;
            CartesianPoint v0(-v.y, v.x);
            inst.record_out(num, 0, b + v0);
            inst.record_out(num, 1, a + v0);
        }
    }
}
void NumEngine::compute_trapezoid(NumInstance& inst, Numeric* num) {
    double base1 = NumUtils::urand(0.5, 1.5);
    double base2 = NumUtils::urand(0.5, 1.5);
    double e = NumUtils::urand(-0.5, 0.5);
    double height = NumUtils::urand(0.5, 1.5);
    CartesianPoint a(0, 0);
    CartesianPoint b(base1, 0);
    CartesianPoint c((base1 + base2) / 2 + e, height);
    CartesianPoint d((base1 - base2) / 2 + e, height);
    auto [aff_a, aff_sc, aff_shx, aff_shy] = Cartesian::random_affine(a, b, c, d);
    inst.record_out(num, 0, a);
    inst.record_out(num, 1, b);
    inst.record_out(num, 2, c);
    inst.record_out(num, 3, d);
    inst.record_params({base2/base1, e/base1, height/base1, aff_a, aff_sc, aff_shx, aff_shy});
}
void NumEngine::compute_eq_trapezoid(NumInstance& inst, Numeric* num) {
    double base1 = NumUtils::urand(0.25, 0.75);
    double base2 = NumUtils::urand(0.25, 0.75);
    double height = NumUtils::urand(0.5, 1.5);
    CartesianPoint a(-base1, 0);
    CartesianPoint b(base1, 0);
    CartesianPoint c(-base2, height);
    CartesianPoint d(base2, height);
    auto [aff_a, aff_sc, aff_shx, aff_shy] = Cartesian::random_affine(a, b, c, d);
    inst.record_out(num, 0, a);
    inst.record_out(num, 1, b);
    inst.record_out(num, 2, c);
    inst.record_out(num, 3, d);
    inst.record_params({base2/base1, height/base1, aff_a, aff_sc, aff_shx, aff_shy});
}


void NumEngine::compute_line(NumInstance& inst, Numeric* num) {
    for (CartesianPoint a : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint b : inst.get_arg_coords(num, 1)) {
            inst.record_out(num, 0, CartesianLine(a, b));
        }
    }
}
void NumEngine::compute_line_at_angle(NumInstance& inst, Numeric* num) {
    for (CartesianPoint a : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint b : inst.get_arg_coords(num, 1)) {
            for (CartesianPoint c : inst.get_arg_coords(num, 2)) {
                for (CartesianPoint d : inst.get_arg_coords(num, 3)) {
                    for (CartesianPoint e : inst.get_arg_coords(num, 4)) {
                        double angle = Cartesian::angle_between(c, d, e);
                        double base_angle = Cartesian::angle_of(a, b);
                        double new_angle = base_angle - angle - M_PI_2; // subtract M_PI_2 to convert to polar angles
                        CartesianPoint dir(std::cos(new_angle), std::sin(new_angle));
                        inst.record_out(num, 0, CartesianRay(a, a + dir));
                    }
                }
            }
        }
    }
}
void NumEngine::compute_line_bisect(NumInstance& inst, Numeric* num) {
    for (CartesianPoint a : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint b : inst.get_arg_coords(num, 1)) {
            inst.record_out(num, 0, Cartesian::perp_bisect(a, b));
        }
    }
}
void NumEngine::compute_line_para(NumInstance& inst, Numeric* num) {
    for (CartesianPoint a : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint b : inst.get_arg_coords(num, 1)) {
            for (CartesianPoint c : inst.get_arg_coords(num, 2)) {
                inst.record_out(num, 0, Cartesian::para_line(a, CartesianLine(b, c)));
            }
        }
    }
}
void NumEngine::compute_line_perp(NumInstance& inst, Numeric* num) {
    for (CartesianPoint a : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint b : inst.get_arg_coords(num, 1)) {
            for (CartesianPoint c : inst.get_arg_coords(num, 2)) {
                inst.record_out(num, 0, Cartesian::perp_line(a, CartesianLine(b, c)));
            }
        }
    }
}
void NumEngine::compute_ray(NumInstance& inst, Numeric* num) {
    for (CartesianPoint a : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint b : inst.get_arg_coords(num, 1)) {
            inst.record_out(num, 0, CartesianRay(a, b));
        }
    }
}

void NumEngine::compute_circle(NumInstance& inst, Numeric* num) {
    for (CartesianPoint c : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint r1 : inst.get_arg_coords(num, 1)) {
            for (CartesianPoint r2 : inst.get_arg_coords(num, 2)) {
                double r = Cartesian::distance(r1, r2);
                inst.record_out(num, 0, CartesianCircle(c, r));
            }
        }
    }
}
void NumEngine::compute_circum(NumInstance& inst, Numeric* num) {
    for (CartesianPoint a : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint b : inst.get_arg_coords(num, 1)) {
            for (CartesianPoint c : inst.get_arg_coords(num, 2)) {
                inst.record_out(num, 0, CartesianCircle(a, b, c));
            }
        }
    }
}
void NumEngine::compute_diameter(NumInstance& inst, Numeric* num) {
    for (CartesianPoint a : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint b : inst.get_arg_coords(num, 1)) {
            CartesianPoint c = Cartesian::midpoint(a, b);
            double r = Cartesian::distance(a, b) / 2;
            inst.record_out(num, 0, CartesianCircle(c, r));
        }
    }
}


void NumEngine::compute_midpoint(NumInstance& inst, Numeric* num) {
    for (CartesianPoint a : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint b : inst.get_arg_coords(num, 1)) {
            inst.record_out(num, 0, Cartesian::midpoint(a, b));
        }
    }
}
void NumEngine::compute_trisegment(NumInstance& inst, Numeric* num) {
    for (CartesianPoint a : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint b : inst.get_arg_coords(num, 1)) {
            CartesianPoint v = (b - a) / 3;
            inst.record_out(num, 0, a + v);
            inst.record_out(num, 1, b - v);
        }
    }
}
void NumEngine::compute_mirror(NumInstance& inst, Numeric* num) {
    for (CartesianPoint a : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint b : inst.get_arg_coords(num, 1)) {
            inst.record_out(num, 0, 2 * b - a);
        }
    }
}
void NumEngine::compute_reflect(NumInstance& inst, Numeric* num) {
    for (CartesianPoint a : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint b : inst.get_arg_coords(num, 1)) {
            for (CartesianPoint c : inst.get_arg_coords(num, 2)) {
                inst.record_out(num, 0, Cartesian::reflect(a, CartesianLine(b, c)));
            }
        }
    }
}


void NumEngine::compute_angle_eq2(NumInstance& inst, Numeric* num) {
    double ks = NumUtils::urand(0, 1);
    for (CartesianPoint a : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint b : inst.get_arg_coords(num, 1)) {
            for (CartesianPoint c : inst.get_arg_coords(num, 2)) {
                double kl = ks;

                double d_ab = Cartesian::distance2(a, b);
                double d_bc = Cartesian::distance2(b, c);
                bool ab_geq_bc = (d_ab > d_bc);
                /* Our objective is to generate points D, E on lines AB, BC respectively, then intersect
                AE, CD to produce X.
                Either D is on *segment* AB and E is on *segment* BC simultaneously (k<0.5), or D and E are
                simultaneously on their respective rays (k>0.5). */
                if (ks < 0.5) {
                    // map ks=[0, 0.5] to ks=[0.1, 0.9]
                    // ks applies to the shorter segment of AB, BC
                    ks = 0.1 + ks * 1.6;
                    if (ab_geq_bc) {
                        kl = (d_bc / d_ab) * ks;
                    } else {
                        kl = (d_ab / d_bc) * ks;
                    }
                } else {
                    // map kl=[0.5, 1] to kl=[1.1, 2.9]
                    // kl applies to the longer segment of AB, BC
                    kl = 1.1 + (kl - 0.5) * 3.6;
                    if (ab_geq_bc) {
                        ks = (d_ab / d_bc) * kl;
                    } else {
                        ks = (d_bc / d_ab) * kl;
                    }
                }
                
                CartesianPoint d;
                CartesianPoint e;
                if (ab_geq_bc) {
                    d = b + (a-b) * kl;
                    e = b + (c-b) * ks;
                } else {
                    d = b + (a-b) * ks;
                    e = b + (c-b) * kl;
                }

                auto x = Cartesian::intersect(CartesianLine(a, e), CartesianLine(c, d));
                if (x) {
                    inst.record_out(num, 0, x.value());
                    inst.record_params({ks});
                }
            }
        }
    }
}
void NumEngine::compute_angle_eq3(NumInstance& inst, Numeric* num) {
    for (CartesianPoint a : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint b : inst.get_arg_coords(num, 1)) {
            for (CartesianPoint d : inst.get_arg_coords(num, 2)) {
                for (CartesianPoint e : inst.get_arg_coords(num, 3)) {
                    for (CartesianPoint f : inst.get_arg_coords(num, 4)) {
                        double angle = Cartesian::angle_between(d, e, f);

                        /* If the directed angle <(DE, EF) is anticlockwise, then <(AX, XB) should be as well
                        In other words, if D, E, F are in clockwise order, then A, X, B should also be in clockwise order
                        The position of the circle center C is thus determined by the following:
                        - If <(DE, EF) is anticlockwise and acute: A, B, C should be in anticlockwise order
                        - If <(DE, EF) is anticlockwise and obtuse: A, B, C should be in clockwise order
                        - If <(DE, EF) is clockwise and acute: A, B, C should be in clockwise order
                        - If <(DE, EF) is clockwise and obtuse: A, B, C should be in anticlockwise order 
                        (Ngl I think I just duplicated the functionality of std::tan2. But oh wells that's low priority too)*/

                        // indicator for position of C: 1 means anticlockwise, -1 means clockwise
                        double sgn = angle > 0 ? 
                            (angle < M_PI_2 ? 1.0 : -1.0) :
                            (angle > -M_PI_2 ? -1.0 : 1.0);

                        // Normalise the angle for trigonometric calculations
                        angle = std::abs(angle);
                        if (angle > M_PI_2) angle = M_PI - angle;

                        double D = Cartesian::distance(a, b);
                        double r = D / std::sin(angle) / 2;
                        double h = D * std::tan(M_PI_2 - angle) / 2;
                        // The circle c has to be located so that the points a, b, c are oriented in anticlockwise order
                        CartesianPoint c = Cartesian::midpoint(a, b) + h * sgn * Cartesian::perp_vec_p(a, b) / D;
                        inst.record_out(num, 0, CartesianCircle(c, r));
                    }
                }
            }
        }
    }
}
void NumEngine::compute_angle_mirror(NumInstance& inst, Numeric* num) {
    for (CartesianPoint a : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint b : inst.get_arg_coords(num, 1)) {
            for (CartesianPoint c : inst.get_arg_coords(num, 2)) {
                inst.record_out(num, 0, CartesianRay(b, Cartesian::reflect(a, CartesianLine(b, c))));
            }
        }
    }
}
void NumEngine::compute_angle_bisect(NumInstance& inst, Numeric* num) {
    for (CartesianPoint a : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint b : inst.get_arg_coords(num, 1)) {
            for (CartesianPoint c : inst.get_arg_coords(num, 2)) {
                CartesianPoint a0 = b + (a - b) * Cartesian::distance(b, c) / Cartesian::distance(b, a);
                inst.record_out(num, 0, CartesianRay(b, Cartesian::midpoint(a0, c)));
            }
        }
    }
}
void NumEngine::compute_angle_exbisect(NumInstance& inst, Numeric* num) {
    for (CartesianPoint a : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint b : inst.get_arg_coords(num, 1)) {
            for (CartesianPoint c : inst.get_arg_coords(num, 2)) {
                CartesianPoint a0 = b + (a - b) * Cartesian::distance(b, c) / Cartesian::distance(b, a);
                inst.record_out(num, 0, Cartesian::para_line(b, CartesianLine(a0, c)));
            }
        }
    }
}
void NumEngine::compute_angle_trisect(NumInstance& inst, Numeric* num) {
    for (CartesianPoint a : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint b : inst.get_arg_coords(num, 1)) {
            for (CartesianPoint c : inst.get_arg_coords(num, 2)) {
                double angle1 = Cartesian::angle_of(b, a);
                double angle2 = Cartesian::angle_of(b, c);

                /* Do some swapping nonsense so that angle2 - angle1 in [0, pi] */
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
                CartesianPoint x = b + Cartesian::from_polar(1, angle_x - M_PI_2);
                CartesianPoint y = b + Cartesian::from_polar(1, angle_y - M_PI_2);

                if (swap) {
                    inst.record_out(num, 0, CartesianRay(b, y));
                    inst.record_out(num, 1, CartesianRay(b, x));
                } else {
                    inst.record_out(num, 0, CartesianRay(b, x));
                    inst.record_out(num, 1, CartesianRay(b, y));
                }
            }
        }
    }
    
}


void NumEngine::compute_line_tangent(NumInstance& inst, Numeric* num) {
    for (CartesianPoint a : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint b : inst.get_arg_coords(num, 1)) {
            for (CartesianPoint c : inst.get_arg_coords(num, 2)) {
                double aa = a.norm2();
                double bb = b.norm2();
                double cc = c.norm2();
                double d = a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y);
                double e = aa * (c.y - b.y) + bb * (a.y - c.y) + cc * (b.y - a.y);
                double f = aa * (b.x - c.x) + bb * (c.x - a.x) + cc * (a.x - b.x);
                double ab = a.x * b.y - a.y * b.x;
                double bc = b.x * c.y - b.y * c.x;
                double ca = c.x * a.y - c.y * a.x;
                double g = aa * bc + bb * ca + cc * ab;
                inst.record_out(num, 0, CartesianLine(
                    2 * d * a.x + e,
                    2 * d * a.y + f,
                    - g - d * aa
                ));
            }
        }
    }
}
void NumEngine::compute_tangents(NumInstance& inst, Numeric* num) {
    for (CartesianPoint a : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint o : inst.get_arg_coords(num, 1)) {
            for (CartesianPoint b : inst.get_arg_coords(num, 2)) {
                double r = Cartesian::distance(o, b);
                double _rat = r / Cartesian::distance(a, o);
                if (std::abs(_rat) > 1.0 - TOL) continue;   // no tangents as point is in circle
                double angle = std::acos(_rat); // return value in [0, pi/2]
                
                double base_angle = Cartesian::angle_of(o, a);
                double angle1 = base_angle + angle, angle2 = base_angle - angle;
                inst.record_out(num, 0, o + Cartesian::from_polar(r, angle1 - M_PI_2));
                inst.record_out(num, 1, o + Cartesian::from_polar(r, angle2 - M_PI_2));
            }
        }
    }
}
void NumEngine::compute_common_tangent(NumInstance& inst, Numeric* num) {
    for (CartesianPoint o : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint a : inst.get_arg_coords(num, 1)) {
            for (CartesianPoint i : inst.get_arg_coords(num, 2)) {
                for (CartesianPoint b : inst.get_arg_coords(num, 3)) {
                    double r1 = Cartesian::distance(o, a);
                    double r2 = Cartesian::distance(i, b);

                    if (NumUtils::is_close(r1, r2)) {
                        CartesianLine oi(o, i);
                        CartesianCircle oa(o, r1);
                        CartesianLine o_diam = Cartesian::perp_line(o, oi);
                        auto [_x, _z] = Cartesian::intersect(o_diam, oa);
                        if (_x) {
                            CartesianPoint x = _x.value();
                            inst.record_out(num, 0, x);
                            inst.record_out(num, 1, x + (i - o));
                        }
                        continue;
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
                    double _rat = r1 / Cartesian::distance(o, q);
                    if (std::abs(_rat) > 1.0 - TOL) continue;
                    double angle = std::acos(_rat); // return value in [0, pi/2]
                    double base_angle = Cartesian::angle_of(o, q);
                    double angle1 = base_angle + angle - M_PI_2;
                    if (swap) {
                        inst.record_out(num, 0, i + Cartesian::from_polar(r2, angle1));
                        inst.record_out(num, 1, o + Cartesian::from_polar(r1, angle1));
                    } else {
                        inst.record_out(num, 0, o + Cartesian::from_polar(r1, angle1));
                        inst.record_out(num, 1, i + Cartesian::from_polar(r2, angle1));
                    }
                }
            }
        }
    }
}
void NumEngine::compute_common_tangent2(NumInstance& inst, Numeric* num) {
    for (CartesianPoint o : inst.get_arg_coords(num, 0)) {
        for (CartesianPoint a : inst.get_arg_coords(num, 1)) {
            for (CartesianPoint i : inst.get_arg_coords(num, 2)) {
                for (CartesianPoint b : inst.get_arg_coords(num, 3)) {
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
                            inst.record_out(num, 0, x);
                            inst.record_out(num, 1, x + (i - o));
                            inst.record_out(num, 2, z);
                            inst.record_out(num, 3, z + (i - o));
                        }
                        continue;
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
                    double _rat = r1 / Cartesian::distance(o, q);
                    if (std::abs(_rat) > 1.0 - TOL) continue;
                    double angle = std::acos(_rat); // return value in [0, pi/2]
                    double base_angle = Cartesian::angle_of(o, q);
                    double angle1 = base_angle + angle - M_PI_2, angle2 = base_angle - angle - M_PI_2;
                    if (swap) {
                        inst.record_out(num, 0, i + Cartesian::from_polar(r2, angle1));
                        inst.record_out(num, 1, o + Cartesian::from_polar(r1, angle1));
                        inst.record_out(num, 2, i + Cartesian::from_polar(r2, angle2));
                        inst.record_out(num, 3, o + Cartesian::from_polar(r1, angle2));
                    } else {
                        inst.record_out(num, 0, o + Cartesian::from_polar(r1, angle1));
                        inst.record_out(num, 1, i + Cartesian::from_polar(r2, angle1));
                        inst.record_out(num, 2, o + Cartesian::from_polar(r1, angle2));
                        inst.record_out(num, 3, i + Cartesian::from_polar(r2, angle2));
                    }
                }
            }
        }
    }
}





void NumEngine::get_operation_order() {
    std::map<Point*, NumInstance::ComputationStatus> point_status;
    for (Point* p : all_points) {
        point_status[p] = NumInstance::ComputationStatus::UNCOMPUTED;
    }

    bool resolution_needed = false;
    int i = 0;

    for (auto it = numerics.begin(); it != numerics.end(); ++it) {
        Numeric* num = it->get();

        for (Point* p : num->args) {
            if (point_status[p] < NumInstance::ComputationStatus::RESOLVED) {
                point_status[p] = NumInstance::ComputationStatus::TO_RESOLVE;
                resolution_needed = true;
            }
        }

        if (resolution_needed) {
            order_of_ops.emplace_back(i);
            i = 0;
            for (auto& [p, status] : point_status) {
                if (status == NumInstance::ComputationStatus::TO_RESOLVE) {
                    status = NumInstance::ComputationStatus::RESOLVED;
                    order_of_resolution.emplace_back(p);
                    i++;
                }
            }
            order_of_ops.emplace_back(i);
            i = 0; resolution_needed = false;
        }

        for (Point* p : num->outs) {
            point_status[p] = NumInstance::ComputationStatus::COMPUTING;
        }
        i++;
    }

    order_of_ops.emplace_back(i);
    i = 0;
    for (auto& [p, status] : point_status) {
        if (status <= NumInstance::ComputationStatus::TO_RESOLVE) {
            status = NumInstance::ComputationStatus::RESOLVED;
            order_of_resolution.emplace_back(p);
            i++;
        }
    }
    order_of_ops.emplace_back(i);

    assert(order_of_ops.size() % 2 == 0);
    assert(order_of_resolution.size() == all_points.size());
}






void NumEngine::compute_one(NumInstance& inst, Numeric* num) {

    num_t name = num->name;

    int i = 0, l = num->outs.size();
    if (compute_function_map_point.contains(name)) {
        (this->*compute_function_map_point[name])(inst, num);

    } else if (compute_function_map_line.contains(name)) {
        inst.next_outs(num);
        (this->*compute_function_map_line[name])(inst, num);

    } else if (compute_function_map_ray.contains(name)) {
        inst.next_outs(num);
        (this->*compute_function_map_ray[name])(inst, num);

    } else if (compute_function_map_circle.contains(name)) {
        inst.next_outs(num);
        (this->*compute_function_map_circle[name])(inst, num);

    } else {
        throw NumericsInternalError("NumEngine::compute_one(): No compute function for numeric " + num->to_string());
    }
}

void NumEngine::resolve_one(NumInstance &inst, Point* p) {

    /* Step 1: Generate candidate CartesianPoints by intersecting CartesianObjects from different Numerics
    Note: We prioritise new intersection points */
    switch(inst.point_to_cartesian_objs.at(p).size()) {
        case 0: {
            // do nothing
        }
        break;
        case 1: {
            if (inst.point_to_cartesian_objs[p][0].empty()) {
                break;
            }
            // We only have one Numeric worth of CartesianObjects to work with
            // Simply pick random points off these CartesianObjects using the same random parameter
            auto [pts, rand] = Cartesian::get_random_points(
                inst.point_to_cartesian_objs[p][0],
                inst.centroid_of_resolved_points,
                inst.max_dist
            );
            inst.point_to_coords[p].insert(inst.point_to_coords[p].end(), pts.begin(), pts.end());
            inst.record_params({rand});
        }
        break;
        default: {
            for (auto it1 = inst.point_to_cartesian_objs[p].begin(); it1 != inst.point_to_cartesian_objs[p].end(); ++it1) {
                for (auto it2 = std::next(it1); it2 != inst.point_to_cartesian_objs[p].end(); ++it2) {
                    for (CartesianObject obj1 : *it1) {
                        for (CartesianObject obj2 : *it2) {
                            auto intersect_gen = Cartesian::intersect(obj1, obj2);
                            bool new_coord_found = false;
                            std::vector<CartesianPoint> existing_coords;

                            /* If any of the intersection coords are new, then only add the new coords
                            Only if none of the coords are new do we add the already existing coords */
                            while (intersect_gen) {
                                CartesianPoint pt = intersect_gen();
                                if (inst.check_against_existing_point_numerics(pt)) {
                                    existing_coords.emplace_back(pt);
                                } else {
                                    inst.point_to_coords[p].emplace_back(pt);
                                    new_coord_found = true;
                                }
                            }

                            if (!new_coord_found) {
                                for (CartesianPoint& pt : existing_coords) {
                                    inst.point_to_coords[p].emplace_back(pt);
                                }
                            }
                        }
                    }
                }
            }
        }
        break;
    }

    /* Step 2: Aggregate CartesianPoint candidates, taking care to prioritise newly-seen coordinates */
    switch(inst.point_to_coords[p].size()) {
        case 0: 
            throw NumericsInternalError("NumEngine::resolve_one(): Insufficient information to resolve point " + p->name + " to Cartesian coordinates.");
        case 1: {
            // do nothing
            inst.point_coord_occurences[p].emplace_back(1);
        }
        break;
        default: {
            /* Aggregate the list of candidates in `point_to_coords[p]`, counting each candidate's no. of occurences
            Here, `new_candidates` stores candidate coordinates which have yet to appear, while `existing_candidates`
            stores candidate coordinates which have already appeared.
            We will prioritise new candidates over existing ones. */
            std::vector<std::pair<CartesianPoint, int>> new_candidates;
            std::vector<std::pair<CartesianPoint, int>> existing_candidates;

            for (auto it = inst.point_to_coords[p].begin(); it != inst.point_to_coords[p].end(); ) {
                CartesianPoint cp = *it;
                bool found = false;
                for (auto& [avg, i] : existing_candidates) {
                    if (CartesianPoint::is_close(cp, avg)) {
                        avg = (avg * i + cp) / (i + 1);
                        i += 1;
                        found = true;
                        break;
                    }
                }
                for (auto& [avg, i] : new_candidates) {
                    if (CartesianPoint::is_close(cp, avg)) {
                        avg = (avg * i + cp) / (i + 1);
                        i += 1;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    if (inst.check_against_existing_point_numerics(cp)) {
                        existing_candidates.emplace_back(cp, 1); 
                    } else {
                        new_candidates.emplace_back(cp, 1);
                    }
                }
                it = inst.point_to_coords[p].erase(it);
            }

            assert(inst.point_to_coords[p].empty());
            if (existing_candidates.empty() && new_candidates.empty()) {
                throw GGraphInternalError("NumEngine::resolve_one(): No viable candidate numeric for point " + p->to_string());
            }

            int num_candidates = new_candidates.size();

            if (new_candidates.empty()) {
                /* No new candidates - fall back to existing candidates */
                num_candidates = existing_candidates.size();

                for (auto it = existing_candidates.begin(); it != existing_candidates.end(); ) {
                    inst.point_to_coords[p].emplace_back(it->first);
                    inst.point_coord_occurences[p].emplace_back(it->second);
                    
                    it = existing_candidates.erase(it);
                }
                
            } else {
                /* New candidates available - discard existing ones */
                for (auto it = new_candidates.begin(); it != new_candidates.end(); ) {
                    inst.point_to_coords[p].emplace_back(it->first);
                    inst.point_coord_occurences[p].emplace_back(it->second);

                    it = new_candidates.erase(it);
                }
            }
        }
        break;
    }
    assert(inst.point_to_coords[p].size() == inst.point_coord_occurences[p].size());

    inst.update_resolved_centroid_and_radius(inst.get_most_likely_coords(p));
}

bool NumEngine::compute(NumInstance& inst) {
    auto it = numerics.begin();
    auto jt = order_of_resolution.begin();

    for (int i=0; i<order_of_ops.size(); i++) {
        if (i % 2 == 0) {
            // Compute some numerics
            for (int j=0; j<order_of_ops[i]; j++) {
                Numeric* num = it->get();
                inst.record_computation_status(num);
                compute_one(inst, num);
                assert(it != numerics.end());
                ++it;
            }
        } else {
            // Resolve some points
            for (int j=0; j<order_of_ops[i]; j++) {
                Point* p = *jt;
                resolve_one(inst, p);
                inst.record_resolution_status(p);
                assert(jt != order_of_resolution.end());
                ++jt;
            }
        }
    }

    assert(it == numerics.end());
    assert(jt == order_of_resolution.end());

    inst.compute_loss();

    return inst.is_valid();
}




void NumEngine::draw() {
    get_operation_order();

    NumInstance inst(all_points);

    if (compute(inst)) {
        final_inst = inst;
    } else {
        std::cout << "Invalid instance with loss " << inst.loss << std::endl;
    }
}




void NumEngine::reset_problem() {
    numerics.clear();
    all_points.clear();

    order_of_resolution.clear();
    order_of_ops.clear();

    instances.clear();
    final_inst = NumInstance();
}