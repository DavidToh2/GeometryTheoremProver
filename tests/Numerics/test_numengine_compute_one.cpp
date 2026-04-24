
#include <doctest.h>
#include <memory>
#include <iostream>

#include "Common/NumUtils.hh"
#include "Numerics/Cartesian.hh"
#include "Numerics/NumEngine.hh"
#include "Numerics/NumInstance.hh"
#include "Numerics/Numerics.hh"

TEST_SUITE("NumEngine Computation") {
    TEST_CASE("Free Polygon computations") {
        std::map<std::string, std::unique_ptr<Point>> point_map;
        for (char pt_ = 'a'; pt_ <= 'd'; pt_++) {
            std::string pt(1, pt_);
            point_map[pt] = std::make_unique<Point>(pt);
        }
        NumEngine ne;
        NumInstance inst(point_map);

        Point* a = point_map["a"].get();
        Point* b = point_map["b"].get();
        Point* c = point_map["c"].get();
        Point* d = point_map["d"].get();

        SUBCASE("iso_triangle") {
            Numeric num("a b c = iso_triangle", point_map);
            ne.compute_iso_triangle(inst, &num);
            CartesianPoint pa = inst.__get_coord(a);
            CartesianPoint pb = inst.__get_coord(b);
            CartesianPoint pc = inst.__get_coord(c);
            CHECK(NumUtils::is_close(Cartesian::distance(pa, pb), Cartesian::distance(pa, pc)));
        }
        SUBCASE("r_triangle") {
            Numeric num("a b c = r_triangle", point_map);
            ne.compute_r_triangle(inst, &num);
            CartesianPoint pa = inst.__get_coord(a);
            CartesianPoint pb = inst.__get_coord(b);
            CartesianPoint pc = inst.__get_coord(c);
            double ab = Cartesian::distance2(pa, pb);
            double ac = Cartesian::distance2(pa, pc);
            double bc = Cartesian::distance2(pb, pc);
            CHECK(NumUtils::is_close(ab + ac, bc));
            double ang = Cartesian::angle_between(pb, pa, pc);
            CHECK(NumUtils::is_close(std::abs(ang), M_PI_2));
        }
        SUBCASE("riso_triangle") {
            Numeric num("a b c = riso_triangle", point_map);
            ne.compute_riso_triangle(inst, &num);
            CartesianPoint pa = inst.__get_coord(a);
            CartesianPoint pb = inst.__get_coord(b);
            CartesianPoint pc = inst.__get_coord(c);
            double ab = Cartesian::distance2(pa, pb);
            double ac = Cartesian::distance2(pa, pc);
            double bc = Cartesian::distance2(pb, pc);
            CHECK(NumUtils::is_close(ab, ac));
            CHECK(NumUtils::is_close(ab + ac, bc));
            double ang = Cartesian::angle_between(pb, pa, pc);
            CHECK(NumUtils::is_close(std::abs(ang), M_PI_2));
            double ang2 = Cartesian::angle_between(pa, pb, pc);
            CHECK(NumUtils::is_close(std::abs(ang2), M_PI_4));
        }
        SUBCASE("equi_triangle") {
            Numeric num("a b c = equi_triangle", point_map);
            ne.compute_equi_triangle(inst, &num);
            CartesianPoint pa = inst.__get_coord(a);
            CartesianPoint pb = inst.__get_coord(b);
            CartesianPoint pc = inst.__get_coord(c);
            double ab = Cartesian::distance2(pa, pb);
            double ac = Cartesian::distance2(pa, pc);
            double bc = Cartesian::distance2(pb, pc);
            CHECK(NumUtils::is_close(ab, ac));
            CHECK(NumUtils::is_close(ac, bc));
            double ang_abc = Cartesian::angle_between(pa, pb, pc);
            double ang_bca = Cartesian::angle_between(pb, pc, pa);
            double ang_cab = Cartesian::angle_between(pc, pa, pb);
            CHECK(NumUtils::is_close(std::abs(ang_abc), M_PI / 3));
            CHECK(NumUtils::is_close(std::abs(ang_bca), M_PI / 3));
            CHECK(NumUtils::is_close(std::abs(ang_cab), M_PI / 3));
        }
        SUBCASE("rectangle") {
            Numeric num("a b c d = rectangle", point_map);
            ne.compute_rectangle(inst, &num);
            CartesianPoint pa = inst.__get_coord(a);
            CartesianPoint pb = inst.__get_coord(b);
            CartesianPoint pc = inst.__get_coord(c);
            CartesianPoint pd = inst.__get_coord(d);
            double ab = Cartesian::distance2(pa, pb);
            double bc = Cartesian::distance2(pb, pc);
            double cd = Cartesian::distance2(pc, pd);
            double da = Cartesian::distance2(pd, pa);
            CHECK(NumUtils::is_close(ab, cd));
            CHECK(NumUtils::is_close(bc, da));
            double ac = Cartesian::distance2(pa, pc);
            double bd = Cartesian::distance2(pb, pd);
            CHECK(NumUtils::is_close(ab + bc, ac));
            CHECK(NumUtils::is_close(ac, bd));
            double ang_of_ab = Cartesian::angle_of(pa, pb);
            double ang_of_cd = Cartesian::angle_of(pc, pd);
            CHECK(NumUtils::is_close(std::abs(ang_of_ab - ang_of_cd), M_PI));
            double ang_bac = Cartesian::angle_between(pb, pa, pc);
            double ang_dca = Cartesian::angle_between(pd, pc, pa);
            CHECK(NumUtils::is_close(ang_bac, ang_dca));
        }
        SUBCASE("square") {
            Numeric num("a b c d = square", point_map);
            ne.compute_square(inst, &num);
            CartesianPoint pa = inst.__get_coord(a);
            CartesianPoint pb = inst.__get_coord(b);
            CartesianPoint pc = inst.__get_coord(c);
            CartesianPoint pd = inst.__get_coord(d);
            double ab = Cartesian::distance2(pa, pb);
            double bc = Cartesian::distance2(pb, pc);
            double cd = Cartesian::distance2(pc, pd);
            CHECK(NumUtils::is_close(ab, bc));
            double bd = Cartesian::distance2(pb, pd);
            CHECK(NumUtils::is_close(bc + cd, bd));
            double ang_bcd = Cartesian::angle_between(pb, pc, pd);
            double ang_cda = Cartesian::angle_between(pc, pd, pa);
            CHECK(NumUtils::is_close(std::abs(ang_bcd), M_PI_2));
            CHECK(NumUtils::is_close(std::abs(ang_cda), M_PI_2));
            double ang_bac = Cartesian::angle_between(pb, pa, pc);
            double ang_dca = Cartesian::angle_between(pd, pc, pa);
            CHECK(NumUtils::is_close(std::abs(ang_bac), M_PI_4));
            CHECK(NumUtils::is_close(std::abs(ang_dca), M_PI_4));
        }
        SUBCASE("trapezoid") {
            Numeric num("a b c d = trapezoid", point_map);
            ne.compute_trapezoid(inst, &num);
            CartesianPoint pa = inst.__get_coord(a);
            CartesianPoint pb = inst.__get_coord(b);
            CartesianPoint pc = inst.__get_coord(c);
            CartesianPoint pd = inst.__get_coord(d);
            CartesianLine lab(pa, pb);
            CartesianLine lcd(pc, pd);
            CHECK(NumUtils::is_close(Cartesian::angle_of(lab), Cartesian::angle_of(lcd)));
        }
        SUBCASE("eq_trapezoid") {
            Numeric num("a b c d = eq_trapezoid", point_map);
            ne.compute_eq_trapezoid(inst, &num);
            CartesianPoint pa = inst.__get_coord(a);
            CartesianPoint pb = inst.__get_coord(b);
            CartesianPoint pc = inst.__get_coord(c);
            CartesianPoint pd = inst.__get_coord(d);
            double ad = Cartesian::distance(pa, pd);
            double bc = Cartesian::distance(pb, pc);
            CHECK(NumUtils::is_close(ad, bc));
            CartesianLine lab(pa, pb);
            CartesianLine lcd(pc, pd);
            CHECK(NumUtils::is_close(Cartesian::angle_of(lab), Cartesian::angle_of(lcd)));
        }
    }
    TEST_CASE("Line and Ray computations") {
        std::map<std::string, std::unique_ptr<Point>> point_map;
        for (char pt_ = 'a'; pt_ <= 'f'; pt_++) {
            std::string pt(1, pt_);
            point_map[pt] = std::make_unique<Point>(pt);
        }
        Point* a = point_map["a"].get();
        Point* b = point_map["b"].get();
        Point* c = point_map["c"].get();
        Point* d = point_map["d"].get();
        Point* e = point_map["e"].get();
        Point* f = point_map["f"].get();
        NumEngine ne;
        NumInstance inst(point_map);
        
        Numeric num_("a b = segment", point_map);
        ne.compute_segment(inst, &num_);

        SUBCASE("line") {
            Numeric num("c = line a b", point_map);
            inst.next_outs(&num);
            ne.compute_line(inst, &num);
            CartesianLine line = std::get<CartesianLine>(inst.__get_obj(c));
            CartesianPoint pa = inst.__get_coord(a);
            CartesianPoint pb = inst.__get_coord(b);
            CHECK(line.contains(pa));
            CHECK(line.contains(pb));
        }
        SUBCASE("line_at_angle") {
            Numeric num0("c d e = triangle", point_map);
            ne.compute_triangle(inst, &num0);

            Numeric num("f = line_at_angle a b c d e", point_map);
            inst.next_outs(&num);
            ne.compute_line_at_angle(inst, &num);
            CartesianLine line = std::get<CartesianLine>(inst.__get_obj(f));
            CartesianPoint pf = Cartesian::get_random_point_on_line(line, 0.8);
            CartesianPoint pa = inst.__get_coord(a);
            CartesianPoint pb = inst.__get_coord(b);
            CartesianPoint pc = inst.__get_coord(c);
            CartesianPoint pd = inst.__get_coord(d);
            CartesianPoint pe = inst.__get_coord(e);

            double ang_fab = Cartesian::angle_between(pf, pa, pb);
            double ang_cde = Cartesian::angle_between(pc, pd, pe);
            CHECK((
                NumUtils::is_close(ang_fab, ang_cde)
                || NumUtils::is_close(std::abs(ang_fab - ang_cde), M_PI)
            ));
        }
        SUBCASE("line_bisect") {
            Numeric num("c = line_bisect a b", point_map);
            inst.next_outs(&num);
            ne.compute_line_bisect(inst, &num);
            CartesianLine bisect = std::get<CartesianLine>(inst.__get_obj(c));
            CartesianPoint pa = inst.__get_coord(a);
            CartesianPoint pb = inst.__get_coord(b);
            CartesianPoint mid = Cartesian::midpoint(pa, pb);
            CHECK(bisect.contains(mid));
            CartesianPoint rand = Cartesian::get_random_point_on_line(bisect, 0.8);
            double ang1 = Cartesian::angle_between(rand, pa, pb);;
            double ang2 = Cartesian::angle_between(pa, pb, rand);
            CHECK(NumUtils::is_close(ang1, ang2));
        }
        SUBCASE("line_para") {
            Numeric num0("c = free", point_map);
            ne.compute_free(inst, &num0);

            Numeric num("d = line_para c a b", point_map);
            inst.next_outs(&num);
            ne.compute_line_para(inst, &num);
            CartesianLine para = std::get<CartesianLine>(inst.__get_obj(d));
            CartesianPoint pc = inst.__get_coord(c);
            CartesianPoint pa = inst.__get_coord(a);
            CartesianPoint pb = inst.__get_coord(b);
            CHECK(para.contains(pc));
            double ang_ab = Cartesian::angle_of(CartesianLine(pa, pb));
            double ang_pc = Cartesian::angle_of(para);
            CHECK(NumUtils::is_close(ang_ab, ang_pc));
        }
        SUBCASE("line_perp") {
            Numeric num0("c = free", point_map);
            ne.compute_free(inst, &num0);

            Numeric num("d = line_perp c a b", point_map);
            inst.next_outs(&num);
            ne.compute_line_perp(inst, &num);
            CartesianLine perp = std::get<CartesianLine>(inst.__get_obj(d));
            CartesianPoint pc = inst.__get_coord(c);
            CartesianPoint pa = inst.__get_coord(a);
            CartesianPoint pb = inst.__get_coord(b);
            CHECK(perp.contains(pc));
            double ang_ab = Cartesian::angle_of(CartesianLine(pa, pb));
            double ang_pc = Cartesian::angle_of(perp);
            CHECK(NumUtils::is_close(std::abs(ang_ab - ang_pc), M_PI_2));
        }
        SUBCASE("ray") {
            Numeric num("c = ray a b", point_map);
            inst.next_outs(&num);
            ne.compute_ray(inst, &num);
            CartesianRay ray = std::get<CartesianRay>(inst.__get_obj(c));
            CartesianPoint pa = inst.__get_coord(a);
            CartesianPoint pb = inst.__get_coord(b);
            CHECK(ray.contains(pa));
            CHECK(ray.contains(pb));
            CHECK(ray.start == pa);
        }
    }
    TEST_CASE("Point computations") {
        std::map<std::string, std::unique_ptr<Point>> point_map;
        for (char pt_ = 'a'; pt_ <= 'f'; pt_++) {
            std::string pt(1, pt_);
            point_map[pt] = std::make_unique<Point>(pt);
        }
        Point* a = point_map["a"].get();
        Point* b = point_map["b"].get();
        Point* c = point_map["c"].get();
        Point* d = point_map["d"].get();
        Point* e = point_map["e"].get();
        Point* f = point_map["f"].get();
        NumEngine ne;
        NumInstance inst(point_map);

        Numeric num_("a b c = triangle", point_map);
        ne.compute_triangle(inst, &num_);

        SUBCASE("midpoint") {
            Numeric num("d = midpoint a b", point_map);
            inst.next_outs(&num);
            ne.compute_midpoint(inst, &num);
            CartesianPoint mid = inst.__get_coord(d);
            CartesianPoint pa = inst.__get_coord(a);
            CartesianPoint pb = inst.__get_coord(b);
            CHECK(Cartesian::midpoint(pa, pb) == mid);
        }
        SUBCASE("trisegment") {
            Numeric num("d e = trisegment a b", point_map);
            inst.next_outs(&num);
            ne.compute_trisegment(inst, &num);
            CartesianPoint pd = inst.__get_coord(d);
            CartesianPoint pe = inst.__get_coord(e);
            CartesianPoint pa = inst.__get_coord(a);
            CartesianPoint pb = inst.__get_coord(b);
            CHECK(pd == 2 * pa / 3 + pb / 3);
            CHECK(pe == pa / 3 + 2 * pb / 3);
        }
        SUBCASE("mirror") {
            Numeric num("c = mirror a b", point_map);
            inst.next_outs(&num);
            ne.compute_mirror(inst, &num);
            CartesianPoint pc = inst.__get_coord(c);
            CartesianPoint pa = inst.__get_coord(a);
            CartesianPoint pb = inst.__get_coord(b);
            CHECK(pc == 2 * pb - pa);
        }
        SUBCASE("reflect") {
            Numeric num("d = reflect c a b", point_map);
            inst.next_outs(&num);
            ne.compute_reflect(inst, &num);
            CartesianPoint pd = inst.__get_coord(d);
            CartesianPoint pc = inst.__get_coord(c);
            CartesianPoint pa = inst.__get_coord(a);
            CartesianPoint pb = inst.__get_coord(b);
            CHECK(NumUtils::is_close(Cartesian::distance(pc, pa), Cartesian::distance(pd, pa)));
            CHECK(NumUtils::is_close(Cartesian::distance(pc, pb), Cartesian::distance(pd, pb)));
            
            CartesianLine line_ab(pa, pb);
            CartesianLine line_cd(pc, pd);
            double ang = Cartesian::angle_between(line_ab, line_cd);
            CHECK(NumUtils::is_close(std::abs(ang), M_PI_2));
        }
    }
    TEST_CASE("Angle computations") {
        std::map<std::string, std::unique_ptr<Point>> point_map;
        for (char pt_ = 'a'; pt_ <= 'f'; pt_++) {
            std::string pt(1, pt_);
            point_map[pt] = std::make_unique<Point>(pt);
        }
        Point* a = point_map["a"].get();
        Point* b = point_map["b"].get();
        Point* c = point_map["c"].get();
        Point* d = point_map["d"].get();
        Point* e = point_map["e"].get();
        Point* f = point_map["f"].get();
        NumEngine ne;
        NumInstance inst(point_map);

        Numeric num_("a b c = triangle", point_map);
        ne.compute_triangle(inst, &num_);

        SUBCASE("angle_eq2") {
            // <(BA, AF) = <(FC, CB)
            Numeric num("f = angle_eq2 a b c", point_map);
            CartesianPoint pa = inst.__get_coord(a);
            CartesianPoint pb = inst.__get_coord(b);
            CartesianPoint pc = inst.__get_coord(c);
            for (int N=0; N<10; N++) {
                ne.compute_angle_eq2(inst, &num);
                CartesianPoint pf = inst.__get_coord(f);
                double ang_baf = Cartesian::angle_between(pb, pa, pf);
                double ang_fcb = Cartesian::angle_between(pf, pc, pb);
                CHECK(NumUtils::is_close(ang_baf, ang_fcb));
            }
        }
        SUBCASE("angle_eq3") {
            for (int N=0; N<10; N++) {
                Numeric num_("d e = segment", point_map);
                ne.compute_segment(inst, &num_);

                // <(DF, FE) = <(AB, BC)
                Numeric num("f = angle_eq3 d e a b c", point_map);

                inst.next_outs(&num);
                ne.compute_angle_eq3(inst, &num);

                CartesianCircle cf = std::get<CartesianCircle>(inst.__get_obj(f));
                CartesianPoint pd = inst.__get_coord(d);
                CartesianPoint pe = inst.__get_coord(e);
                CartesianPoint pa = inst.__get_coord(a);
                CartesianPoint pb = inst.__get_coord(b);
                CartesianPoint pc = inst.__get_coord(c);
                CHECK((cf.contains(pd) && cf.contains(pe)));

                CartesianPoint pf = Cartesian::get_random_point_on_circle(cf, 0.25 + 0.05*N);
                double ang_dfe = Cartesian::angle_between(pd, pf, pe);
                double ang_abc = Cartesian::angle_between(pa, pb, pc);
                CHECK((NumUtils::is_close(ang_dfe, ang_abc) || NumUtils::is_close(std::abs(ang_dfe - ang_abc), M_PI)));
            }
        }
        SUBCASE("angle_mirror") {
            // <(BA, BC) = <(BC, BD)
            Numeric num("d = angle_mirror a b c", point_map);
            CartesianPoint pa = inst.__get_coord(a);
            CartesianPoint pb = inst.__get_coord(b);
            CartesianPoint pc = inst.__get_coord(c);

            inst.next_outs(&num);
            ne.compute_angle_mirror(inst, &num);

            CartesianRay rd = std::get<CartesianRay>(inst.__get_obj(d));
            CartesianPoint pd = Cartesian::get_random_point_on_ray(rd, 0.8);
            double ang_abc = Cartesian::angle_between(pa, pb, pc);
            double ang_cbd = Cartesian::angle_between(pc, pb, pd);
            CHECK(NumUtils::is_close(ang_abc, ang_cbd));
        }
        SUBCASE("angle_bisect") {
            // bisect angle ABC
            Numeric num("d = angle_bisect a b c", point_map);
            CartesianPoint pa = inst.__get_coord(a);
            CartesianPoint pb = inst.__get_coord(b);
            CartesianPoint pc = inst.__get_coord(c);

            inst.next_outs(&num);
            ne.compute_angle_bisect(inst, &num);

            CartesianRay rd = std::get<CartesianRay>(inst.__get_obj(d));
            CartesianPoint pd = Cartesian::get_random_point_on_ray(rd, 0.8);
            double ang_abd = Cartesian::angle_between(pa, pb, pd);
            double ang_dbc = Cartesian::angle_between(pd, pb, pc);
            CHECK(NumUtils::is_close(ang_abd, ang_dbc));
            CHECK(std::abs(ang_abd) < M_PI_2);
        }
        SUBCASE("angle_exbisect") {
            Numeric num("d = angle_exbisect a b c", point_map);
            CartesianPoint pa = inst.__get_coord(a);
            CartesianPoint pb = inst.__get_coord(b);
            CartesianPoint pc = inst.__get_coord(c);

            inst.next_outs(&num);
            ne.compute_angle_exbisect(inst, &num);

            CartesianLine ld = std::get<CartesianLine>(inst.__get_obj(d));
            CartesianPoint pd = Cartesian::get_random_point_on_line(ld, 0.8);
            double ang_abd = Cartesian::angle_between(pa, pb, pd);
            double ang_dbc = Cartesian::angle_between(pd, pb, pc);
            CHECK(NumUtils::is_close(std::abs(ang_abd) + std::abs(ang_dbc), M_PI));
            CHECK(((std::abs(ang_abd) > M_PI_2) || (std::abs(ang_dbc) > M_PI_2)));
        }
        SUBCASE("angle_trisect") {
            Numeric num("d e = angle_trisect a b c", point_map);
            CartesianPoint pa = inst.__get_coord(a);
            CartesianPoint pb = inst.__get_coord(b);
            CartesianPoint pc = inst.__get_coord(c);

            inst.next_outs(&num);
            ne.compute_angle_trisect(inst, &num);

            CartesianRay rd = std::get<CartesianRay>(inst.__get_obj(d));
            CartesianRay re = std::get<CartesianRay>(inst.__get_obj(e));
            CartesianPoint pd = Cartesian::get_random_point_on_ray(rd, 0.8);
            CartesianPoint pe = Cartesian::get_random_point_on_ray(re, 0.8);
            double ang_abd = Cartesian::angle_between(pa, pb, pd);
            double ang_dbe = Cartesian::angle_between(pd, pb, pe);
            double ang_ebc = Cartesian::angle_between(pe, pb, pc);
            CHECK(NumUtils::is_close(ang_abd, ang_dbe));
            CHECK(NumUtils::is_close(ang_dbe, ang_ebc));
        }
    }
    TEST_CASE("Circle computations") {
        std::map<std::string, std::unique_ptr<Point>> point_map;
        for (char pt_ = 'a'; pt_ <= 'e'; pt_++) {
            std::string pt(1, pt_);
            point_map[pt] = std::make_unique<Point>(pt);
        }
        Point* a = point_map["a"].get();
        Point* b = point_map["b"].get();
        Point* c = point_map["c"].get();
        Point* d = point_map["d"].get();
        Point* e = point_map["e"].get();
        NumEngine ne;
        NumInstance inst(point_map);

        Numeric num_("a b c d = quadrilateral", point_map);
        ne.compute_quadrilateral(inst, &num_);

        SUBCASE("circle") {
            Numeric num("e = circle a b c", point_map);

            inst.next_outs(&num);
            ne.compute_circle(inst, &num);

            CartesianCircle circle = std::get<CartesianCircle>(inst.__get_obj(e));
            CartesianPoint pe = Cartesian::get_random_point_on_circle(circle, 1.2);
            CartesianPoint pa = inst.__get_coord(a);
            CartesianPoint pb = inst.__get_coord(b);
            CartesianPoint pc = inst.__get_coord(c);
            CHECK(circle.c == pa);
            CHECK(NumUtils::is_close(Cartesian::distance(pe, pa), circle.r));
            CHECK(NumUtils::is_close(Cartesian::distance(pb, pc), circle.r));
            CHECK(circle.contains(pe));
        }
        SUBCASE("circum") {
            Numeric num("e = circum a b c", point_map);

            inst.next_outs(&num);
            ne.compute_circum(inst, &num);

            CartesianCircle circum = std::get<CartesianCircle>(inst.__get_obj(e));
            CartesianPoint pa = inst.__get_coord(a);
            CartesianPoint pb = inst.__get_coord(b);
            CartesianPoint pc = inst.__get_coord(c);
            CHECK((circum.contains(pa) && circum.contains(pb) && circum.contains(pc)));

            CartesianPoint pe = Cartesian::get_random_point_on_circle(circum, 1.2);
            CartesianLine b_ab = Cartesian::perp_bisect(pa, pb);
            CartesianLine b_bc = Cartesian::perp_bisect(pb, pc);
            CartesianPoint center = Cartesian::intersect(b_ab, b_bc).value();
            CHECK(circum.c == center);

            double r_a = Cartesian::distance(center, pa);
            double r_b = Cartesian::distance(center, pb);
            double r_c = Cartesian::distance(center, pc);
            double r_e = Cartesian::distance(center, pe);
            CHECK((NumUtils::is_close(circum.r, r_a) && NumUtils::is_close(circum.r, r_b) 
                && NumUtils::is_close(circum.r, r_c) && NumUtils::is_close(circum.r, r_e)));
        }
        SUBCASE("diameter") {
            Numeric num("e = diameter a b", point_map);

            inst.next_outs(&num);
            ne.compute_diameter(inst, &num);

            CartesianCircle circ = std::get<CartesianCircle>(inst.__get_obj(e));
            CartesianPoint pa = inst.__get_coord(a);
            CartesianPoint pb = inst.__get_coord(b);
            CartesianPoint pe = Cartesian::get_random_point_on_circle(circ, 1.2);
            CHECK(circ.c == (pa + pb) / 2);
            CHECK(NumUtils::is_close(circ.r, Cartesian::distance(pa, pb) / 2));
            CHECK(circ.contains(pa));
            CHECK(circ.contains(pb));
            CHECK(NumUtils::is_close(std::abs(Cartesian::angle_between(pa, pe, pb)), M_PI_2));
        }
    }

    TEST_CASE("Tangent computations") {
        std::map<std::string, std::unique_ptr<Point>> point_map;
        for (char pt_ = 'a'; pt_ <= 'l'; pt_++) {
            std::string pt(1, pt_);
            point_map[pt] = std::make_unique<Point>(pt);
        }
        Point* a = point_map["a"].get();
        Point* b = point_map["b"].get();
        Point* c = point_map["c"].get();
        Point* d = point_map["d"].get();
        Point* e = point_map["e"].get();
        Point* f = point_map["f"].get();
        Point* g = point_map["g"].get();
        Point* h = point_map["h"].get();
        Point* i = point_map["i"].get();
        Point* j = point_map["j"].get();
        Point* k = point_map["k"].get();
        Point* l = point_map["l"].get();
        NumEngine ne;
        NumInstance inst(point_map);

        /* Circles (A, AB) and (C, CD) have two intersections. 
        Circle (E, EF) contains circle (G, GH). 
        Circles (A, AB) and (E, EF) have the same radius.
        The two groups of circles are pairwise outside each other. */
        inst.point_to_coords[a].push_back(CartesianPoint(-9.01084, 1.32536));
        inst.point_to_coords[b].push_back(CartesianPoint(-9.01084, 3.68154));
        inst.point_to_coords[c].push_back(CartesianPoint(-7.85923, -0.38221));
        inst.point_to_coords[d].push_back(CartesianPoint(-8.25634, 0.66351));
        inst.point_to_coords[e].push_back(CartesianPoint(-3.29247, 1.18637));
        inst.point_to_coords[f].push_back(CartesianPoint(-3.29247, -1.16981));
        inst.point_to_coords[g].push_back(CartesianPoint(-3.71605, 1.3386));
        inst.point_to_coords[h].push_back(CartesianPoint(-2.85565, 2.59611));
        
        CartesianPoint pa = inst.__get_coord(a);
        CartesianPoint pb = inst.__get_coord(b);
        CartesianPoint pc = inst.__get_coord(c);
        CartesianPoint pd = inst.__get_coord(d);
        CartesianPoint pe = inst.__get_coord(e);
        CartesianPoint pf = inst.__get_coord(f);
        CartesianPoint pg = inst.__get_coord(g);
        CartesianPoint ph = inst.__get_coord(h);

        CartesianCircle cab(pa, Cartesian::distance(pa, pb));
        CartesianCircle ccd(pc, Cartesian::distance(pc, pd));
        CartesianCircle cef(pe, Cartesian::distance(pe, pf));
        CartesianCircle cgh(pg, Cartesian::distance(pg, ph));

        REQUIRE(NumUtils::is_close(cab.r, cef.r));

        SUBCASE("line_tangent") {
            Numeric num1("i = line_tangent a c e", point_map);

            inst.next_outs(&num1);
            ne.compute_line_tangent(inst, &num1);

            CartesianLine line = std::get<CartesianLine>(inst.__get_obj(i));
            CHECK(line.contains(CartesianPoint(-7.7373372370592, -3.9196413474696)));

            Numeric num2("j = line_tangent c a e", point_map);

            inst.next_outs(&num2);
            ne.compute_line_tangent(inst, &num2);

            CartesianLine line2 = std::get<CartesianLine>(inst.__get_obj(j));

            Numeric num3("k = line_tangent e a c", point_map);

            inst.next_outs(&num3);
            ne.compute_line_tangent(inst, &num3);

            CartesianLine line3 = std::get<CartesianLine>(inst.__get_obj(k));

            CartesianPoint d = Cartesian::intersect(line2, line3).value();
            CHECK(d == CartesianPoint(-4.4717835709978, -2.8122963182687));
        }
        SUBCASE("tangents") {
            Numeric num("i j = tangents g a b", point_map);
            // AE, AF are tangent to the circle centered at B with radius BC

            inst.next_outs(&num);
            ne.compute_tangents(inst, &num);

            CartesianPoint pi = inst.__get_coord(i);
            CartesianPoint pj = inst.__get_coord(j);
            double r = cab.r;
            CHECK((NumUtils::is_close(Cartesian::distance(pa, pi), r) && 
                NumUtils::is_close(Cartesian::distance(pa, pj), r)));
            CHECK((NumUtils::is_close(std::abs(Cartesian::angle_between(pa, pi, pg)), M_PI_2) &&
                NumUtils::is_close(std::abs(Cartesian::angle_between(pa, pj, pg)), M_PI_2)));
            CHECK(NumUtils::is_close(Cartesian::distance(pg, pi), Cartesian::distance(pg, pj)));
            CHECK(NumUtils::is_close(Cartesian::angle_between(pi, pg, pa), -Cartesian::angle_between(pj, pg, pa)));
        }
        SUBCASE("tangents (point inside circle)") {
            Numeric num("i j = tangents c a b", point_map);

            inst.next_outs(&num);
            ne.compute_tangents(inst, &num);

            CHECK((
                inst.point_to_coords[i].empty() && inst.point_to_coords[j].empty()
            ));
        }
        SUBCASE("common_tangent") {
            Numeric num("i j = common_tangent a b c d", point_map);
            // IJ is a common tangent to the circles (A, AB) and (C, CD)

            inst.next_outs(&num);
            ne.compute_common_tangent(inst, &num);

            CartesianPoint pi = inst.__get_coord(i);
            CartesianPoint pj = inst.__get_coord(j);
            double rab = cab.r;
            double rcd = ccd.r;
            CHECK((NumUtils::is_close(Cartesian::distance(pa, pi), rab) && 
                NumUtils::is_close(Cartesian::distance(pc, pj), rcd)));
            CHECK((NumUtils::is_close(std::abs(Cartesian::angle_between(pa, pi, pj)), M_PI_2) &&
                NumUtils::is_close(std::abs(Cartesian::angle_between(pc, pj, pi)), M_PI_2)));
        }
        SUBCASE("common_tangent (same radius)") {
            Numeric num("i j = common_tangent a b e f", point_map);

            inst.next_outs(&num);
            ne.compute_common_tangent(inst, &num);

            CartesianPoint pi = inst.__get_coord(i);
            CartesianPoint pj = inst.__get_coord(j);
            double rab = cab.r;
            double ref = cef.r;
            CHECK((NumUtils::is_close(Cartesian::distance(pa, pi), ref) && 
                NumUtils::is_close(Cartesian::distance(pe, pj), rab)));
            CHECK((NumUtils::is_close(std::abs(Cartesian::angle_between(pa, pi, pj)), M_PI_2) &&
                NumUtils::is_close(std::abs(Cartesian::angle_between(pe, pj, pi)), M_PI_2)));
            CHECK((NumUtils::is_close(Cartesian::angle_of(pa, pe), Cartesian::angle_of(pi, pj))));
        }
        SUBCASE("common_tangent2") {
            Numeric num("i j k l = common_tangent c d e f", point_map);
            // IJ and KL are the two common tangents to the circles (C, CD) and (E, EF)

            inst.next_outs(&num);
            ne.compute_common_tangent2(inst, &num);

            CartesianPoint pi = inst.__get_coord(i);
            CartesianPoint pj = inst.__get_coord(j);
            CartesianPoint pk = inst.__get_coord(k);
            CartesianPoint pl = inst.__get_coord(l);
            double rcd = ccd.r;
            double ref = cef.r;
            CHECK((pi != pj && pk != pl));
            CHECK((NumUtils::is_close(Cartesian::distance(pc, pi), rcd) &&
                NumUtils::is_close(Cartesian::distance(pe, pj), ref) &&
                NumUtils::is_close(Cartesian::distance(pc, pk), rcd) &&
                NumUtils::is_close(Cartesian::distance(pe, pl), ref)));
            CHECK((NumUtils::is_close(std::abs(Cartesian::angle_between(pc, pi, pj)), M_PI_2) &&
                NumUtils::is_close(std::abs(Cartesian::angle_between(pe, pj, pi)), M_PI_2) &&
                NumUtils::is_close(std::abs(Cartesian::angle_between(pc, pk, pl)), M_PI_2) &&
                NumUtils::is_close(std::abs(Cartesian::angle_between(pe, pl, pk)), M_PI_2)));
        }
        SUBCASE("common_tangent2 (circle inside circle)") {
            Numeric num("i j k l = common_tangent2 e f g h", point_map);
            // IJ and KL are the two common tangents to the circles (E, EF) and (G, GH)

            inst.next_outs(&num);
            ne.compute_common_tangent2(inst, &num);

            CHECK((inst.point_to_coords[i].empty() && inst.point_to_coords[j].empty() &&
                inst.point_to_coords[k].empty() && inst.point_to_coords[l].empty()));
        }
    }
}