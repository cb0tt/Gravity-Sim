#ifndef MATH_H_INCLUDED
#define MATH_H_INCLUDED
#include <cmath>

struct Vec2 {
    double x, y;
};

 Vec2 add(Vec2 a, Vec2 b);
 Vec2 sub(Vec2 a, Vec2 b);
 Vec2 scale(Vec2 a, double k);
 double dot(Vec2 a, Vec2 b);
 double norm(Vec2 a);

#endif // MATH_H_INCLUDED
