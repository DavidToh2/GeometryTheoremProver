
#include <doctest.h>
#include <iostream>

#include "Numerics/Cartesian.hh"

TEST_SUITE("CartesianRay") {
    TEST_CASE("contains") {
        CartesianPoint start(1.0, 1.0);
        CartesianPoint head(4.0, 4.0);
        CartesianRay r(start, head);

        CHECK(r.contains(CartesianPoint(2.0, 2.0)));
        CHECK(r.contains(start));
        CHECK_FALSE(r.contains(CartesianPoint(0.5, 0.5)));
        CHECK_FALSE(r.contains(CartesianPoint(0.9999999,0.9999999)));
    }

    TEST_CASE("intersect (ray with line/ray)") {
        CartesianRay r1({0, 0}, {4.0, 4.0}); // y = x, x >= 0
        CartesianRay r2({1.0/3.0, 5.0/3.0}, {1, 1}); // y = -x + 2, x >= 1/3
        CHECK(Cartesian::intersect(r1, r2).value() == CartesianPoint(1.0, 1.0));

        CartesianRay r3({2.5, 1}, {3, 2}); // y = 2x - 4, x >= 2.5
        CHECK(Cartesian::intersect(r1, r3).value() == CartesianPoint(4.0, 4.0));
        CHECK_FALSE(Cartesian::intersect(r2, r3));

        CartesianRay r4({3, 3}, {1, 2}); // y = x/2 + 3/2, x <= 3
        CHECK(Cartesian::intersect(r1, r4).value() == CartesianPoint(3.0, 3.0));
        CHECK(Cartesian::intersect(r2, r4).value() == CartesianPoint(1.0/3.0, 5.0/3.0));
        CHECK_FALSE(Cartesian::intersect(r3, r4));

        CartesianLine l1(0.5, -1, 0); // y = 0.5x
        CHECK(Cartesian::intersect(r1, l1).value() == CartesianPoint(0.0, 0.0));
        CHECK(Cartesian::intersect(r2, l1).value() == CartesianPoint(4.0/3.0, 2.0/3.0));
        CHECK(Cartesian::intersect(r3, l1).value() == CartesianPoint(8.0/3.0, 4.0/3.0));
        CHECK_FALSE(Cartesian::intersect(r4, l1));
    }

    TEST_CASE("angle_of, angle_between") {

        SUBCASE("Basic rays") {
            CartesianPoint p(4.0, -2.0);
            std::array<CartesianRay, 13> rays;
            for (int i=0; i<13; i++) {
                double angle = i * M_PI / 6;
                rays[i] = CartesianRay(p, p + Cartesian::from_polar(1, angle));
            }
            for (int i=0; i<12; i++) {
                // rays[3] points straight upward and should have angle_of() = M_PI
                // rays[4] onwards should have negative angle_of()
                // rays[9] poitns straight downward and shoud have angle_of() = 0
                CHECK(NumUtils::is_close(Cartesian::angle_of(rays[i]), ((i + 8) % 12 - 5) * M_PI / 6));
                CHECK(NumUtils::is_close(Cartesian::angle_between(rays[i], rays[i+1]), M_PI / 6));
                CHECK(NumUtils::is_close(Cartesian::angle_between(rays[i+1], rays[i]), -M_PI / 6));
            }
            CHECK(NumUtils::is_close(Cartesian::angle_of(rays[12]), Cartesian::angle_of(rays[0])));

            // We want to maintain consistency that angle_between(r1, r2) = M_PI whenever r1, r2 are opposite:

            // rays[8] points SW, rays[2] points NE
            CHECK(NumUtils::is_close(Cartesian::angle_between(rays[8], rays[2]), M_PI));
            CHECK(NumUtils::is_close(Cartesian::angle_between(rays[2], rays[8]), M_PI));

            // rays[9] points S, rays[3] points N
            CHECK(NumUtils::is_close(Cartesian::angle_between(rays[9], rays[3]), M_PI));
            CHECK(NumUtils::is_close(Cartesian::angle_between(rays[3], rays[9]), M_PI));

            // rays[10] points SE, rays[4] points NW
            CHECK(NumUtils::is_close(Cartesian::angle_between(rays[10], rays[4]), M_PI));
            CHECK(NumUtils::is_close(Cartesian::angle_between(rays[4], rays[10]), M_PI));
        }

        SUBCASE("Angle Arithmetic") {
            CartesianPoint o(0.0, 0.0);
            CartesianPoint p(4.0, -2.0);
            CartesianPoint p2(4.0, 3.0);

            CHECK(NumUtils::is_close(
                Cartesian::angle_of(CartesianRay(o, p2)),
                2 * Cartesian::angle_of(CartesianRay(o, p))
            ));
            CHECK(NumUtils::is_close(
                Cartesian::angle_of(CartesianRay(p2, o)) - M_PI,
                2 * Cartesian::angle_of(CartesianRay(p, o))
            ));

            CartesianPoint q(0, -1.0);
            CHECK(NumUtils::is_close(
                Cartesian::angle_between(CartesianRay(o, q), CartesianRay(o, p)),
                Cartesian::angle_between(CartesianRay(o, p), CartesianRay(o, p2))
            ));
            CHECK(NumUtils::is_close(
                Cartesian::angle_between(CartesianRay(q, o), CartesianRay(p, o)),
                Cartesian::angle_between(CartesianRay(p, o), CartesianRay(p2, o))
            ));
        }

        SUBCASE("Equilateral Triangle") {
            CartesianPoint p1(-9.91, -2.89);
            CartesianPoint p2(-12.33, 4.33);
            CartesianPoint p3(-4.8672965846764, 2.8157814771583);

            double angle12 = Cartesian::angle_of(p1, p2);
            double angle13 = Cartesian::angle_of(p1, p3);
            double angle23 = Cartesian::angle_of(p2, p3);
            CHECK(NumUtils::is_close(angle12, 0.3234116824369 - M_PI));
            CHECK(NumUtils::is_close(angle13, 2.4178067848301));
            CHECK(NumUtils::is_close(angle23, M_PI - 1.7709834199563));
            CHECK(NumUtils::is_close(angle13 - angle12, 5 * M_PI / 3));

            double angle123 = Cartesian::angle_between(p1, p2, p3);
            CHECK(NumUtils::is_close(angle123, M_PI / 3));
            double angle321 = Cartesian::angle_between(p3, p2, p1);;
            CHECK(NumUtils::is_close(angle321, -M_PI / 3));

            double angle213 = Cartesian::angle_between(p2, p1, p3);
            CHECK(NumUtils::is_close(angle213, -M_PI / 3));
            double angle312 = Cartesian::angle_between(p3, p1, p2);
            CHECK(NumUtils::is_close(angle312, M_PI / 3));

            double angle132 = Cartesian::angle_between(p1, p3, p2);;
            CHECK(NumUtils::is_close(angle132, -M_PI / 3));
            double angle231 = Cartesian::angle_between(p2, p3, p1);;
            CHECK(NumUtils::is_close(angle231, M_PI / 3));
        }

        SUBCASE("Counterclockwise addition of vectors; Convex polygon") {
            std::array<CartesianPoint, 12> pts;
            for (int i=0; i<10; i++) {
                double e = NumUtils::urand(0, 0.05);
                double angle = i * M_PI / 5 + e;
                pts[i] = Cartesian::from_polar(1.0, angle);
            }
            pts[10] = pts[0];
            pts[11] = pts[1];
            CartesianPoint o(0.0, 0.0);

            double angle_sum = 0.0;
            for (int i=0; i<10; i++) {
                angle_sum += Cartesian::angle_between(
                    CartesianRay(o, pts[i]),
                    CartesianRay(o, pts[i+1])
                );
            }
            CHECK(NumUtils::is_close(angle_sum, 2 * M_PI));

            angle_sum = 0.0;
            for (int i=0; i<10; i++) {
                // To add up the positive counterclockwise angles, we have to go clockwise
                // around the polygon. Adding the other way around would yield negative angles
                angle_sum += Cartesian::angle_between(
                    CartesianRay(pts[i+1], pts[i+2]),
                    CartesianRay(pts[i+1], pts[i])
                );
            }
            CHECK(NumUtils::is_close(angle_sum, 8 * M_PI));
        }
    }
}