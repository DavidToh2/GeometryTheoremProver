
#include <doctest.h>
#include <iomanip>
#include <iostream>

#include "Numerics/Cartesian.hh"

TEST_SUITE("CartesianLine") {
    TEST_CASE("contains") {
        CartesianLine l1(1, -1, 0); // y = x
        CHECK(l1.contains(CartesianPoint(1.0, 1.0)));
        CHECK_FALSE(l1.contains(CartesianPoint(1.0, 2.0)));

        CartesianLine l2(3, 4, -10); // 3x + 4y = 10
        CHECK(l2.contains(CartesianPoint(2.0, 1.0)));
        CHECK_FALSE(l2.contains(CartesianPoint(1.0, 1.0)));

        CartesianLine l3(std::sqrt(3), -1, std::sqrt(2)); // y = sqrt(3)x + sqrt(2)
        CHECK(l3.contains(CartesianPoint(-2.0, -2 * std::sqrt(3) + std::sqrt(2))));
        CHECK(l3.contains(CartesianPoint(std::sqrt(3), 3 + std::sqrt(2))));
        CHECK_FALSE(l3.contains(CartesianPoint(0, 1.414)));

        CartesianLine l4(CartesianPoint(5.0/6.0, 1.0/4.0), CartesianPoint(1.0/3.0, 2.0/3.0)); // y = 17/18 - 5x/6
        CHECK(l4.contains(CartesianPoint(2.0/3.0, 7.0/18.0)));
        CHECK_FALSE(l4.contains(CartesianPoint(1.0, 1.0)));

        CartesianLine l5({-2 * std::sqrt(3), -1}, {5 * std::sqrt(3), 6}); // y = x/sqrt(3) + 1
        CHECK(l5.contains({2 * std::sqrt(3), 3}));
        CHECK_FALSE(l5.contains({0.001, 0.999}));
    }

    TEST_CASE("intersect (line with line)") {
        CartesianLine l1(1, -1, 0); // y = x
        CartesianLine l2(1, 1, -2); // y = 2 - x
        auto p1 = Cartesian::intersect(l1, l2);
        REQUIRE(p1);
        CHECK(p1.value() == CartesianPoint(1.0, 1.0));

        CartesianLine l3(2, -1, -4); // y = 2x - 4
        auto p2 = Cartesian::intersect(l1, l3);
        REQUIRE(p2);
        CHECK(p2.value() == CartesianPoint(4.0, 4.0));

        CartesianLine l4(1, -2, 3); // y = x/2 + 3/2
        auto p3 = Cartesian::intersect(l3, l4);
        REQUIRE(p3);
        CHECK(p3.value() == CartesianPoint(3 + 2.0/3.0, 3 + 1.0/3.0));

        CartesianLine l5(1, -2, 4); // y = x/2 + 2
        auto p4 = Cartesian::intersect(l4, l5);
        CHECK_FALSE(p4);
        auto p5 = Cartesian::intersect(l3, l5);
        REQUIRE(p5);
        CHECK(l1.contains(p5.value()));

        CartesianLine l6({-2, -std::sqrt(3) + 1}, {4, std::sqrt(3)+1}); // y = (x-1)/sqrt(3) + 1
        auto p6 = Cartesian::intersect(l2, l6);
        REQUIRE(p6);
        CHECK(l1.contains(p6.value()));

        CartesianLine l7(7.0/6.0, -29.0/12.0, std::sqrt(3));
        auto p7 = Cartesian::intersect(l3, l6);
        REQUIRE(p7);
        CHECK(l7.contains(p7.value()));
    }

    TEST_CASE("angle_of and angle_between") {

        SUBCASE("Multiples of M_PI_4") {
            CartesianLine l1(1, -1, 0); // y = x
            CartesianLine l2(1, 0, -1); // x = 1
            CartesianLine l3(1, 1, 2);  // y = -x-2
            CartesianLine l4(0, 1, -1); // y = 1
            std::array al1 = {l1, l2, l3, l4, l1};

            double angle1 = Cartesian::angle_of(l1);
            CHECK(NumUtils::is_close(angle1, M_PI_2 + M_PI_4));

            double angle2 = Cartesian::angle_of(l2);
            CHECK(NumUtils::is_close(angle2, 0));

            double angle3 = Cartesian::angle_of(l3);
            CHECK(NumUtils::is_close(angle3, M_PI_4));

            double angle4 = Cartesian::angle_of(l4);
            CHECK(NumUtils::is_close(angle4, M_PI_2));

            for (int i=0; i<4; i++) {
                double ab = Cartesian::angle_between(al1[i], al1[i+1]);
                CHECK(NumUtils::is_close(ab, M_PI_4));
                ab = Cartesian::angle_between(al1[i+1], al1[i]);
                CHECK(NumUtils::is_close(ab, 3 * M_PI_4));
            }

            CartesianLine l2a(1, 0.0000001, -2); // x = 2 with infinitesimal tilt towards the left
            double angle2a = Cartesian::angle_of(l2a);
            CHECK_FALSE(NumUtils::is_close(angle2a, 0.0));
            CHECK(angle2a < 1); // angle should be small
        }
        SUBCASE("Equilateral Triangle") {
            CartesianPoint p1(-9.91, -2.89);
            CartesianPoint p2(-12.33, 4.33);
            CartesianPoint p3(-4.8672965846764, 2.8157814771583);

            CartesianLine l1(p1, p2);
            CartesianLine l2(p2, p3);
            CartesianLine l3(p3, p1);

            double angle12 = Cartesian::angle_of(l1);
            double angle13 = Cartesian::angle_of(l3);
            double angle23 = Cartesian::angle_of(l2);
            CHECK(NumUtils::is_close(angle12, 0.3234116824369));
            CHECK(NumUtils::is_close(angle13, 2.4178067848301));
            CHECK(NumUtils::is_close(angle23, M_PI - 1.7709834199563));
            CHECK(NumUtils::is_close(angle13 - angle12, 2 * M_PI / 3));

            double angle123 = Cartesian::angle_between(l1, l2);
            CHECK(NumUtils::is_close(angle123, M_PI / 3));
            double angle321 = Cartesian::angle_between(l2, l1);;
            CHECK(NumUtils::is_close(angle321, 2 * M_PI / 3));

            double angle213 = Cartesian::angle_between(l1, l3);
            CHECK(NumUtils::is_close(angle213, 2 * M_PI / 3));
            double angle312 = Cartesian::angle_between(l3, l1);
            CHECK(NumUtils::is_close(angle312, M_PI / 3));

            double angle132 = Cartesian::angle_between(l3, l2);
            CHECK(NumUtils::is_close(angle132, 2 * M_PI / 3));
            double angle231 = Cartesian::angle_between(l2, l3);
            CHECK(NumUtils::is_close(angle231, M_PI / 3));
        }
        SUBCASE("Convex Polygon") {
            std::array<CartesianPoint, 8> pts = {
                CartesianPoint(0.0, 0.0),
                CartesianPoint(4.0, 0.0),
                CartesianPoint(5.0, 2.0 * std::sqrt(3)),
                CartesianPoint(3.0, 4.0 * std::sqrt(3)),
                CartesianPoint(1.0, 4.0 * std::sqrt(3)),
                CartesianPoint(-1.0, 2.0 * std::sqrt(3)),
                CartesianPoint(-2.0, 0.0),
                CartesianPoint(0.0, 0.0)
            };
            std::vector<CartesianLine> lines;
            for (int i = 0; i < 7; ++i) {
                lines.emplace_back(pts[i], pts[i+1]);
            }
            double sum = 0;
            for (int i = 0; i < 7; ++i) {
                sum += Cartesian::angle_between(lines[i], lines[(i + 1) % 7]);
            }
            CHECK(NumUtils::is_close(sum, 2 * M_PI));
        }
    }

    TEST_CASE("para_line") {
        CartesianPoint a(3.523233943264, 14.6359935953849);
        CartesianPoint b(-3.3288346929421, 6.5127273561798);
        CartesianPoint c(6.3756780279404, 8.9000994951828);

        CartesianLine lbc(b, c);
        CartesianLine lbc_para = Cartesian::para_line(a, lbc);

        CHECK(lbc_para.contains(a));
        CHECK(lbc_para.contains(a + (c-b)));
        CHECK_FALSE(Cartesian::intersect(lbc, lbc_para));

        CartesianLine lac(a, c);
        CartesianLine lac_para = Cartesian::para_line(b, lac);
        CartesianPoint p = Cartesian::intersect(lbc_para, lac_para).value();
        CHECK(p + c == a + b);
    }
    TEST_CASE("perp_line") {
        CartesianPoint a(3.523233943264, 14.6359935953849);
        CartesianPoint b(-3.3288346929421, 6.5127273561798);
        CartesianPoint c(6.3756780279404, 8.9000994951828);

        CartesianLine lab(a, b);
        CartesianLine lac(a, c);
        CartesianLine lbc(b, c);

        CartesianLine lab_perp = Cartesian::perp_line(c, lab);
        CartesianLine lac_perp = Cartesian::perp_line(b, lac);
        CartesianLine lbc_perp = Cartesian::perp_line(a, lbc);

        SUBCASE("Checking perpendicularity") {
            CHECK(lab_perp.contains(c));
            CartesianPoint v_ab_perp = Cartesian::perp_vec_n(a, b);
            CHECK(lab_perp.contains(c + v_ab_perp));
        }
        SUBCASE("Checking orthocenter") {
            CartesianPoint h = Cartesian::intersect(lab_perp, lac_perp).value();
            CHECK(lbc_perp.contains(h));
        }
        SUBCASE("Perpendicular through endpoint") {
            CartesianLine lab_perp_a = Cartesian::perp_line(a, lab);
            CHECK(lab_perp_a.contains(a));
            CartesianLine lab_perp_b = Cartesian::perp_line(b, lab);
            CartesianPoint b_ = Cartesian::intersect(lab_perp_b, lab).value();
            CHECK(b == b_);
        }
        SUBCASE("Two perpendiculars make a parallel") {
            CartesianPoint p(1, 1);
            CartesianLine lab_perp_perp = Cartesian::perp_line(p, lab_perp);
            CHECK(NumUtils::is_close(Cartesian::angle_of(lab), Cartesian::angle_of(lab_perp_perp)));
        }
    }

    TEST_CASE("perp_bisect") {
        CartesianPoint a(-5.6852019989711, 11.1944571352637);
        CartesianPoint b(-0.1043320636393, 9.1171333260013);
        CartesianPoint c(-1.1831950217977, 14.7540483905033);
        CartesianPoint d(-3.6301562157547, 8.180122794798);

        CartesianLine ab_bisect = Cartesian::perp_bisect(a, b);
        CHECK(ab_bisect.contains(Cartesian::midpoint(a, b)));
        CHECK(ab_bisect.contains(c));
        CHECK(ab_bisect.contains(d));
    }

    TEST_CASE("foot") {
        CartesianPoint a(3.523233943264, 14.6359935953849);
        CartesianPoint b(-3.3288346929421, 6.5127273561798);
        CartesianPoint c(6.3756780279404, 8.9000994951828);

        CartesianLine lab(a, b);
        CartesianLine lac(a, c);
        CartesianLine lbc(b, c);
        // std::cout << std::setprecision(15) << lbc.a << " " << lbc.b << " " << lbc.c << "\n";

        CartesianPoint a_f = Cartesian::foot(a, lbc);
        CartesianPoint expected0(5.0165530726808, 8.5657460716845);
        // Note: Actual coordinates are 5.01655359029296, 8.56574619902034
        // The discrepancy from Geogebra's coordinates is approximately 5e-7
        CHECK(CartesianPoint::is_close(a_f, expected0));
        // CHECK(a_f == expected0);

        CartesianPoint b_f = Cartesian::foot(b, lac);
        CartesianLine lac_perp = Cartesian::perp_line(b, lac);
        CHECK(lac_perp.contains(b_f));

        /* I am more confident that any rounding errors are Geogebra's, not ours, because
        our implementations of foot() and intersect() agree on the coordinates of the
        perpendicular */
        CartesianPoint c_f = Cartesian::foot(c, lab);
        CartesianLine lab_perp_(c, c_f);
        CartesianPoint c_f_ = Cartesian::intersect(lab, lab_perp_).value();
        CHECK(c_f == c_f_);

        // Degenerate case
        CartesianPoint a_f_f = Cartesian::foot(a_f, lbc);
        CHECK(a_f_f == a_f);
    }

    TEST_CASE("reflect") {
        CartesianPoint a(3.523233943264, 14.6359935953849);
        CartesianPoint b(-3.3288346929421, 6.5127273561798);
        CartesianPoint c(6.3756780279404, 8.9000994951828);

        CartesianLine lbc(b, c);
        CartesianPoint a_ = Cartesian::reflect(a, lbc);
        // Note: actual coordinates are 6.509873237322, 2.495498802656
        // The discrepancy from Geogebra's coordinates is again approximately 5e-7
        CHECK(CartesianPoint::is_close(a_, CartesianPoint(6.5098727510352, 2.495498547984)));

        CartesianLine lbc_perp = Cartesian::perp_line(a, lbc);
        CartesianPoint a_2 = Cartesian::intersect(lbc, lbc_perp).value();
        CHECK(a_ == a_2 * 2 - a);
    }
}