
#include <cmath>

#include "Cartesian.hh"
#include "Common/Exceptions.hh"
#include "Common/NumUtils.hh"

EqPoint& EqPoint::operator+=(const EqPoint &other) {
    this->x += other.x;
    this->y += other.y;
    return *this;
}
EqPoint& EqPoint::operator-=(const EqPoint &other) {
    this->x -= other.x;
    this->y -= other.y;
    return *this;
}
EqPoint& EqPoint::operator*=(double scalar) {
    this->x *= scalar;
    this->y *= scalar;
    return *this;
}
EqPoint EqPoint::operator+(const EqPoint &other) const {
    return EqPoint(x + other.x, y + other.y);
}
EqPoint EqPoint::operator-(const EqPoint &other) const {
    return EqPoint(x - other.x, y - other.y);
}
EqPoint EqPoint::operator*(double scalar) const {
    return EqPoint(x * scalar, y * scalar);
}
EqPoint operator*(double scalar, const EqPoint &point) {
    return EqPoint(scalar * point.x, scalar * point.y);
}
void operator*(double scalar, EqPoint &point) {
    point.x *= scalar;
    point.y *= scalar;
}

bool EqPoint::is_same(const EqPoint& other) {
    return (NumUtils::is_close(x, other.x) && NumUtils::is_close(y, other.y));
}
bool EqPoint::is_same(const EqPoint& p1, const EqPoint& p2) {
    return (NumUtils::is_close(p1.x, p2.x) && NumUtils::is_close(p1.y, p2.y));
}

double EqPoint::distance(const EqPoint &other) const {
    return std::sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
}
double EqPoint::distance(const EqPoint &p1, const EqPoint &p2) {
    return std::sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}



auto EqLine::operator<=>(const EqLine &other) const {
    return (b * other.a <=> other.b * a);
}

EqLine EqLine::para_line(const EqPoint &p) {
    return EqLine(a, b, -(a * p.x + b * p.y));
}
EqLine EqLine::perp_line(const EqPoint &p) {
    return EqLine(p, p + EqPoint(a, b));
}

double EqLine::angle_of(const EqLine& l1) {
    return std::fmod(std::atan2(-l1.a, l1.b) + M_PI_2, M_PI);
}
double EqLine::angle_between(const EqLine& l1, const EqLine& l2) {
    double angle1 = angle_of(l1);
    double angle2 = angle_of(l2);
    return (std::fmod(angle2 - angle1 + M_PI, 2 * M_PI) - M_PI);
}
double EqLine::angle_of(const EqPoint& p1, const EqPoint& p2) {
    return std::fmod(std::atan2(p2.y - p1.y, p2.x - p1.x) + M_PI_2, 2 * M_PI);
}
double EqLine::angle_between(const EqPoint& p1, const EqPoint& p2, const EqPoint& p3) {
    double angle1 = angle_of(p2, p1);
    double angle2 = angle_of(p2, p3);
    return (std::fmod(angle2 - angle1, 2 * M_PI) - M_PI);
}



namespace Cartesian {
    EqPoint intersect_line_line(const EqLine &l1, const EqLine &l2) {
        double den = l1.a * l2.b - l2.a * l1.b;
        if (NumUtils::is_close(den, 0.0)) {
            throw NumericsError("Lines are parallel; no intersection.");
        }
        return EqPoint((l2.b * (-l1.c) - l1.b * (-l2.c)) / den, (l1.a * (-l2.c) - l2.a * (-l1.c)) / den);
    }
    EqPoint intersect_line_line(const EqLine &l, const EqPoint &p1, const EqPoint &p2) {
        double dx = p2.x - p1.x;
        double dy = p2.y - p1.y;
        double den = l.a * dx + l.b * dy;
        if (NumUtils::is_close(den, 0.0)) {
            throw NumericsError("Line and point-pair are parallel; no intersection.");
        }
        double t = -(l.a * p1.x + l.b * p1.y + l.c) / den;
        return EqPoint(p1.x + t * dx, p1.y + t * dy);
    }
    std::pair<bool, std::pair<EqPoint, EqPoint>> intersect_line_circle(const EqLine &l, const EqCircle &c) {
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
    std::pair<bool, std::pair<EqPoint, EqPoint>> intersect_circle_circle(const EqCircle &c1, const EqCircle &c2) {
        // https://mathworld.wolfram.com/Circle-CircleIntersection.html

        double d = EqPoint::distance(c1.c, c2.c);
        if (NumUtils::is_close(d, 0.0) && NumUtils::is_close(c1.r, c2.r)) {
            return {false, {}};
        }
        if (d > c1.r + c2.r || d < std::abs(c1.r - c2.r)) {
            return {false, {}};
        }
        double a = (c1.r * c1.r - c2.r * c2.r + d * d) / (2 * d);
        double h = std::sqrt(c1.r * c1.r - a * a);
        EqPoint p0 = c1.c + (a / d) * (c2.c - c1.c);
        EqPoint offset = (h / d) * EqPoint(-(c2.c.y - c1.c.y), c2.c.x - c1.c.x);
        return {true, {p0 + offset, p0 - offset}};
    }
};