
#include <doctest.h>
#include <iomanip>
#include <iostream>
#include <memory>

#include "Common/NumUtils.hh"
#include "Numerics/Cartesian.hh"
#include "Numerics/NumEngine.hh"
#include "Numerics/Numerics.hh"

TEST_SUITE("NumEngine Computation") {
    TEST_CASE("Free Polygon computations") {
        std::map<std::string, std::unique_ptr<Point>> point_map;
        for (char pt_ = 'a'; pt_ <= 'd'; pt_++) {
            std::string pt(1, pt_);
            point_map[pt] = std::make_unique<Point>(pt);
        }
        NumEngine ne;

        SUBCASE("iso_triangle") {
            Numeric num("a b c = iso_triangle", point_map);
            auto gen = ne.compute_iso_triangle(&num);
            CartesianPoint pa = gen();
            CartesianPoint pb = gen();
            CartesianPoint pc = gen();
            CHECK(NumUtils::is_close(Cartesian::distance(pa, pb), Cartesian::distance(pa, pc)));
        }
        SUBCASE("r_triangle") {
            Numeric num("a b c = r_triangle", point_map);
            auto gen = ne.compute_r_triangle(&num);
            CartesianPoint pa = gen();
            CartesianPoint pb = gen();
            CartesianPoint pc = gen();
            double ab = Cartesian::distance2(pa, pb);
            double ac = Cartesian::distance2(pa, pc);
            double bc = Cartesian::distance2(pb, pc);
            CHECK(NumUtils::is_close(ab + ac, bc));
            double ang = Cartesian::angle_between(pb, pa, pc);
            CHECK(NumUtils::is_close(std::abs(ang), M_PI_2));
        }
        SUBCASE("riso_triangle") {
            Numeric num("a b c = riso_triangle", point_map);
            auto gen = ne.compute_riso_triangle(&num);
            CartesianPoint pa = gen();
            CartesianPoint pb = gen();
            CartesianPoint pc = gen();
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
            auto gen = ne.compute_equi_triangle(&num);
            CartesianPoint pa = gen();
            CartesianPoint pb = gen();
            CartesianPoint pc = gen();
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
            auto gen = ne.compute_rectangle(&num);
            CartesianPoint pa = gen();
            CartesianPoint pb = gen();
            CartesianPoint pc = gen();
            CartesianPoint pd = gen();
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
            auto gen = ne.compute_square(&num);
            CartesianPoint pa = gen();
            CartesianPoint pb = gen();
            CartesianPoint pc = gen();
            CartesianPoint pd = gen();
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
            auto gen = ne.compute_trapezoid(&num);
            CartesianPoint pa = gen();
            CartesianPoint pb = gen();
            CartesianPoint pc = gen();
            CartesianPoint pd = gen();
            CartesianLine lab(pa, pb);
            CartesianLine lcd(pc, pd);
            CHECK(NumUtils::is_close(Cartesian::angle_of(lab), Cartesian::angle_of(lcd)));
        }
        SUBCASE("eq_trapezoid") {
            Numeric num("a b c d = eq_trapezoid", point_map);
            auto gen = ne.compute_eq_trapezoid(&num);
            CartesianPoint pa = gen();
            CartesianPoint pb = gen();
            CartesianPoint pc = gen();
            CartesianPoint pd = gen();
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
        NumEngine ne;
        
        Numeric num_("a b = segment", point_map);
        auto gen_ = ne.compute_segment(&num_);
        ne.point_to_cartesian[point_map["a"].get()].push_back(gen_());
        ne.point_to_cartesian[point_map["b"].get()].push_back(gen_());

        SUBCASE("line") {
            Numeric num("c = line a b", point_map);
            auto gen = ne.compute_line(&num);
            CartesianLine line = gen();
            CartesianPoint pa = ne.get_arg_cartesian(&num, 0);
            CartesianPoint pb = ne.get_arg_cartesian(&num, 1);
            CHECK(line.contains(pa));
            CHECK(line.contains(pb));
        }
        SUBCASE("line_at_angle") {
            Numeric num0("c d e = triangle", point_map);
            auto gen0 = ne.compute_triangle(&num0);
            ne.point_to_cartesian[point_map["c"].get()].push_back(gen0());
            ne.point_to_cartesian[point_map["d"].get()].push_back(gen0());
            ne.point_to_cartesian[point_map["e"].get()].push_back(gen0());

            Numeric num("f = line_at_angle a b c d e", point_map);
            auto gen = ne.compute_line_at_angle(&num);
            CartesianRay ray = gen();
            CartesianPoint pf = Cartesian::get_random_point_on_ray(ray);
            CartesianPoint pa = ne.get_arg_cartesian(&num, 0);
            CartesianPoint pb = ne.get_arg_cartesian(&num, 1);
            CartesianPoint pc = ne.get_arg_cartesian(&num, 2);
            CartesianPoint pd = ne.get_arg_cartesian(&num, 3);
            CartesianPoint pe = ne.get_arg_cartesian(&num, 4);

            double ang_fab = Cartesian::angle_between(pf, pa, pb);
            double ang_cde = Cartesian::angle_between(pc, pd, pe);
            CHECK(NumUtils::is_close(ang_fab, ang_cde));
        }
        SUBCASE("line_bisect") {
            Numeric num("c = line_bisect a b", point_map);
            auto gen = ne.compute_line_bisect(&num);
            CartesianLine bisect = gen();
            CartesianPoint pa = ne.get_arg_cartesian(&num, 0);
            CartesianPoint pb = ne.get_arg_cartesian(&num, 1);
            CartesianPoint mid = Cartesian::midpoint(pa, pb);
            CHECK(bisect.contains(mid));
            CartesianPoint rand = Cartesian::get_random_point_on_line(bisect);
            double ang1 = Cartesian::angle_between(rand, pa, pb);;
            double ang2 = Cartesian::angle_between(pa, pb, rand);
            CHECK(NumUtils::is_close(ang1, ang2));
        }
        SUBCASE("line_para") {
            Numeric num0("c = free", point_map);
            auto gen0 = ne.compute_free(&num0);
            ne.point_to_cartesian[point_map["c"].get()].push_back(gen0());

            Numeric num("d = line_para c a b", point_map);
            auto gen = ne.compute_line_para(&num);
            CartesianLine para = gen();
            CartesianPoint pc = ne.get_arg_cartesian(&num, 0);
            CartesianPoint pa = ne.get_arg_cartesian(&num, 1);
            CartesianPoint pb = ne.get_arg_cartesian(&num, 2);
            CHECK(para.contains(pc));
            double ang_ab = Cartesian::angle_of(CartesianLine(pa, pb));
            double ang_pc = Cartesian::angle_of(para);
            CHECK(NumUtils::is_close(ang_ab, ang_pc));
        }
        SUBCASE("line_perp") {
            Numeric num0("c = free", point_map);
            auto gen0 = ne.compute_free(&num0);
            ne.point_to_cartesian[point_map["c"].get()].push_back(gen0());

            Numeric num("d = line_perp c a b", point_map);
            auto gen = ne.compute_line_perp(&num);
            CartesianLine perp = gen();
            CartesianPoint pc = ne.get_arg_cartesian(&num, 0);
            CartesianPoint pa = ne.get_arg_cartesian(&num, 1);
            CartesianPoint pb = ne.get_arg_cartesian(&num, 2);
            CHECK(perp.contains(pc));
            double ang_ab = Cartesian::angle_of(CartesianLine(pa, pb));
            double ang_pc = Cartesian::angle_of(perp);
            CHECK(NumUtils::is_close(std::abs(ang_ab - ang_pc), M_PI_2));
        }
        SUBCASE("ray") {
            Numeric num("c = ray a b", point_map);
            auto gen = ne.compute_ray(&num);
            CartesianRay ray = gen();
            CartesianPoint pa = ne.get_arg_cartesian(&num, 0);
            CartesianPoint pb = ne.get_arg_cartesian(&num, 1);
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
        NumEngine ne;
        
        Numeric num_("a b c = triangle", point_map);
        auto gen_ = ne.compute_triangle(&num_);
        ne.point_to_cartesian[point_map["a"].get()].push_back(gen_());
        ne.point_to_cartesian[point_map["b"].get()].push_back(gen_());
        ne.point_to_cartesian[point_map["c"].get()].push_back(gen_());


        SUBCASE("midpoint") {
            Numeric num("d = midpoint a b", point_map);
            auto gen = ne.compute_midpoint(&num);
            CartesianPoint mid = gen();
            CartesianPoint pa = ne.get_arg_cartesian(&num, 0);
            CartesianPoint pb = ne.get_arg_cartesian(&num, 1);
            CHECK(Cartesian::midpoint(pa, pb) == mid);
        }
        SUBCASE("trisegment") {
            Numeric num("d e = trisegment a b", point_map);
            auto gen = ne.compute_trisegment(&num);
            CartesianPoint pd = gen();
            CartesianPoint pe = gen();
            CartesianPoint pa = ne.get_arg_cartesian(&num, 0);
            CartesianPoint pb = ne.get_arg_cartesian(&num, 1);
            CHECK(pd == 2 * pa / 3 + pb / 3);
            CHECK(pe == pa / 3 + 2 * pb / 3);
        }
        SUBCASE("mirror") {
            Numeric num("c = mirror a b", point_map);
            auto gen = ne.compute_mirror(&num);
            CartesianPoint pc = gen();
            CartesianPoint pa = ne.get_arg_cartesian(&num, 0);
            CartesianPoint pb = ne.get_arg_cartesian(&num, 1);
            CHECK(pc == 2 * pb - pa);
        }
        SUBCASE("reflect") {
            Numeric num("d = reflect c a b", point_map);
            auto gen = ne.compute_reflect(&num);
            CartesianPoint pd = gen();
            CartesianPoint pc = ne.get_arg_cartesian(&num, 0);
            CartesianPoint pa = ne.get_arg_cartesian(&num, 1);
            CartesianPoint pb = ne.get_arg_cartesian(&num, 2);
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
        NumEngine ne;

        Numeric num_("a b c = triangle", point_map);
        auto gen = ne.compute_triangle(&num_);
        ne.point_to_cartesian[point_map["a"].get()].push_back(gen());
        ne.point_to_cartesian[point_map["b"].get()].push_back(gen());
        ne.point_to_cartesian[point_map["c"].get()].push_back(gen());

        SUBCASE("angle_eq2") {
            // <(BA, AF) = <(FC, CB)
            Numeric num("f = angle_eq2 a b c", point_map);
            CartesianPoint pa = ne.get_arg_cartesian(&num, 0);
            CartesianPoint pb = ne.get_arg_cartesian(&num, 1);
            CartesianPoint pc = ne.get_arg_cartesian(&num, 2);
            for (int N=0; N<10; N++) {
                auto gen = ne.compute_angle_eq2(&num);
                CartesianPoint pf = gen();
                double ang_baf = Cartesian::angle_between(pb, pa, pf);
                double ang_fcb = Cartesian::angle_between(pf, pc, pb);
                CHECK(NumUtils::is_close(ang_baf, ang_fcb));
            }
        }
        SUBCASE("angle_eq3") {
            for (int N=0; N<10; N++) {
                Numeric num_("d e = segment", point_map);
                auto gen_ = ne.compute_segment(&num_);
                ne.point_to_cartesian[point_map["d"].get()].push_back(gen_());
                ne.point_to_cartesian[point_map["e"].get()].push_back(gen_());

                // <(DF, FE) = <(AB, BC)
                Numeric num("f = angle_eq3 d e a b c", point_map);
                auto gen = ne.compute_angle_eq3(&num);
                CartesianCircle cf = gen();
                CartesianPoint pd = ne.get_arg_cartesian(&num, 0);
                CartesianPoint pe = ne.get_arg_cartesian(&num, 1);
                CartesianPoint pa = ne.get_arg_cartesian(&num, 2);
                CartesianPoint pb = ne.get_arg_cartesian(&num, 3);
                CartesianPoint pc = ne.get_arg_cartesian(&num, 4);
                CHECK((cf.contains(pd) && cf.contains(pe)));

                CartesianPoint pf = Cartesian::get_random_point_on_circle(cf);
                double ang_dfe = Cartesian::angle_between(pd, pf, pe);
                double ang_abc = Cartesian::angle_between(pa, pb, pc);
                CHECK((NumUtils::is_close(ang_dfe, ang_abc) || NumUtils::is_close(std::abs(ang_dfe - ang_abc), M_PI)));
            }
        }
        SUBCASE("angle_mirror") {
            // <(BA, BC) = <(BC, BD)
            Numeric num("d = angle_mirror a b c", point_map);
            CartesianPoint pa = ne.get_arg_cartesian(&num, 0);
            CartesianPoint pb = ne.get_arg_cartesian(&num, 1);
            CartesianPoint pc = ne.get_arg_cartesian(&num, 2);
            auto gen = ne.compute_angle_mirror(&num);
            CartesianRay rd = gen();
            CartesianPoint pd = Cartesian::get_random_point_on_ray(rd);
            double ang_abc = Cartesian::angle_between(pa, pb, pc);
            double ang_cbd = Cartesian::angle_between(pc, pb, pd);
            CHECK(NumUtils::is_close(ang_abc, ang_cbd));
        }
        SUBCASE("angle_bisect") {
            // bisect angle ABC
            Numeric num("d = angle_bisect a b c", point_map);
            CartesianPoint pa = ne.get_arg_cartesian(&num, 0);
            CartesianPoint pb = ne.get_arg_cartesian(&num, 1);
            CartesianPoint pc = ne.get_arg_cartesian(&num, 2);
            auto gen = ne.compute_angle_bisect(&num);
            CartesianRay rd = gen();
            CartesianPoint pd = Cartesian::get_random_point_on_ray(rd);
            double ang_abd = Cartesian::angle_between(pa, pb, pd);
            double ang_dbc = Cartesian::angle_between(pd, pb, pc);
            CHECK(NumUtils::is_close(ang_abd, ang_dbc));
            CHECK(std::abs(ang_abd) < M_PI_2);
        }
        SUBCASE("angle_exbisect") {
            Numeric num("d = angle_exbisect a b c", point_map);
            CartesianPoint pa = ne.get_arg_cartesian(&num, 0);
            CartesianPoint pb = ne.get_arg_cartesian(&num, 1);
            CartesianPoint pc = ne.get_arg_cartesian(&num, 2);
            auto gen = ne.compute_angle_exbisect(&num);
            CartesianLine ld = gen();
            CartesianPoint pd = Cartesian::get_random_point_on_line(ld);
            double ang_abd = Cartesian::angle_between(pa, pb, pd);
            double ang_dbc = Cartesian::angle_between(pd, pb, pc);
            CHECK(NumUtils::is_close(std::abs(ang_abd) + std::abs(ang_dbc), M_PI));
            CHECK(((std::abs(ang_abd) > M_PI_2) || (std::abs(ang_dbc) > M_PI_2)));
        }
        SUBCASE("angle_trisect") {
            Numeric num("d e = angle_trisect a b c", point_map);
            CartesianPoint pa = ne.get_arg_cartesian(&num, 0);
            CartesianPoint pb = ne.get_arg_cartesian(&num, 1);
            CartesianPoint pc = ne.get_arg_cartesian(&num, 2);
            auto gen = ne.compute_angle_trisect(&num);
            CartesianRay rd = gen();
            CartesianRay re = gen();
            CartesianPoint pd = Cartesian::get_random_point_on_ray(rd);
            CartesianPoint pe = Cartesian::get_random_point_on_ray(re);
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
        NumEngine ne;

        Numeric num_("a b c d = quadrilateral", point_map);
        auto gen = ne.compute_quadrilateral(&num_);
        ne.point_to_cartesian[point_map["a"].get()].push_back(gen());
        ne.point_to_cartesian[point_map["b"].get()].push_back(gen());
        ne.point_to_cartesian[point_map["c"].get()].push_back(gen());
        ne.point_to_cartesian[point_map["d"].get()].push_back(gen());

        SUBCASE("circle") {
            Numeric num("e = circle a b c", point_map);
            auto gen = ne.compute_circle(&num);
            CartesianCircle circle = gen();
            CartesianPoint pe = Cartesian::get_random_point_on_circle(circle);
            CartesianPoint pa = ne.get_arg_cartesian(&num, 0);
            CartesianPoint pb = ne.get_arg_cartesian(&num, 1);
            CartesianPoint pc = ne.get_arg_cartesian(&num, 2);
            CHECK(circle.c == pa);
            CHECK(NumUtils::is_close(Cartesian::distance(pe, pa), circle.r));
            CHECK(NumUtils::is_close(Cartesian::distance(pb, pc), circle.r));
            CHECK(circle.contains(pe));
        }
        SUBCASE("circum") {
            Numeric num("e = circum a b c", point_map);
            auto gen = ne.compute_circum(&num);
            CartesianCircle circum = gen();
            CartesianPoint pa = ne.get_arg_cartesian(&num, 0);
            CartesianPoint pb = ne.get_arg_cartesian(&num, 1);
            CartesianPoint pc = ne.get_arg_cartesian(&num, 2);
            CHECK((circum.contains(pa) && circum.contains(pb) && circum.contains(pc)));

            CartesianPoint pe = Cartesian::get_random_point_on_circle(circum);
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
            auto gen = ne.compute_diameter(&num);
            CartesianCircle circ = gen();
            CartesianPoint pa = ne.get_arg_cartesian(&num, 0);
            CartesianPoint pb = ne.get_arg_cartesian(&num, 1);
            CartesianPoint pe = Cartesian::get_random_point_on_circle(circ);
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
        NumEngine ne;

        /* Circles (A, AB) and (C, CD) have two intersections. 
        Circle (E, EF) contains circle (G, GH). 
        Circles (A, AB) and (E, EF) have the same radius.
        The two groups of circles are pairwise outside each other. */
        ne.point_to_cartesian[point_map["a"].get()].push_back(CartesianPoint(-9.01084, 1.32536));
        ne.point_to_cartesian[point_map["b"].get()].push_back(CartesianPoint(-9.01084, 3.68154));
        ne.point_to_cartesian[point_map["c"].get()].push_back(CartesianPoint(-7.85923, -0.38221));
        ne.point_to_cartesian[point_map["d"].get()].push_back(CartesianPoint(-8.25634, 0.66351));
        ne.point_to_cartesian[point_map["e"].get()].push_back(CartesianPoint(-3.29247, 1.18637));
        ne.point_to_cartesian[point_map["f"].get()].push_back(CartesianPoint(-3.29247, -1.16981));
        ne.point_to_cartesian[point_map["g"].get()].push_back(CartesianPoint(-3.71605, 1.3386));
        ne.point_to_cartesian[point_map["h"].get()].push_back(CartesianPoint(-2.85565, 2.59611));
        
        CartesianPoint pa = ne.get_cartesian(point_map["a"].get());
        CartesianPoint pb = ne.get_cartesian(point_map["b"].get());
        CartesianPoint pc = ne.get_cartesian(point_map["c"].get());
        CartesianPoint pd = ne.get_cartesian(point_map["d"].get());
        CartesianPoint pe = ne.get_cartesian(point_map["e"].get());
        CartesianPoint pf = ne.get_cartesian(point_map["f"].get());
        CartesianPoint pg = ne.get_cartesian(point_map["g"].get());
        CartesianPoint ph = ne.get_cartesian(point_map["h"].get());

        CartesianCircle cab(pa, Cartesian::distance(pa, pb));
        CartesianCircle ccd(pc, Cartesian::distance(pc, pd));
        CartesianCircle cef(pe, Cartesian::distance(pe, pf));
        CartesianCircle cgh(pg, Cartesian::distance(pg, ph));

        REQUIRE(NumUtils::is_close(cab.r, cef.r));

        SUBCASE("tangents") {
            Numeric num("i j = tangents g a b", point_map);
            // AE, AF are tangent to the circle centered at B with radius BC
            auto gen = ne.compute_tangents(&num);
            CartesianPoint pi = gen();
            CartesianPoint pj = gen();
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
            auto gen = ne.compute_tangents(&num);
            CHECK_FALSE(gen.next());
        }
        SUBCASE("common_tangent") {
            Numeric num("i j = common_tangent a b c d", point_map);
            // IJ is a common tangent to the circles (A, AB) and (C, CD)
            auto gen = ne.compute_common_tangent(&num);
            CartesianPoint pi = gen();
            CartesianPoint pj = gen();
            double rab = cab.r;
            double rcd = ccd.r;
            CHECK((NumUtils::is_close(Cartesian::distance(pa, pi), rab) && 
                NumUtils::is_close(Cartesian::distance(pc, pj), rcd)));
            CHECK((NumUtils::is_close(std::abs(Cartesian::angle_between(pa, pi, pj)), M_PI_2) &&
                NumUtils::is_close(std::abs(Cartesian::angle_between(pc, pj, pi)), M_PI_2)));
        }
        SUBCASE("common_tangent (same radius)") {
            Numeric num("i j = common_tangent a b e f", point_map);
            auto gen = ne.compute_common_tangent(&num);
            CartesianPoint pi = gen();
            CartesianPoint pj = gen();
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
            auto gen = ne.compute_common_tangent2(&num);
            CartesianPoint pi = gen();
            CartesianPoint pj = gen();
            CartesianPoint pk = gen();
            CartesianPoint pl = gen();
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
            auto gen = ne.compute_common_tangent2(&num);
            CHECK_FALSE(gen.next());
        }
    }
}