
#include <doctest.h>
#include <iomanip>
#include <iostream>

#include "Numerics/Cartesian.hh"

TEST_SUITE("CartesianCircle") {
    TEST_CASE("contains") {
        CartesianCircle c1(CartesianPoint(0.0, 0.0), 5.0);
        CHECK(c1.contains(CartesianPoint(3.0, 4.0)));
        CHECK(c1.contains(CartesianPoint(5.0, 0.0)));
        CHECK_FALSE(c1.contains(CartesianPoint(5.1, 0.0)));

        CartesianCircle c2(CartesianPoint(-2.5, 4.0), 2.5);
        CHECK(c2.contains(CartesianPoint(-2.5, 6.5)));
        CHECK(c2.contains(CartesianPoint(0.0, 4.0)));
        CHECK_FALSE(c2.contains(CartesianPoint(0.1, 4.0)));
    }

    TEST_CASE("intersect (line/ray)") {
        CartesianPoint o(-9.4281496234261, 5.1598425087233);
        CartesianPoint p(-5.0429957374619, 2.2711823862455);

        CartesianPoint a(-8.7693846274334, 2.0188020075193);
        CartesianPoint b(-3.1229837498626, 7.1552678012925);
        CartesianPoint c(-4.1293557628934, 0.392775881849);
        CartesianPoint d(-4.2465702080979, 12.1699346087539);

        CartesianCircle circ(o, p);

        SUBCASE("Two intersections") {
            CartesianLine l1(a, b);
            auto [p1, p2] = Cartesian::intersect(l1, circ);
            REQUIRE((p1.has_value() && p2.has_value()));
            std::array<CartesianPoint, 2> expected = {
                CartesianPoint(-4.2649306275609, 6.1164518895666),
                CartesianPoint(-10.8677499825771, 0.1099432312414)
            };
            CHECK((p1.value() == expected[0] || p1.value() == expected[1]));
            CHECK((p2.value() == expected[0] || p2.value() == expected[1]));

            CartesianRay r1_1(a, b);
            auto [p3, p4] = Cartesian::intersect(r1_1, circ);
            REQUIRE((p3.has_value() && !p4.has_value()));
            CHECK((p3.value() == expected[0] || p3.value() == expected[1]));

            CartesianRay r1_2(b, a);
            auto [p5, p6] = Cartesian::intersect(r1_2, circ);
            REQUIRE((p5.has_value() && p6.has_value()));
            CHECK(p5.value() == p1.value());
            CHECK(p6.value() == p2.value());
        }
        SUBCASE("No intersections") {
            CartesianLine l2(b, c);
            auto [p1, p2] = Cartesian::intersect(l2, circ);
            CHECK((!p1.has_value() && !p2.has_value()));

            CartesianRay r2_1(b, c);
            auto [p3, p4] = Cartesian::intersect(r2_1, circ);
            CHECK((!p3.has_value() && !p4.has_value()));

            CartesianRay r2_2(c, b);
            auto [p5, p6] = Cartesian::intersect(r2_2, circ);
            CHECK((!p5.has_value() && !p6.has_value()));
        }
        SUBCASE("Tangent line") {
            CartesianLine l3(c, d);
            auto [p1, p2] = Cartesian::intersect(l3, circ);
            REQUIRE((p1.has_value() && p2.has_value()));
            // p1: -4.177321117907, 5.212110208806
            // p2: -4.177320962342, 5.212094578396
            // The massive discrepancy is definitely due to my program here. Geogebra probably has
            // some bigfloat module integrated in it.
            CartesianPoint expected = CartesianPoint(-4.1773210401249, 5.2121023936012);
            CHECK(CartesianPoint::is_close(p1.value(), expected));
            CHECK(CartesianPoint::is_close(p2.value(), expected));

            CartesianRay r3_1(expected, c);
            auto [p3, p4] = Cartesian::intersect(r3_1, circ);
            REQUIRE((p3.has_value() && p4.has_value()));
            CHECK(p3.value() == expected);
            CHECK(p4.value() == expected);
        }
    }

    TEST_CASE("intersect (circle)") {
        CartesianPoint a(-8.7693846274334, 2.0188020075193);
        CartesianPoint b(-3.1229837498626, 7.1552678012925);
        CartesianPoint c(-4.1293557628934, 0.392775881849);
        CartesianPoint d(-4.2465702080979, 12.1699346087539);
        CartesianPoint e(-5.0429957374619, 2.2711823862455);

        CartesianCircle c1(a, b);
        CartesianCircle c2(c, d);
        CartesianCircle c3(e, b);

        SUBCASE("Two intersections") {
            auto [p1, p2] = Cartesian::intersect(c1, c2);
            REQUIRE((p1.has_value() && p2.has_value()));
            std::array<CartesianPoint, 2> expected1 = {
                CartesianPoint(-12.4998934928464, 8.6782634967021),
                CartesianPoint(-15.8407825059026, -0.855298854527)
            };
            CHECK((p1.value() == expected1[0] || p1.value() == expected1[1]));
            CHECK((p2.value() == expected1[0] || p2.value() == expected1[1]));
        }

        SUBCASE("Two intersections with one endpoint already specified") {
            auto [p3, p4] = Cartesian::intersect(c1, c3);
            REQUIRE((p3.has_value() && p4.has_value()));
            std::array<CartesianPoint, 2> expected2 = {
                b,
                CartesianPoint(-2.4819612034144, -2.3094112968284)
            };
            CHECK((p3.value() == expected2[0] || p3.value() == expected2[1]));
            CHECK((p4.value() == expected2[0] || p4.value() == expected2[1]));
        }

        SUBCASE("No intersections") {
            auto [p5, p6] = Cartesian::intersect(c2, c3);;
            CHECK((!p5.has_value() && !p6.has_value()));
        }
    }
}