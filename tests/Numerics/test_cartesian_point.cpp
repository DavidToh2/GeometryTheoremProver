
#include <doctest.h>

#include "Numerics/Cartesian.hh"

TEST_SUITE("CartesianPoint") {
    TEST_CASE("is_same") {
        CartesianPoint p1(1.0, 2.0);
        CartesianPoint p2(1.0 + 1e-10, 2.0 - 1e-10);
        CartesianPoint p3(1.0 + 1e-5, 2.0);

        CHECK(p1.is_same(p2));
        CHECK(CartesianPoint::is_same(p1, p2));
        CHECK_FALSE(p1.is_same(p3));
        CHECK_FALSE(CartesianPoint::is_same(p1, p3));

        CartesianPoint p4(2.109749823748256, -1.507538781223123);
        CartesianPoint p5(-3.507839201237489, 4.908374092374890);
        CHECK(p4.x == 2.109749823748256);
        CHECK(p4.y == -1.507538781223123);
        CHECK(p5.x == -3.507839201237489);
        CHECK(p5.y == 4.908374092374890);
    }

    TEST_CASE("Arithmetic") {
        CartesianPoint p1(1.0, 2.0);
        CartesianPoint p2(3.0, 4.0);

        CartesianPoint p3 = p1 + p2;
        CHECK(p3 == CartesianPoint(4.0, 6.0));

        CartesianPoint p4 = p2 - p1;
        CHECK(p4 == CartesianPoint(2.0, 2.0));

        CartesianPoint p5 = p1 * 2.0;
        CHECK(p5 == CartesianPoint(2.0, 4.0));

        CartesianPoint p6 = p2 / 1.6;
        CHECK(p6 == CartesianPoint(1.875, 2.5));
    }

    TEST_CASE("Transforms") {
        CartesianPoint p1(1.0, 0.0);
        p1.rotate(M_PI / 2);
        CHECK(p1 == CartesianPoint(0.0, 1.0));
        p1.rotate(M_PI / 3);
        CHECK(p1 == CartesianPoint(-std::sqrt(3)/2, 0.5));
        p1.rotate(-M_PI / 6);
        CHECK(p1 == CartesianPoint(-0.5, std::sqrt(3)/2));
        p1.rotate(-M_PI_2 / 2);
        CHECK(p1 == CartesianPoint((std::sqrt(3) - 1)/(2 * std::sqrt(2)), (std::sqrt(3) + 1)/(2 * std::sqrt(2))));
        p1.rotate(M_PI + 5 * M_PI / 12);
        CHECK(p1 == CartesianPoint(std::sqrt(3) / 2, -0.5));
        p1.rotate(M_PI_2 + M_PI / 3);
        CHECK(p1 == CartesianPoint(-0.5, std::sqrt(3)/2));

        CartesianPoint p2(1.0, 1.0);
        p2.scale(2.0);
        CHECK(p2 == CartesianPoint(2.0, 2.0));
        p2.scale(0.5);
        CHECK(p2 == CartesianPoint(1.0, 1.0));
        p2.rotate(M_PI / 3).flip();
        CHECK(p2 == CartesianPoint((std::sqrt(3) - 1) / 2.0, (std::sqrt(3) + 1) / 2.0));

        CartesianPoint p3(1.0, 1.0);
        p3.shift(CartesianPoint(2.0, 3.0));
        CHECK(p3 == CartesianPoint(3.0, 4.0));
        p3.rotate(M_PI / 2).rotate(M_PI_2).rotate(M_PI_4 * 2);
        CHECK(p3 == CartesianPoint(4.0, -3.0));
    }

    TEST_CASE("from_polar") {
        CartesianPoint p1 = Cartesian::from_polar(1.0, 0.0);
        CHECK(p1 == CartesianPoint(1.0, 0.0));

        CartesianPoint p2 = Cartesian::from_polar(2.0, M_PI_2);
        CHECK(p2 == CartesianPoint(0.0, 2.0));

        CartesianPoint p3 = Cartesian::from_polar(std::sqrt(3), -M_PI / 3);
        CHECK(p3 == CartesianPoint(std::sqrt(3) / 2, -3.0 / 2));

        double angle = std::atan2(3.0, 4.0);
        CartesianPoint p4 = Cartesian::from_polar(-4.0, angle);
        CHECK(p4 == Cartesian::from_polar(4.0, angle + M_PI));
    }

    TEST_CASE("distance, dot") {
        CartesianPoint p1(1.0, 2.0);
        CartesianPoint p2(4.0, 6.0);

        double dist = Cartesian::distance(p1, p2);
        CHECK(NumUtils::is_close(dist, 5.0));

        double dist2 = Cartesian::distance2(p1, p2);
        CHECK(NumUtils::is_close(dist2, 25.0));

        double dot_product = Cartesian::dot(p1, p2);
        CHECK(NumUtils::is_close(dot_product, 16.0));
    }

    TEST_CASE("midpoint") {
        CartesianPoint p1(1.0, 2.0);
        CartesianPoint p2(3.0, 4.0);

        CartesianPoint mid = Cartesian::midpoint(p1, p2);
        CHECK(mid == CartesianPoint(2.0, 3.0));

        CartesianPoint p3(2.109749823748256, -1.507538781223123);
        CartesianPoint p4(-3.507839201237489, 4.908374092374890);
        CartesianPoint mid2 = Cartesian::midpoint(p3, p4);
        CHECK(mid2 == CartesianPoint(-0.6990446887446165, 1.7004176555758835));
    }

    TEST_CASE("perp_vec_p, perp_vec_n") {
        CartesianPoint p1(1.9823786298356, -0.6957398739485);
        CartesianPoint p2(-4.2394872398472, 2.7848762638452);

        CartesianPoint p3 = p2 + Cartesian::perp_vec_p(p2, p1);
        CHECK(p3 == CartesianPoint(-0.7588711020535, 9.006742133528));
        CartesianPoint p4 = p2 + Cartesian::perp_vec_n(p2, p1);
        CHECK(p4 == CartesianPoint(-7.7201033776409, -3.4369896058376));

        CartesianPoint p5a = p2 + Cartesian::perp_vec_p(p2, p3);;
        CartesianPoint p5b = p2 + Cartesian::perp_vec_n(p2, p4);
        CHECK(p5a == p5b);

        CHECK(p5a == p3 + Cartesian::perp_vec_n(p3, p1));
        CHECK(p5a == p4 + Cartesian::perp_vec_p(p4, p1));
    }

    TEST_CASE("angle_of") {
        CartesianPoint o(0, 0);
        CartesianPoint p1(1, 0);
        CartesianPoint p2(0, 1);
        CartesianPoint p3(-1, 0);
        CartesianPoint p4(0, -1);
        CartesianPoint p5(-3, -4);

        CHECK(NumUtils::is_close(Cartesian::angle_of(o, p1), M_PI_2));
        CHECK(NumUtils::is_close(Cartesian::angle_of(o, p2), M_PI));
        CHECK(NumUtils::is_close(Cartesian::angle_of(o, p3), -M_PI_2));
        CHECK(NumUtils::is_close(Cartesian::angle_of(o, p4), 0.0));
        CHECK(NumUtils::is_close(Cartesian::angle_of(o, p5), 0.9272952180016 - M_PI_2));
    }

    TEST_CASE("Random Affine") {
        CartesianPoint a(0, 0);
        CartesianPoint b(1, 0);
        CartesianPoint c(0, 2);
        CartesianPoint d(1.7129879846292, 1.1258923749802);
        double ab = Cartesian::distance(a, b);
        double bc = Cartesian::distance(b, c);
        double cd = Cartesian::distance(c, d);
        double ad = Cartesian::distance(a, d);
        double ac = Cartesian::distance(a, c);
        double bd = Cartesian::distance(b, d);
        std::array<double, 6> dist = {ab, bc, cd, ad, ac, bd};
        double ang_abc = Cartesian::angle_between(a, b, c);
        double ang_bcd = Cartesian::angle_between(b, c, d);
        double ang_cda = Cartesian::angle_between(c, d, a);
        double ang_dab = Cartesian::angle_between(d, a, b);
        std::array<double, 4> angles = {ang_abc, ang_bcd, ang_cda, ang_dab};

        Cartesian::random_affine(a, b, c, d);
        double ab2 = Cartesian::distance(a, b);
        double bc2 = Cartesian::distance(b, c);
        double cd2 = Cartesian::distance(c, d);
        double ad2 = Cartesian::distance(a, d);
        double ac2 = Cartesian::distance(a, c);
        double bd2 = Cartesian::distance(b, d);
        std::array<double, 6> dist2 = {ab2, bc2, cd2, ad2, ac2, bd2};
        double ang_abc2 = Cartesian::angle_between(a, b, c);
        double ang_bcd2 = Cartesian::angle_between(b, c, d);
        double ang_cda2 = Cartesian::angle_between(c, d, a);
        double ang_dab2 = Cartesian::angle_between(d, a, b);
        std::array<double, 4> angles2 = {ang_abc2, ang_bcd2, ang_cda2, ang_dab2};
        
        bool all_pass_1 = true;
        double r = ab / ab2;
        CHECK(!NumUtils::is_close(r, 1));
        for (int i=1; i<6; i++) {
            all_pass_1 = all_pass_1 && NumUtils::is_close(dist[i] / dist2[i], r);
        }
        CHECK(all_pass_1);

        bool all_pass_2 = true;
        for (int i=0; i<4; i++) {
            all_pass_2 = all_pass_2 && NumUtils::is_close(angles[i], angles2[i]);
        }
        CHECK(all_pass_2);
    }
}

