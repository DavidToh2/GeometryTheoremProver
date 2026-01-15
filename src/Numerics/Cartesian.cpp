
#include <array>
#include <cmath>
#include <random>

#include "Cartesian.hh"
#include "Common/Exceptions.hh"
#include "Common/NumUtils.hh"





CartesianPoint& CartesianPoint::operator+=(const CartesianPoint &other) {
    this->x += other.x;
    this->y += other.y;
    return *this;
}
CartesianPoint& CartesianPoint::operator-=(const CartesianPoint &other) {
    this->x -= other.x;
    this->y -= other.y;
    return *this;
}
CartesianPoint& CartesianPoint::operator*=(double scalar) {
    this->x *= scalar;
    this->y *= scalar;
    return *this;
}
CartesianPoint CartesianPoint::operator+(const CartesianPoint &other) const {
    return CartesianPoint(x + other.x, y + other.y);
}
CartesianPoint CartesianPoint::operator-(const CartesianPoint &other) const {
    return CartesianPoint(x - other.x, y - other.y);
}
CartesianPoint CartesianPoint::operator*(double scalar) const {
    return CartesianPoint(x * scalar, y * scalar);
}
CartesianPoint operator*(double scalar, const CartesianPoint &point) {
    return CartesianPoint(scalar * point.x, scalar * point.y);
}
void operator*(double scalar, CartesianPoint &point) {
    point.x *= scalar;
    point.y *= scalar;
}

bool CartesianPoint::is_same(const CartesianPoint& other) {
    return (NumUtils::is_close(x, other.x) && NumUtils::is_close(y, other.y));
}
bool CartesianPoint::is_same(const CartesianPoint& p1, const CartesianPoint& p2) {
    return (NumUtils::is_close(p1.x, p2.x) && NumUtils::is_close(p1.y, p2.y));
}

double CartesianPoint::distance(const CartesianPoint &other) const {
    return std::sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
}
double CartesianPoint::distance(const CartesianPoint &p1, const CartesianPoint &p2) {
    return std::sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}



auto CartesianLine::operator<=>(const CartesianLine &other) const {
    return (b * other.a <=> other.b * a);
}

CartesianLine CartesianLine::para_line(const CartesianPoint &p) {
    return CartesianLine(a, b, -(a * p.x + b * p.y));
}
CartesianLine CartesianLine::perp_line(const CartesianPoint &p) {
    return CartesianLine(p, p + CartesianPoint(a, b));
}

double CartesianLine::angle_of(const CartesianLine& l1) {
    return std::fmod(std::atan2(-l1.a, l1.b) + M_PI_2, M_PI);
}
double CartesianLine::angle_between(const CartesianLine& l1, const CartesianLine& l2) {
    double angle1 = angle_of(l1);
    double angle2 = angle_of(l2);
    return (std::fmod(angle2 - angle1 + M_PI, 2 * M_PI) - M_PI);
}
double CartesianLine::angle_of(const CartesianPoint& p1, const CartesianPoint& p2) {
    return std::fmod(std::atan2(p2.y - p1.y, p2.x - p1.x) + M_PI_2, 2 * M_PI);
}
double CartesianLine::angle_between(const CartesianPoint& p1, const CartesianPoint& p2, const CartesianPoint& p3) {
    double angle1 = angle_of(p2, p1);
    double angle2 = angle_of(p2, p3);
    return (std::fmod(angle2 - angle1, 2 * M_PI) - M_PI);
}




CartesianPoint Cartesian::intersect_line_line(const CartesianLine &l1, const CartesianLine &l2) {
    double den = l1.a * l2.b - l2.a * l1.b;
    if (NumUtils::is_close(den, 0.0)) {
        throw NumericsInternalError("Lines are parallel; no intersection.");
    }
    return CartesianPoint((l2.b * (-l1.c) - l1.b * (-l2.c)) / den, (l1.a * (-l2.c) - l2.a * (-l1.c)) / den);
}
CartesianPoint Cartesian::intersect_line_line(const CartesianLine &l, const CartesianPoint &p1, const CartesianPoint &p2) {
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    double den = l.a * dx + l.b * dy;
    if (NumUtils::is_close(den, 0.0)) {
        throw NumericsInternalError("Line and point-pair are parallel; no intersection.");
    }
    double t = -(l.a * p1.x + l.b * p1.y + l.c) / den;
    return CartesianPoint(p1.x + t * dx, p1.y + t * dy);
}
std::pair<bool, std::pair<CartesianPoint, CartesianPoint>> 
Cartesian::intersect_line_circle(const CartesianLine &l, const CartesianCircle &c) {
    // https://math.stackexchange.com/questions/228841/how-do-i-calculate-the-intersections-of-a-straight-line-and-a-circle

    double p = c.c.x, q = c.c.y, r = c.r;
    if (NumUtils::is_close(l.b, 0.0)) {
        double x = -l.c / l.a;
        double A = l.a * l.a + l.b * l.b;
        double B = 2 * (l.b * l.c + l.a * l.b * p - l.a * l.a * q);
        double C = l.c * l.c + 2 * l.a * l.c * p + l.a * l.a * (p * p + q * q - r * r);
        auto [s, sols] = NumUtils::solve_quadratic(A, B, C);
        if (!s) {
            return {false, {}};
        }
        return {true, {{x, sols.first}, {x, sols.second}}};

    } else if (NumUtils::is_close(l.a, 0.0)) {
        double y = -l.c / l.b;
        double A = l.a * l.a + l.b * l.b;
        double B = 2 * (l.a * l.c + l.a * l.b * q - l.b * l.b * p);
        double C = l.c * l.c + 2 * l.b * l.c * q + l.b * l.b * (p * p + q * q - r * r);
        auto [s, sols] = NumUtils::solve_quadratic(A, B, C);
        if (!s) {
            return {false, {}};
        }
        return {true, {{sols.first, y}, {sols.second, y}}};
        
    } else {
        double A = l.a * l.a + l.b * l.b;
        double B = 2 * (l.a * l.c + l.a * l.b * q - l.b * l.b * p);
        double C = l.c * l.c + 2 * l.b * l.c * q + l.b * l.b * (p * p + q * q - r * r);
        auto [s, sols] = NumUtils::solve_quadratic(A, B, C);
        if (!s) {
            return {false, {}};
        }
        return {true, {{sols.first, (-l.c - l.a * sols.first) / l.b}, {sols.second, (-l.c - l.a * sols.second) / l.b}}};
    }
}
std::pair<bool, std::pair<CartesianPoint, CartesianPoint>> 
Cartesian::intersect_circle_circle(const CartesianCircle &c1, const CartesianCircle &c2) {
    // https://mathworld.wolfram.com/Circle-CircleIntersection.html

    double d = CartesianPoint::distance(c1.c, c2.c);
    if (NumUtils::is_close(d, 0.0) && NumUtils::is_close(c1.r, c2.r)) {
        return {false, {}};
    }
    if (d > c1.r + c2.r || d < std::abs(c1.r - c2.r)) {
        return {false, {}};
    }
    double a = (c1.r * c1.r - c2.r * c2.r + d * d) / (2 * d);
    double h = std::sqrt(c1.r * c1.r - a * a);
    CartesianPoint p0 = c1.c + (a / d) * (c2.c - c1.c);
    CartesianPoint offset = (h / d) * CartesianPoint(-(c2.c.y - c1.c.y), c2.c.x - c1.c.x);
    return {true, {p0 + offset, p0 - offset}};
}

Generator<CartesianPoint> Cartesian::intersect(CartesianObject obj1, CartesianObject obj2) {
    return std::visit( overloaded {
        [&](const CartesianLine &l1, const CartesianLine &l2) -> Generator<CartesianPoint> {
            CartesianPoint p = intersect_line_line(l1, l2);
            co_yield p;
            co_return;
        },
        [&](const CartesianLine &l, const CartesianCircle &c) -> Generator<CartesianPoint> {
            auto [s, sols] = intersect_line_circle(l, c);
            if (s) {
                co_yield sols.first;
                co_yield sols.second;
            }
            co_return;
        },
        [&](const CartesianCircle &c, const CartesianLine &l) -> Generator<CartesianPoint> {
            auto [s, sols] = intersect_line_circle(l, c);
            if (s) {
                co_yield sols.first;
                co_yield sols.second;
            }
            co_return;
        },
        [&](const CartesianCircle &c1, const CartesianCircle &c2) -> Generator<CartesianPoint> {
            auto [s, sols] = intersect_circle_circle(c1, c2);
            if (s) {
                co_yield sols.first;
                co_yield sols.second;
            }
            co_return;
        },
        [&](auto&, auto&) -> Generator<CartesianPoint> {
            throw NumericsInternalError("Intersection not defined for given Cartesian object types.");
        }
    }, obj1, obj2);
}





CartesianPoint Cartesian::from_polar(double r, double theta) {
    return CartesianPoint(r * std::cos(theta), r * std::sin(theta));
}



CartesianPoint Cartesian::random_point() {
    return CartesianPoint(NumUtils::urand(-1, 1), NumUtils::urand(-1, 1));
}
std::vector<CartesianPoint> Cartesian::random_points(int n) {
    std::vector<CartesianPoint> pts(n, random_point());
    for (int i = 0; i < n; ++i) {
        pts.emplace_back(random_point());
    }
    return pts;
}