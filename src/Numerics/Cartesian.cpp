
#include <array>
#include <cmath>
#include <random>

#include "Cartesian.hh"
#include "Common/Exceptions.hh"
#include "Common/NumUtils.hh"
#include "DD/DDEngine.hh"





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
CartesianPoint& CartesianPoint::operator/=(double scalar) {
    this->x /= scalar;
    this->y /= scalar;
    return *this;
}
CartesianPoint CartesianPoint::operator+(const CartesianPoint &other) const {
    return CartesianPoint(x + other.x, y + other.y);
}
CartesianPoint CartesianPoint::operator-(const CartesianPoint &other) const {
    return CartesianPoint(x - other.x, y - other.y);
}
CartesianPoint CartesianPoint::operator-() const {
    return CartesianPoint(-x, -y);
}
CartesianPoint CartesianPoint::operator*(double scalar) const {
    return CartesianPoint(x * scalar, y * scalar);
}
CartesianPoint CartesianPoint::operator/(double scalar) const {
    return CartesianPoint(x / scalar, y / scalar);
}
CartesianPoint operator*(double scalar, const CartesianPoint &point) {
    return CartesianPoint(scalar * point.x, scalar * point.y);
}

auto CartesianPoint::operator<=>(const CartesianPoint &other) const {
    return (x <=> other.x != 0) ? (x <=> other.x) : (y <=> other.y);
}

bool CartesianPoint::is_same(const CartesianPoint& other) const {
    return (NumUtils::is_close(x, other.x) && NumUtils::is_close(y, other.y));
}
bool CartesianPoint::is_same(const CartesianPoint& p1, const CartesianPoint& p2) {
    return (NumUtils::is_close(p1.x, p2.x) && NumUtils::is_close(p1.y, p2.y));
}
bool CartesianPoint::operator==(CartesianPoint other) const {
    return (NumUtils::is_close(x, other.x) && NumUtils::is_close(y, other.y));
}
bool CartesianPoint::is_close(const CartesianPoint& p1, const CartesianPoint& p2) {
    return (NumUtils::is_close_2(p1.x, p2.x) && NumUtils::is_close_2(p1.y, p2.y));
}


CartesianPoint& CartesianPoint::flip() {
    x = -x;
    return *this;
}
CartesianPoint& CartesianPoint::rotate(double angle) {
    double new_x = x * std::cos(angle) - y * std::sin(angle);
    double new_y = x * std::sin(angle) + y * std::cos(angle);
    x = new_x;
    y = new_y;
    return *this;
}
CartesianPoint& CartesianPoint::scale(double factor) {
    *this *= factor;
    return *this;
}
CartesianPoint& CartesianPoint::shift(const CartesianPoint &offset) {
    *this += offset;
    return *this;
}




auto CartesianLine::operator<=>(const CartesianLine &other) const {
    return (b * other.a <=> other.b * a);
}
bool CartesianLine::contains(const CartesianPoint &p) const {
    return NumUtils::is_close(a * p.x + b * p.y + c, 0.0);
}
bool CartesianRay::contains(const CartesianPoint &p) const {
    if (!CartesianLine::contains(p)) {
        return false;
    }
    return (Cartesian::dot(head - start, p - start) >= 0);
}



CartesianCircle::CartesianCircle(CartesianPoint p1, CartesianPoint p2, CartesianPoint p3) {
    // https://mathworld.wolfram.com/Circumcircle.html
    double D = 2 * (p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y));
    if (NumUtils::is_close(D, 0.0)) {
        throw NumericsInternalError("Points are collinear; no circumcircle.");
    }
    double S1 = p1.norm2(), S2 = p2.norm2(), S3 = p3.norm2();
    double ux = (S1 * (p2.y - p3.y) + S2 * (p3.y - p1.y) + S3 * (p1.y - p2.y)) / D;
    double uy = (S1 * (p3.x - p2.x) + S2 * (p1.x - p3.x) + S3 * (p2.x - p1.x)) / D;
    c = CartesianPoint(ux, uy);
    r = Cartesian::distance(c, p1);
}
bool CartesianCircle::contains(const CartesianPoint &p) const {
    return NumUtils::is_close(Cartesian::distance2(c, p), r * r);
}



namespace Cartesian {

std::optional<CartesianPoint> intersect(const CartesianLine &l1, const CartesianLine &l2) {
    double den = l1.a * l2.b - l2.a * l1.b;
    if (NumUtils::is_close(den, 0.0)) {
        return std::nullopt;
    }
    return CartesianPoint((l2.b * (-l1.c) - l1.b * (-l2.c)) / den, (l1.a * (-l2.c) - l2.a * (-l1.c)) / den);
}
std::optional<CartesianPoint> intersect(const CartesianLine &l, const CartesianPoint &p1, const CartesianPoint &p2) {
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    double den = l.a * dx + l.b * dy;
    if (NumUtils::is_close(den, 0.0)) {
        return std::nullopt;
    }
    double t = -(l.a * p1.x + l.b * p1.y + l.c) / den;
    return CartesianPoint(p1.x + t * dx, p1.y + t * dy);
}



std::optional<CartesianPoint> intersect(const CartesianRay &r1, const CartesianRay &r2) {
    auto p = intersect(static_cast<const CartesianLine&>(r1), static_cast<const CartesianLine&>(r2));
    if (p.has_value() && r1.contains(*p) && r2.contains(*p)) {
        return p;
    }
    return std::nullopt;
}
std::optional<CartesianPoint> intersect(const CartesianLine &l, const CartesianRay &r) {
    auto p = intersect(l, static_cast<const CartesianLine&>(r));
    if (p.has_value() && r.contains(*p)) {
        return p;
    }
    return std::nullopt;
}
std::optional<CartesianPoint> intersect(const CartesianRay &r, const CartesianLine &l) {
    return intersect(l, r);
}





std::pair<std::optional<CartesianPoint>, std::optional<CartesianPoint>>
intersect(const CartesianLine &l, const CartesianCircle &c) {
    // https://math.stackexchange.com/questions/228841/how-do-i-calculate-the-intersections-of-a-straight-line-and-a-circle

    double p = c.c.x, q = c.c.y, r = c.r;
    if (NumUtils::is_close(l.b, 0.0)) {
        double x = -l.c / l.a;
        double A = l.a * l.a + l.b * l.b;
        double B = 2 * (l.b * l.c + l.a * l.b * p - l.a * l.a * q);
        double C = l.c * l.c + 2 * l.a * l.c * p + l.a * l.a * (p * p + q * q - r * r);
        auto [s, sols] = NumUtils::solve_quadratic(A, B, C);
        if (!s) {
            return {std::nullopt, std::nullopt};
        }
        return {{{x, sols.first}}, {{x, sols.second}}};

    } else if (NumUtils::is_close(l.a, 0.0)) {
        double y = -l.c / l.b;
        double A = l.a * l.a + l.b * l.b;
        double B = 2 * (l.a * l.c + l.a * l.b * q - l.b * l.b * p);
        double C = l.c * l.c + 2 * l.b * l.c * q + l.b * l.b * (p * p + q * q - r * r);
        auto [s, sols] = NumUtils::solve_quadratic(A, B, C);
        if (!s) {
            return {std::nullopt, std::nullopt};
        }
        return {{{sols.first, y}}, {{sols.second, y}}};
        
    } else {
        double A = l.a * l.a + l.b * l.b;
        double B = 2 * (l.a * l.c + l.a * l.b * q - l.b * l.b * p);
        double C = l.c * l.c + 2 * l.b * l.c * q + l.b * l.b * (p * p + q * q - r * r);
        auto [s, sols] = NumUtils::solve_quadratic(A, B, C);
        if (!s) {
            return {std::nullopt, std::nullopt};
        }
        return {{{sols.first, (-l.c - l.a * sols.first) / l.b}}, {{sols.second, (-l.c - l.a * sols.second) / l.b}}};
    }
}
std::pair<std::optional<CartesianPoint>, std::optional<CartesianPoint>>
intersect(const CartesianCircle &c, const CartesianLine &l) {
    return intersect(l, c);
}

std::pair<std::optional<CartesianPoint>, std::optional<CartesianPoint>>
intersect(const CartesianRay &r, const CartesianCircle &c) {
    auto [p1, p2] = intersect(static_cast<const CartesianLine&>(r), c);
    
    if (p2.has_value() && !r.contains(p2.value())) {
        p2 = std::nullopt;
    }
    if (p1.has_value() && !r.contains(p1.value())) {
        if (p2.has_value()) {
            std::swap(p1, p2);
            p2 = std::nullopt;
        } else {
            p1 = std::nullopt;
        }
    }
    return {p1, p2};
}
std::pair<std::optional<CartesianPoint>, std::optional<CartesianPoint>>
intersect(const CartesianCircle &c, const CartesianRay &r) {
    return intersect(r, c);
}

std::pair<std::optional<CartesianPoint>, std::optional<CartesianPoint>>
intersect(const CartesianCircle &c1, const CartesianCircle &c2) {
    // https://mathworld.wolfram.com/Circle-CircleIntersection.html

    double d = distance(c1.c, c2.c);
    if (NumUtils::is_close(d, 0.0) && NumUtils::is_close(c1.r, c2.r)) {
        return {std::nullopt, std::nullopt};
    }
    if (d > c1.r + c2.r || d < std::abs(c1.r - c2.r)) {
        return {std::nullopt, std::nullopt};
    }
    double a = (c1.r * c1.r - c2.r * c2.r + d * d) / (2 * d);
    double h = std::sqrt(c1.r * c1.r - a * a);
    CartesianPoint p0 = c1.c + (a / d) * (c2.c - c1.c);
    CartesianPoint offset = (h / d) * CartesianPoint(-(c2.c.y - c1.c.y), c2.c.x - c1.c.x);
    return {{p0 + offset}, {p0 - offset}};
}

Generator<CartesianPoint> intersect(CartesianObject obj1, CartesianObject obj2) {
    return std::visit([&](const auto &o1, const auto &o2) -> Generator<CartesianPoint> {
        auto res = intersect(o1, o2);

        if constexpr (std::is_same_v<decltype(res), CartesianPoint>) {
            co_yield res;
        } else if constexpr (std::is_same_v<decltype(res), std::optional<CartesianPoint>>) {
            if (res) {
                co_yield res.value();
            }
        } else if constexpr (std::is_same_v<decltype(res), std::pair<std::optional<CartesianPoint>, std::optional<CartesianPoint>>>) {
            if (res.first) {
                co_yield res.first.value();
            }
            if (res.second) {
                co_yield res.second.value();
            }
        } else {
            throw NumericsInternalError("Unhandled return type in Cartesian::intersect.");
        }
        co_return;
    }, obj1, obj2);
}



CartesianPoint get_random_point_on_line(CartesianLine &line, CartesianPoint &near, double max_dist) {
    auto [p1, p2] = intersect(line, CartesianCircle(near, max_dist));
    if (p1 && p2) {
        double r = NumUtils::urand(0.1, 0.9);
        return p1.value() * (1 - r) + p2.value() * r;
    }
    throw NumericsInternalError("Unable to satisfy max_dist requirement getting random point on line.");
}
CartesianPoint get_random_point_on_circle(CartesianCircle &circle) {
    double theta = NumUtils::urand(0, 2 * M_PI);
    return CartesianPoint(
        circle.c.x + circle.r * std::cos(theta),
        circle.c.y + circle.r * std::sin(theta)
    );
}
CartesianPoint get_random_point(CartesianObject &obj, CartesianPoint near, double max_dist) {
    return std::visit( overloaded {
        [&](CartesianLine &line) -> CartesianPoint {
            return get_random_point_on_line(line, near, max_dist);
        },
        [&](CartesianCircle &circle) -> CartesianPoint {
            return get_random_point_on_circle(circle);
        },
        [&](auto&) -> CartesianPoint {
            throw NumericsInternalError("Random point generation not defined for given Cartesian object type.");
        }
    }, obj);
}





CartesianPoint from_polar(double r, double theta) {
    return CartesianPoint(r * std::cos(theta), r * std::sin(theta));
}



CartesianPoint random_point() {
    return CartesianPoint(NumUtils::urand(-1, 1), NumUtils::urand(-1, 1));
}
std::vector<CartesianPoint> random_points(int n) {
    std::vector<CartesianPoint> pts(n, random_point());
    for (int i = 0; i < n; ++i) {
        pts.emplace_back(random_point());
    }
    return pts;
}


double distance(const CartesianPoint &p1, const CartesianPoint &p2) {
    return std::hypot(p1.x - p2.x, p1.y - p2.y);
}
double distance2(const CartesianPoint &p1, const CartesianPoint &p2) {
    return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
}
double dot(const CartesianPoint &p1, const CartesianPoint &p2) {
    return p1.x * p2.x + p1.y * p2.y;
}
double angle_of(const CartesianLine& l1) {
    return std::fmod(std::atan2(-l1.a, l1.b) + M_PI + M_PI_2, M_PI);
}
double angle_of(const CartesianRay& r) {
    return angle_of(r.start, r.head);
}
double angle_between(const CartesianLine& l1, const CartesianLine& l2) {
    double angle1 = angle_of(l1);
    double angle2 = angle_of(l2);
    double res = angle2 - angle1;
    return res < -TOL ? res + M_PI : res;
}
double angle_between(const CartesianRay& r1, const CartesianRay& r2) {
    double angle1 = angle_of(r1);
    double angle2 = angle_of(r2);
    double res = angle2 - angle1;
    return res < -M_PI + TOL ? res + 2 * M_PI : 
        res > M_PI + TOL ? res - 2 * M_PI : res;
}
double angle_of(const CartesianPoint& p1, const CartesianPoint& p2) {
    // same as the std::atan2 of the vector 90-degrees counterclockwise to p1->p2
    // this vector has y-displacement p2.x-p1.x and x-displacement p1.y-p2.y
    return std::atan2(p2.x - p1.x, p1.y - p2.y);
}
double angle_between(const CartesianPoint& p1, const CartesianPoint& p2, const CartesianPoint& p3) {
    double angle1 = angle_of(p2, p1);
    double angle2 = angle_of(p2, p3);
    double res = angle2 - angle1;
    return res < -M_PI + TOL ? res + 2 * M_PI : 
        res > M_PI + TOL ? res - 2 * M_PI : res;
}

CartesianLine para_line(const CartesianPoint &p, const CartesianLine &l) {
    return CartesianLine(l.a, l.b, -(l.a * p.x + l.b * p.y));
}
CartesianLine perp_line(const CartesianPoint &p, const CartesianLine &l) {
    return CartesianLine(l.b, -l.a, l.a * p.y - l.b * p.x);
}
CartesianLine perp_bisect(const CartesianPoint &p1, const CartesianPoint &p2) {
    return CartesianLine( 2 * (p2.x - p1.x), 2 * (p2.y - p1.y),
                          p1.x * p1.x - p2.x * p2.x + p1.y * p1.y - p2.y * p2.y );
}

CartesianPoint foot(const CartesianPoint &p, const CartesianLine &l) {
    return CartesianPoint(
        p.x - l.a * (l.a * p.x + l.b * p.y + l.c) / (l.a * l.a + l.b * l.b),
        p.y - l.b * (l.a * p.x + l.b * p.y + l.c) / (l.a * l.a + l.b * l.b)
    );
}
CartesianPoint reflect(const CartesianPoint &p, const CartesianLine &l) {
    CartesianPoint f = foot(p, l);
    return f * 2 - p;
}



} // namespace Cartesian