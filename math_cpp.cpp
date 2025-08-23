#include "math.h"
#include <iostream>
#include <cmath>

 using namespace std;

 Vec2 add(Vec2 a, Vec2 b) { return {a.x + b.x, a.y + b.y}; }
 Vec2 sub(Vec2 a, Vec2 b) { return {a.x - b.x, a.y - b.y}; }
 Vec2 scale(Vec2 a, double k) { return {a.x * k, a.y * k}; }
 double dot(Vec2 a, Vec2 b) { return a.x * b.x + a.y * b.y; }
 double norm(Vec2 a) { return std::sqrt(dot(a, a)); }

