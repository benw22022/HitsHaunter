// Vector2D.cpp
#include "Vector2D.h"
#include <cmath>
#include <algorithm>

Vec2 rotate_vec2D(const Vec2& vec, double theta, const Vec2& origin) {
    Vec2 translated_vec = vec.translate(origin);

    std::array<std::array<double, 2>, 2> rotation_matrix = {{
        {std::cos(theta), -std::sin(theta)},
        {std::sin(theta), std::cos(theta)}
    }};

    Vec2 rotated_vec = translated_vec.apply_matrix(rotation_matrix);
    return rotated_vec + origin;
}

std::pair<double, double> rotate_point(double px, double py, double cx, double cy, double angle) {
    double s = std::sin(angle);
    double c = std::cos(angle);
    px -= cx;
    py -= cy;
    double xnew = px * c - py * s;
    double ynew = px * s + py * c;
    return {xnew + cx, ynew + cy};
}
// Cross product of two vectors
double cross(const Vec2& v1, const Vec2& v2) {
    return v1.x * v2.y - v1.y * v2.x;
}

// Function to check if a point is within the rectangle
bool point_in_rectangle(const Vec2& point, const std::vector<std::pair<double, double>>& rect) {
    std::vector<Vec2> corners;
    for (const auto& p : rect) {
        corners.emplace_back(p.first, p.second);
    }

    Vec2 v1 = corners[1] - corners[0];
    Vec2 v2 = corners[2] - corners[1];
    Vec2 v3 = corners[3] - corners[2];
    Vec2 v4 = corners[0] - corners[3];

    Vec2 p1 = point - corners[0];
    Vec2 p2 = point - corners[1];
    Vec2 p3 = point - corners[2];
    Vec2 p4 = point - corners[3];

    double c1 = cross(v1, p1);
    double c2 = cross(v2, p2);
    double c3 = cross(v3, p3);
    double c4 = cross(v4, p4);

    // Check if all cross products have the same sign
    return (c1 >= 0 && c2 >= 0 && c3 >= 0 && c4 >= 0) || (c1 <= 0 && c2 <= 0 && c3 <= 0 && c4 <= 0);
}

// bool point_in_rectangle(double px, double py, const std::vector<std::pair<double, double>>& rect) {
//     double rx_min = std::min_element(rect.begin(), rect.end(), [](const std::pair<double, double>& a, const std::pair<double, double>& b) { return a.first < b.first; })->first;
//     double rx_max = std::max_element(rect.begin(), rect.end(), [](const std::pair<double, double>& a, const std::pair<double, double>& b) { return a.first > b.first; })->first;
//     double ry_min = std::min_element(rect.begin(), rect.end(), [](const std::pair<double, double>& a, const std::pair<double, double>& b) { return a.second < b.second; })->second;
//     double ry_max = std::max_element(rect.begin(), rect.end(), [](const std::pair<double, double>& a, const std::pair<double, double>& b) { return a.second > b.second; })->second;
//     return rx_min < px && px < rx_max && ry_min < py && py < ry_max;
// }

std::vector<double> linspace(double start, double end, int num) {
    std::vector<double> linspaced;
    if (num == 0) return linspaced;
    if (num == 1) {
        linspaced.push_back(start);
        return linspaced;
    }

    double delta = (end - start) / (num - 1);

    for (int i = 0; i < num - 1; ++i) {
        linspaced.push_back(start + delta * i);
    }
    linspaced.push_back(end);

    return linspaced;
}
