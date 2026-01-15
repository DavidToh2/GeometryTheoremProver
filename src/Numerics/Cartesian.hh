
#pragma once

#include <utility>
#include <vector>
#include <variant>
#include <cmath>

#include "Common/Generator.hh"
#include "Common/NumUtils.hh"

class CartesianPoint {

public:
	double x;
	double y;

    CartesianPoint() : x(0), y(0) {}
    CartesianPoint(double x, double y) : x(x), y(y) {}

    CartesianPoint& operator+=(const CartesianPoint &other);
    CartesianPoint& operator-=(const CartesianPoint &other);
    CartesianPoint& operator*=(double scalar);
    CartesianPoint operator+(const CartesianPoint &other) const;
    CartesianPoint operator-(const CartesianPoint &other) const;
    CartesianPoint operator*(double scalar) const;

    bool is_same(const CartesianPoint& other);
    static bool is_same(const CartesianPoint& p1, const CartesianPoint& p2);

    double distance(const CartesianPoint &other) const;
    static double distance(const CartesianPoint &p1, const CartesianPoint &p2);
};

CartesianPoint operator*(double scalar, const CartesianPoint &point);
void operator*(double scalar, CartesianPoint &point);

class CartesianLine {
public:
	double a;
    double b;
	double c;

    CartesianLine() : a(1), b(0), c(0) {}
    CartesianLine(double a, double b, double c) : a(a), b(b), c(c) {}
    CartesianLine(CartesianPoint p1, CartesianPoint p2) : a(p2.y - p1.y), b(p1.x - p2.x), c(p2.x * p1.y - p1.x * p2.y) {};

    auto operator<=>(const CartesianLine &other) const;

    CartesianLine para_line(const CartesianPoint &p);
    CartesianLine perp_line(const CartesianPoint &p);

    /* Computes the anticlockwise angle of rotation from the y-axis to the line.
    Returns a value in the range [0, pi] using std::atan2. */
    static double angle_of(const CartesianLine& l1);
    /* Computes the angle between two lines.
    Returns a value in the range [-pi, pi] using std::atan2. */
    static double angle_between(const CartesianLine& l1, const CartesianLine& l2);
    /* Computes the anticlockwise angle of rotation from the y-axis to the vector p1->p2.
    Returns a value in the range [-pi, pi] using std::atan2. */
    static double angle_of(const CartesianPoint& p1, const CartesianPoint& p2);
    /* Computes the angle p1p2p3: more precisely, the anticlockwise angle of rotation from
    vector p2->p1 to vector p2->p3.
    Returns a value in the range [-pi, pi] using std::atan2. */
    static double angle_between(const CartesianPoint& p1, const CartesianPoint& p2, const CartesianPoint& p3);
};

class CartesianRay {
    CartesianPoint start;
    CartesianPoint head;

    // TODO: Unimplemented for now.
};

class CartesianCircle {
public:
	CartesianPoint c;
	double r;
};

typedef std::variant<CartesianLine, CartesianRay, CartesianCircle> CartesianObject;

namespace Cartesian {
    static CartesianPoint intersect_line_line(const CartesianLine &l1, const CartesianLine &l2);
    static CartesianPoint intersect_line_line(const CartesianLine &l, const CartesianPoint &p1, const CartesianPoint &p2);
    static std::pair<bool, std::pair<CartesianPoint, CartesianPoint>> intersect_line_circle(const CartesianLine &l, const CartesianCircle &c);
    static std::pair<bool, std::pair<CartesianPoint, CartesianPoint>> intersect_circle_circle(const CartesianCircle &c1, const CartesianCircle &c2);
    Generator<CartesianPoint> intersect(CartesianObject obj1, CartesianObject obj2);

    CartesianPoint from_polar(double r, double theta);

    CartesianPoint random_point();
    std::vector<CartesianPoint> random_points(int n=3);

    /* Random rotate-flip-scale-shift a point cloud. */
    template <typename... T>
    void random_affine(T... points) {
        CartesianPoint c;
        c = (points + ...) / sizeof...(points);
        double angle = NumUtils::urand(0, 2 * M_PI);
        double scale = NumUtils::urand(0.5, 1.5);
    }
};