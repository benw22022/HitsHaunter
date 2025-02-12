#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <vector>
#include <stdexcept>
#include <array>
#include <algorithm>
#include <cmath>

struct Vec2 {
    double x, y;

    Vec2(double x = 0, double y = 0) : x(x), y(y) {}

    Vec2 translate(const Vec2& origin) const {
        return Vec2{x - origin.x, y - origin.y};
    }

    Vec2 apply_matrix(const std::array<std::array<double, 2>, 2>& matrix) const {
        return Vec2{x * matrix[0][0] + y * matrix[0][1], x * matrix[1][0] + y * matrix[1][1]};
    }

    Vec2 operator+(const Vec2& other) const {
        return Vec2{x + other.x, y + other.y};
    }
    
    // Vector subtraction
    Vec2 operator-(const Vec2& other) const {
        return Vec2(x - other.x, y - other.y);
    }
};

// Function declarations
Vec2 rotate_vec2D(const Vec2& vec, double theta, const Vec2& origin = {0, 0});
std::pair<double, double> rotate_point(double px, double py, double cx, double cy, double angle);
bool point_in_rectangle(const Vec2& point, const std::vector<std::pair<double, double>>& rect);
std::vector<double> linspace(double start, double end, int num);

class Vector2D {
public:
    Vec2 direction;
    Vec2 anchor;

    Vector2D(const Vec2& dir, const Vec2& anc) : direction(dir), anchor(anc) {}

    Vector2D rotate(double theta, const Vec2& origin = {0, 0}, bool modify = false) {
        Vec2 translated_vec = anchor.translate(origin);
        std::array<std::array<double, 2>, 2> rotation_matrix = {{
            {std::cos(theta), -std::sin(theta)},
            {std::sin(theta), std::cos(theta)}
        }};

        Vec2 rotated_direction = direction.apply_matrix(rotation_matrix);
        Vec2 rotated_anchor = translated_vec.apply_matrix(rotation_matrix);

        if (!modify) {
            return Vector2D(rotated_direction, rotated_anchor + origin);
        }

        direction = rotated_direction;
        anchor = rotated_anchor + origin;
        return *this;
    }

    std::string to_string() const {
        return "Vector2D: direction=(" + std::to_string(direction.x) + ", " + std::to_string(direction.y) +
               ") anchor=(" + std::to_string(anchor.x) + ", " + std::to_string(anchor.y) + ")";
    }

    Vec2 compute_intercept(const Vector2D& other) const {
        std::array<std::array<double, 2>, 2> direction_matrix = {{
            {direction.x, other.direction.x},
            {direction.y, other.direction.y}
        }};

        Vec2 b = {other.anchor.x - anchor.x, other.anchor.y - anchor.y};

        double det = direction_matrix[0][0] * direction_matrix[1][1] - direction_matrix[0][1] * direction_matrix[1][0];
        if (det == 0) {
            throw std::runtime_error("Error: Lines are parallel");
        }

        double t = (b.x * direction_matrix[1][1] - b.y * direction_matrix[0][1]) / det;
        double u = (b.y * direction_matrix[0][0] - b.x * direction_matrix[1][0]) / det;

        return {anchor.x + t * direction.x, anchor.y + t * direction.y};
    }

    Vec2 get_end(double length) const {
        return {anchor.x + direction.x * length, anchor.y + direction.y * length};
    }

    Vector2D translate(const Vec2& amount, bool modify = false) {
        Vec2 new_anchor = anchor + amount;
        if (!modify) {
            return Vector2D(direction, new_anchor);
        }

        anchor = new_anchor;
        return *this;
    }

    double distance_to_point(const Vec2& point, double length) const {
        double x0 = point.x;
        double y0 = point.y;
        double x1 = anchor.x;
        double y1 = anchor.y;
        double x2 = get_end(length).x;
        double y2 = get_end(length).y;

        double numerator = std::abs((y2 - y1) * x0 - (x2 - x1) * y0 + x2 * y1 - y2 * x1);
        double denominator = std::sqrt(std::pow(y2 - y1, 2) + std::pow(x2 - x1, 2));

        return numerator / denominator;
    }

    bool operator<(const Vector2D& other) const {
        return true;
    }
};

#endif // VECTOR2D_H
