
#pragma once

#include <utility>

class EqPoint {

public:
	double x;
	double y;

    EqPoint() : x(0), y(0) {}
    EqPoint(double x, double y) : x(x), y(y) {}

    EqPoint& operator+=(const EqPoint &other);
    EqPoint& operator-=(const EqPoint &other);
    EqPoint& operator*=(double scalar);
    EqPoint operator+(const EqPoint &other) const;
    EqPoint operator-(const EqPoint &other) const;
    EqPoint operator*(double scalar) const;

    bool is_same(const EqPoint& other);
    static bool is_same(const EqPoint& p1, const EqPoint& p2);

    double distance(const EqPoint &other) const;
    static double distance(const EqPoint &p1, const EqPoint &p2);
};

EqPoint operator*(double scalar, const EqPoint &point);
void operator*(double scalar, EqPoint &point);

class EqLine {
public:
	double a;
    double b;
	double c;

    EqLine() : a(1), b(0), c(0) {}
    EqLine(double a, double b, double c) : a(a), b(b), c(c) {}
    EqLine(EqPoint p1, EqPoint p2) : a(p2.y - p1.y), b(p1.x - p2.x), c(p2.x * p1.y - p1.x * p2.y) {};

    auto operator<=>(const EqLine &other) const;

    EqLine para_line(const EqPoint &p);
    EqLine perp_line(const EqPoint &p);

    /* Computes the anticlockwise angle of rotation from the y-axis to the line.
    Returns a value in the range [0, pi] using std::atan2. */
    static double angle_of(const EqLine& l1);
    /* Computes the angle between two lines.
    Returns a value in the range [-pi, pi] using std::atan2. */
    static double angle_between(const EqLine& l1, const EqLine& l2);
    /* Computes the anticlockwise angle of rotation from the y-axis to the vector p1->p2.
    Returns a value in the range [-pi, pi] using std::atan2. */
    static double angle_of(const EqPoint& p1, const EqPoint& p2);
    /* Computes the angle p1p2p3: more precisely, the anticlockwise angle of rotation from
    vector p2->p1 to vector p2->p3.
    Returns a value in the range [-pi, pi] using std::atan2. */
    static double angle_between(const EqPoint& p1, const EqPoint& p2, const EqPoint& p3);
};

class EqRay {
    EqPoint start;
    EqPoint head;
};

class EqCircle {
public:
	EqPoint c;
	double r;
};

namespace Cartesian {
    EqPoint intersect_line_line(const EqLine &l1, const EqLine &l2);
    EqPoint intersect_line_line(const EqLine &l, const EqPoint &p1, const EqPoint &p2);
    std::pair<bool, std::pair<EqPoint, EqPoint>> intersect_line_circle(const EqLine &l, const EqCircle &c);
    std::pair<bool, std::pair<EqPoint, EqPoint>> intersect_circle_circle(const EqCircle &c1, const EqCircle &c2);
};