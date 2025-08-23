#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <deque>
#include <random>
#include <sstream>
#include "math.h"
#include "physics.h"

// Simple input helpers
void read_or_keep(const std::string& prompt, double& value, const std::string& hint) {
    std::cout << prompt << " " << hint;
    std::string line; std::getline(std::cin, line);
    std::istringstream iss(line); double tmp;
    if (!line.empty() && (iss >> tmp)) value = tmp;
}

void read_pair_or_keep(const std::string& prompt, double& x, double& y, const std::string& hint) {
    std::cout << prompt << " " << hint;
    std::string line; std::getline(std::cin, line);
    std::istringstream iss(line); double tx, ty;
    if (!line.empty() && (iss >> tx >> ty)) { x = tx; y = ty; }
}


static sf::Vector2f toPixels(Vec2 p, int W, int H, double box) {
    float u = float((p.x / box + 1.0) * 0.5);         // 0..1
    float v = float((p.y / box + 1.0) * 0.5);
    return { u * (W - 1), (1.0f - v) * (H - 1) };
}

//starfield
static sf::VertexArray makeStars(unsigned W, unsigned H, int count, unsigned seed=1337) {
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> X(0.f, float(W-1));
    std::uniform_real_distribution<float> Y(0.f, float(H-1));
    std::uniform_int_distribution<int> A(80, 200); // alpha
    sf::VertexArray stars(sf::Points, count);
    for (int i = 0; i < count; ++i) {
        stars[i].position = { X(rng), Y(rng) };
        int a = A(rng);
        stars[i].color = sf::Color(255, 255, 255, (sf::Uint8)a);
    }
    return stars;
}



int main() {
    double mu = 1.0;
    double r0x = 1.0, r0y = 0.0;
    double v0x = 0.0, v0y = 1.0;

    read_or_keep("mu (gravity strength)", mu, "[default 1.0, range: 0.001..10]: ");
    read_pair_or_keep("r0.x r0.y (initial position)", r0x, r0y, "[default 1 0, range: 0.2..1.5]: ");
    read_pair_or_keep("v0.x v0.y (initial velocity)", v0x, v0y, "[default 0 1, range: -3..3]: ");

    if (std::sqrt(r0x*r0x + r0y*r0y) < 1e-6) {
        r0x = 1.0; r0y = 0.0;
        std::cout << "Note: |r0| too small; using default (1,0)." << std::endl;
    }

    std::cout << "Using mu=" << mu << ", r0=(" << r0x << ", " << r0y << "), v0=(" << v0x << ", " << v0y << ")" << std::endl;

    sf::ContextSettings cs; cs.antialiasingLevel = 8;
    sf::RenderWindow win(sf::VideoMode(900, 700), "Gravity Simulator", sf::Style::Default, cs);
    win.setFramerateLimit(60);

    //Physics stuff
    const double dt  = 1e-3;
    double box       = 2.0;
    Vec2 r{r0x, r0y}, v{v0x, v0y}, a = accel(r, mu);

    //Visuals
    auto size  = win.getSize();
    int W = int(size.x), H = int(size.y);
    sf::VertexArray stars = makeStars(W, H, std::max(300, (W*H)/5000)); // density scales with window

    // Center + soft glow
    sf::CircleShape centerCore(6.f);  centerCore.setOrigin(6,6);
    centerCore.setFillColor(sf::Color(255, 230, 120));
    sf::CircleShape centerGlow(36.f); centerGlow.setOrigin(36,36);
    centerGlow.setFillColor(sf::Color(255, 220, 80, 35)); // transparent glow

    // Orbiter + soft glow
    sf::CircleShape body(4.f);   body.setOrigin(4,4);
    body.setFillColor(sf::Color(120, 220, 255));
    sf::CircleShape bodyGlow(14.f); bodyGlow.setOrigin(14,14);
    bodyGlow.setFillColor(sf::Color(120, 220, 255, 40));


    const std::size_t TRAIL_MAX = 800;
    std::deque<sf::Vector2f> trail;

    bool paused = false;
    sf::Clock frameClock;
    double t = 0.0;

    while (win.isOpen()) {
        sf::Event e;
        while (win.pollEvent(e)) {
            if (e.type == sf::Event::Closed) win.close();
            if (e.type == sf::Event::KeyPressed) {
                if (e.key.code == sf::Keyboard::Space) paused = !paused;
                if (e.key.code == sf::Keyboard::R) {   // reset to user's initial values
                    r = {r0x, r0y}; v = {v0x, v0y}; a = accel(r, mu); t = 0.0; trail.clear();
                }
                if (e.key.code == sf::Keyboard::Up)   box *= 0.9;  // zoom in
                if (e.key.code == sf::Keyboard::Down) box *= 1.1;  // zoom out
            }
            if (e.type == sf::Event::Resized) {
                // keep pixel-space starfield covering the whole window
                W = int(e.size.width); H = int(e.size.height);
                stars = makeStars(W, H, std::max(300, (W*H)/5000));
                // reset view to new size (prevents stretch)
                win.setView(sf::View(sf::FloatRect(0,0,float(W),float(H))));
            }
        }

        if (!paused) {
            for (int i = 0; i < 16; ++i) { // small steps per frame
                verlet(r, v, a, dt, mu);
                t += dt;
            }
        }

        double rmag = norm(r);
        double vsq  = dot(v, v);
        double E    = 0.5 * vsq - mu / rmag;
        double L    = r.x * v.y - r.y * v.x;
        std::ostringstream hud; hud.setf(std::ios::fixed); hud.precision(3);
        hud << (paused ? "[PAUSED]  " : "") << "t=" << t
            << "  r=" << rmag << "  v=" << std::sqrt(vsq)
            << "  E=" << E << "  L=" << L << "  mu=" << mu;
        win.setTitle(hud.str());

        //draw positions
        sf::Vector2f pCenter = toPixels({0,0}, W, H, box);
        sf::Vector2f pBody = toPixels(r,     W, H, box);


        if (!paused) {
            trail.push_front(pBody);
            if (trail.size() > TRAIL_MAX) trail.pop_back();
        }

        // trail build
        sf::VertexArray trailStrip(sf::LineStrip, trail.size());
        for (std::size_t i = 0; i < trail.size(); ++i) {
            float a = 255.f * (1.f - float(i) / float(std::max<std::size_t>(1, trail.size()-1)));
            trailStrip[i].position = trail[i];
            trailStrip[i].color    = sf::Color(120, 220, 255, (sf::Uint8)a);
        }

        // ---- Draw ----
        win.clear(sf::Color(5, 7, 15));        // deep space blue-black
        win.draw(stars);                        // background stars

        centerGlow.setPosition(pCenter);  win.draw(centerGlow);
        centerCore.setPosition(pCenter);  win.draw(centerCore);

        win.draw(trailStrip);                   // fading trail

        bodyGlow.setPosition(pBody); win.draw(bodyGlow);
        body.setPosition(pBody);     win.draw(body);

        win.display();
    }
    return 0;
}





