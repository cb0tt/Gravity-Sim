# Gravity Simulator (C++/SFML) 🌌

A 2D orbit/gravity simulator written in C++ using SFML.  
It models orbital motion using the **velocity–Verlet integrator**, which is derived from a Taylor expansion of position and the trapezoidal rule for velocity. This simulator lets you specify the **gravitational strength (μ)**, **initial position (r₀)**, and **initial velocity (v₀)**, and then visualizes the trajectory in real time with stars, glow, and a fading trail.



## Goal: to learn more about physics and displaying it with c++.
I’ve always been fascinated by the logic and theory behind how our world works, especially gravity. By building a gravity/orbit simulator, I wanted to see how mathematical laws (Newton’s law of gravitation) could be turned into code, algorithms, and visualizations.
The project is a combination of c++/SFML(graphics), verlet algorithm(velocity) and Newtonian Laws of Gravity

## 🚀 Features
- Console prompts for initial **μ, r₀, v₀** with defaults and recommended ranges
- **Velocity–Verlet** integration scheme
- SFML rendering: stars, glowing center, fading orbit trail

## 📐 Math Derivation

This section walks from **Taylor expansion** of position through the final **Velocity–Verlet** scheme.

---

### 1. Taylor Expansion of Position

Expand $r(t + \Delta t)$ around $t$:

$$
r(t + \Delta t) = r(t) + v(t)\,\Delta t + \tfrac{1}{2} a(t)\,\Delta t^2 + O(\Delta t^3)
$$

Dropping higher-order terms gives:

$$
r_{\text{new}} = r + v\,\Delta t + \tfrac{1}{2} a\,\Delta t^2
$$

 This is how we obtain **$r_{\text{new}}$**.

---

### 2. Gravitational Acceleration

From Newton’s law:

$$
F = G \frac{M m}{r^2}
$$

Acceleration for the particle:

$$
a(r) = - \mu \frac{r}{\|r\|^3}, \quad \mu = GM
$$

 After updating $r_{\text{new}}$, we recompute:

$$
a_{\text{new}} = - \mu \frac{r_{\text{new}}}{\|r_{\text{new}}\|^3}
$$

 This gives **$a_{\text{new}}$**.

---

### 3. Velocity Update from Integral of Acceleration

Velocity evolves according to:

$$
v(t+\Delta t) = v(t) + \int_t^{t+\Delta t} a(\tau)\, d\tau
$$

Since $a(\tau)$ is not exactly known, approximate with the **trapezoid rule**:

$$
\int_t^{t+\Delta t} a(\tau)\, d\tau \;\approx\; \tfrac{1}{2} \big(a(t) + a(t+\Delta t)\big)\,\Delta t
$$

So the velocity update is:

$$
v_{\text{new}} = v + \tfrac{1}{2} (a + a_{\text{new}})\, \Delta t
$$

 This gives **$v_{\text{new}}$**.

---

### 4. Velocity–Verlet Summary

Together, the update rules are:

$$
\begin{aligned}
r_{\text{new}} &= r + v \,\Delta t + \tfrac{1}{2} a \,\Delta t^2 \\
a_{\text{new}} &= -\mu \frac{r_{\text{new}}}{\|r_{\text{new}}\|^3} \\
v_{\text{new}} &= v + \tfrac{1}{2} (a + a_{\text{new}})\,\Delta t
\end{aligned}
$$

- **$r_{\text{new}}$** comes from **Taylor expansion**.  
- **$a_{\text{new}}$** comes from the **gravitational field formula** applied at the new position.  
- **$v_{\text{new}}$** comes from integrating acceleration with the **trapezoid rule**.

---

## 🧮 Conserved Quantities

To check correctness:

- **Specific Energy**

$$
E = \tfrac{1}{2} \|v\|^2 - \frac{\mu}{\|r\|}
$$

- **Angular Momentum**

$$
L = x v_y - y v_x
$$

For circular orbit (μ=1, r=1, v=1):

$$
E = -0.5, \quad L = 1
$$

---

## 🛠️ Code Structure
###  `main.cpp`

**Role**:

* User input (μ, r₀, v₀)
* SFML window setup and rendering
* Time loop: calls `verlet()` every frame
* Displays visual orbit, trail, HUD

**Physics Features Implemented**:

* Calls to:

  * `accel(r, mu)` → gravitational acceleration
  * `verlet(r, v, a, dt, mu)` → motion update
* HUD output of:

  * \$E = \tfrac{1}{2} |v|^2 - \mu/|r|\$
  * \$L = x v\_y - y v\_x\$
* Converts physics coordinates → screen pixels

**Math formulas used**:

```cpp
double rmag = norm(r);                  // ‖r‖
double vsq  = dot(v, v);                // v⋅v = |v|²
double E    = 0.5 * vsq - mu / rmag;   // specific energy
double L    = r.x * v.y - r.y * v.x;   // angular momentum
```

---

###  `math.h` and `math_cpp.cpp`

**Role**:

* Defines a 2D vector struct (`Vec2`)
* Implements basic vector operations

**Functions Implemented**:

* `add(Vec2 a, Vec2 b)` → \$a + b\$
* `sub(Vec2 a, Vec2 b)` → \$a - b\$
* `scale(Vec2 a, double k)` → \$k \cdot a\$
* `dot(Vec2 a, Vec2 b)` → \$a \cdot b = ax \cdot bx + ay \cdot by\$
* `norm(Vec2 a)` → \$|a| = \sqrt{a \cdot a}\$

**Math formulas**:

```cpp
// norm = ||a|| = sqrt(ax² + ay²)
double norm(Vec2 a) {
    return std::sqrt(dot(a, a));
}
```

---

###  `physics.h` and `physics_cpp.cpp`

**Role**:

* Contains core physics equations
* Handles gravitational acceleration
* Runs the velocity–Verlet algorithm

---

#### `accel(Vec2 r, double mu)`

**Implements**:

$$
a(r) = -\mu \frac{r}{\|r\|^3}
$$

**Purpose**:
Calculates gravitational acceleration based on distance from the origin.

```cpp
double factor = -mu / (rmag * rmag * rmag);
return scale(r, factor);
```

---

#### `verlet(Vec2& r, Vec2& v, Vec2& a, double dt, double mu)`

**Implements**:

$$
\begin{aligned}
r_{\text{new}} &= r + v \Delta t + \tfrac{1}{2} a \Delta t^2 \\
a_{\text{new}} &= -\mu \frac{r_{\text{new}}}{\|r_{\text{new}}\|^3} \\
v_{\text{new}} &= v + \tfrac{1}{2}(a + a_{\text{new}}) \Delta t
\end{aligned}
$$

**Purpose**:
Performs one update of the particle’s motion using a second-order accurate method derived from Taylor expansion + trapezoid rule.

```cpp
Vec2 r_new = add(r, add(scale(v, dt), scale(a, 0.5 * dt * dt)));
Vec2 a_new = accel(r_new, mu);
Vec2 v_new = add(v, scale(add(a, a_new), 0.5 * dt));
```











