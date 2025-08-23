#ifndef PHYSICS_H_INCLUDED
#define PHYSICS_H_INCLUDED
#include "math.h"

 Vec2 accel(Vec2 r, double mu);
 void verlet(Vec2& r, Vec2& v, Vec2& a, double dt, double mu);






#endif // PHYSICS_H_INCLUDED
