#include "physics.h"
#include "math.h"
#include <iostream>
#include <cmath>

Vec2 accel(Vec2 r, double mu) {
    double eps = 1e-9;
    double rmag = norm(r);
    if (rmag < eps) {
        rmag = eps;
        std::cerr << "very small radius.\n";
    }
    double factor = -mu / (rmag * rmag * rmag);
    return scale(r, factor);
}
 // passing by reference, updating each step
 void verlet(Vec2& r, Vec2& v, Vec2& a, double dt, double mu) {
     Vec2 r_new = add(r, add(scale(v, dt), scale(a, 0.5 * dt * dt)));
     Vec2 a_new = accel(r_new, mu);
     Vec2 v_new = add(v, scale(add(a, a_new), 0.5 * dt));

     r = r_new;
     v = v_new;
     a = a_new;
 }

