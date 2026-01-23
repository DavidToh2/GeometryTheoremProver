
#pragma once

#include <optional>
#include <utility>
#include <vector>
#include <variant>
#include <cmath>

#include "Common/Exceptions.hh"
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
    CartesianPoint& operator/=(double scalar);
    CartesianPoint operator+(const CartesianPoint &other) const;
    CartesianPoint operator-(const CartesianPoint &other) const;
    CartesianPoint operator-() const;
    CartesianPoint operator*(double scalar) const;
    CartesianPoint operator/(double scalar) const;

    /* Comparison operator for CartesianPoints. Compares x-coordinates followed by y-coordinates.
    Useful for signed lengths for the AREngine. */
    auto operator<=>(const CartesianPoint &other) const {
        return (x <=> other.x != 0) ? (x <=> other.x) : (y <=> other.y);
    }

    bool is_same(const CartesianPoint& other) const;
    static bool is_same(const CartesianPoint& p1, const CartesianPoint& p2);
    static bool is_close(const CartesianPoint& p1, const CartesianPoint& p2);
    bool operator==(CartesianPoint other) const;

    constexpr double norm() const { return std::sqrt(x * x + y * y); };
    constexpr double norm2() const { return x * x + y * y; };

    /* Flips the point about the y-axis. */
    CartesianPoint& flip();
    /* Performs a counterclockwise rotation by the given angle (in radians) around the origin. */
    CartesianPoint& rotate(double angle);
    /* Performs a homothety on the point with origin as center. */
    CartesianPoint& scale(double factor);
    /* Shifts the point by the given offset. */
    CartesianPoint& shift(const CartesianPoint &offset);

    constexpr std::string to_string() const {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

CartesianPoint operator*(double scalar, const CartesianPoint &point);

namespace Cartesian {

    CartesianPoint from_polar(double r, double theta);

    CartesianPoint random_point();
    std::vector<CartesianPoint> random_points(int n=3);

    /* Computes the distance between two points p1, p2. */
    double distance(const CartesianPoint &p1, const CartesianPoint &p2);
    /* Computes the squared distance between two points p1, p2. */
    double distance2(const CartesianPoint &p1, const CartesianPoint &p2);

    /* Computes the dot product of two vectors. */
    double dot(const CartesianPoint &p1, const CartesianPoint &p2);
}

class CartesianLine {
public:
	double a;
    double b;
	double c;

    CartesianLine() : a(0), b(1), c(0) {}
    CartesianLine(double a, double b, double c) : a(a), b(b), c(c) {}
    CartesianLine(CartesianPoint p1, CartesianPoint p2) : a(p2.y - p1.y), b(p1.x - p2.x), c(p2.x * p1.y - p1.x * p2.y) {};

    bool contains(const CartesianPoint &p) const;

    /* Compares the gradients of the two lines. Useful for signed angles for the AREngine. (Note:
    This functionality is not used as Cartesian::angle_of() is less prone to numerical instability
    and serves the same purpose.) */
    auto operator<=>(const CartesianLine &other) const {
        return (b * other.a <=> other.b * a);
    }

    constexpr std::string to_string() const {
        return std::to_string(a) + "x + " + std::to_string(b) + "y + " + std::to_string(c) + " = 0";
    }
};

class CartesianRay : public CartesianLine {
public:
    CartesianPoint start;
    CartesianPoint head;

    CartesianRay() : start(0, 0), head(1, 0), CartesianLine(0, 1, 0) {}
    CartesianRay(CartesianPoint start, CartesianPoint head) : start(start), head(head), CartesianLine(start, head) {}

    bool contains(const CartesianPoint &p) const;
};

class CartesianCircle {
public:
	CartesianPoint c;
	double r;

    CartesianCircle() : c(0, 0), r(1) {}
    CartesianCircle(CartesianPoint c, double r): c(c), r(r) {}
    CartesianCircle(CartesianPoint c, CartesianPoint p): c(c), r(Cartesian::distance(c, p)) {}
    CartesianCircle(CartesianPoint p1, CartesianPoint p2, CartesianPoint p3);

    bool contains(const CartesianPoint &p) const;

    constexpr std::string to_string() const {
        return "Circ[ (" + c.to_string() + "), " + std::to_string(r) + " ]";
    }
};

typedef std::variant<CartesianLine, CartesianRay, CartesianCircle> CartesianObject;

// template<typename T>
// concept isCartesianObject = std::same_as<T, CartesianLine> || std::same_as<T, CartesianRay> || std::same_as<T, CartesianCircle>;

template<typename T>
concept isCartesianLineOrRay = std::same_as<T, CartesianLine> || std::same_as<T, CartesianRay>;

namespace Cartesian {

    std::optional<CartesianPoint> intersect(const CartesianLine &l1, const CartesianLine &l2);
    std::optional<CartesianPoint> intersect(const CartesianLine &l, const CartesianPoint &p1, const CartesianPoint &p2);

    std::optional<CartesianPoint> intersect(const CartesianRay &r1, const CartesianRay &r2);
    std::optional<CartesianPoint> intersect(const CartesianLine &l, const CartesianRay &r);
    std::optional<CartesianPoint> intersect(const CartesianRay &r, const CartesianLine &l);

    std::pair<std::optional<CartesianPoint>, std::optional<CartesianPoint>> intersect(const CartesianLine &l, const CartesianCircle &c);
    std::pair<std::optional<CartesianPoint>, std::optional<CartesianPoint>> intersect(const CartesianCircle &c, const CartesianLine &l);
    std::pair<std::optional<CartesianPoint>, std::optional<CartesianPoint>> intersect(const CartesianRay &r, const CartesianCircle &c);
    std::pair<std::optional<CartesianPoint>, std::optional<CartesianPoint>> intersect(const CartesianCircle &c, const CartesianRay &r);
    std::pair<std::optional<CartesianPoint>, std::optional<CartesianPoint>> intersect(const CartesianCircle &c1, const CartesianCircle &c2);

    template <typename T, typename U> 
    std::optional<CartesianPoint> intersect(const T &obj1, const U &obj2) {
        throw NumericsInternalError("Unhandled CartesianObject intersection types.");
    }
    
    /* Polymorphic function that intersects two CartesianObjects with dynamically determined type.
    Note: Because this function has to be dynamic polymorphic, so we can't use concepts here. */
    Generator<CartesianPoint> intersect(CartesianObject obj1, CartesianObject obj2);

    CartesianPoint get_random_point_on_line(CartesianLine &line, CartesianPoint near = {0, 0}, double max_dist = 10.0);
    CartesianPoint get_random_point_on_circle(CartesianCircle &circle);
    CartesianPoint get_random_point_on_ray(CartesianRay &ray, CartesianPoint near = {0, 0}, double max_dist = 10.0);
    CartesianPoint get_random_point(CartesianObject &obj, CartesianPoint near = {0,0}, double max_dist = 10.0);

    /* Applies a random affine transform on a set of points. */
    template <typename... T>
    requires (std::same_as<T, CartesianPoint> && ...)
    void random_affine(T&... points) {
        CartesianPoint c;
        c = (points + ...) / sizeof...(points);
        double angle = NumUtils::urand(0, 2 * M_PI);
        double scale = NumUtils::urand(0.25, 1.75);
        CartesianPoint shift = CartesianPoint(NumUtils::urand(-1, 1), NumUtils::urand(-1, 1));
        (points.shift(-c).rotate(angle).scale(scale).shift(shift), ...);
    }

    /* Computes the anticlockwise angle of rotation from the y-axis to the line.
    Returns a value in the range [0, pi] using std::atan2. */
    double angle_of(const CartesianLine& l1);
    /* Computes the anticlockwise angle of rotation from the vertically downward y-axis to
    the ray. Note: this is 90 degrees more than the polar angle.
    Returns a value in the range [-pi, pi]. */
    double angle_of(const CartesianRay& r);
    /* Computes the angle between two lines: more precisely, the anticlockwise angle of
    rotation from l1 to l2.
    Returns a value in the range [0, pi]. */
    double angle_between(const CartesianLine& l1, const CartesianLine& l2);
    /* Computes the angle between two rays: more precisely, the anticlockwise angle of
    rotation from r1 to r2.
    Returns a value in the range [-pi, pi].
    Note: Always returns pi if the two rays are pointing in opposite directions. */
    double angle_between(const CartesianRay& r1, const CartesianRay& r2);
    /* Computes the anticlockwise angle of rotation from the vertically downward y-axis to 
    the vector p1->p2.
    Returns a value in the range [-pi, pi]. */
    double angle_of(const CartesianPoint& p1, const CartesianPoint& p2);
    /* Computes the angle p1p2p3: more precisely, the anticlockwise angle of rotation from
    vector p2->p1 to vector p2->p3.
    Returns a value in the range [-pi, pi]. */
    double angle_between(const CartesianPoint& p1, const CartesianPoint& p2, const CartesianPoint& p3);


    constexpr CartesianPoint midpoint(const CartesianPoint &p1, const CartesianPoint &p2) { return (p1 + p2) / 2; }
    /* Given a vector AB, rotates it 90 degrees counterclockwise to vector AC */
    constexpr CartesianPoint perp_vec_p(const CartesianPoint &a, const CartesianPoint &b) { return CartesianPoint(a.y - b.y, b.x - a.x); }
    /* Given a vector AB, rotates it 90 degrees clockwise to vector AC */
    constexpr CartesianPoint perp_vec_n(const CartesianPoint &a, const CartesianPoint &b) { return CartesianPoint(b.y - a.y, a.x - b.x); }

    CartesianLine para_line(const CartesianPoint &p, const CartesianLine &l);
    CartesianLine perp_line(const CartesianPoint &p, const CartesianLine &l);
    CartesianLine perp_bisect(const CartesianPoint &p1, const CartesianPoint &p2);
    CartesianPoint foot(const CartesianPoint &p, const CartesianLine &l);
    CartesianPoint reflect(const CartesianPoint &p, const CartesianLine &l);
    

};