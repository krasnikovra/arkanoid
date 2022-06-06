#define _USE_MATH_DEFINES

#include "Ball.h"
#include "Game.h"
#include <cmath>

constexpr float ANGLE_MAX = 1.3f; // [-ANGLE_MAX, ANGLE_MAX] from normal

Ball::Ball(Game& parent, const sf::Vector2f& velocity, const float radius, const sf::Color& color): 
    _parent(parent), _velocity(velocity),
    _shape(std::make_unique<sf::CircleShape>()) {
    _shape->setRadius(radius);
    _shape->setFillColor(color);
}

void Ball::_move() {
    float timeDeltaMs = _parent.getTimeMsSinceLastFrame();
    _shape->move(timeDeltaMs * _velocity);
}

void Ball::_update() {
    // after all move the ball
    _move();
    // dummy collisions implementation
    _handleWindowCollision();
    //_handleRacketCollision(); // this should be replaced with more intelligent collision system
}

void Ball::_handleWindowCollision() {
    std::unique_ptr<sf::RenderWindow>& window = _parent.getWindow();
    sf::Vector2u winSize = window->getSize();
    sf::Vector2f pos = _shape->getPosition();
    float radius = _shape->getRadius();
    bool inWindowXborder = pos.x >= 0 && pos.x + 2 * radius <= winSize.x;
    if (!inWindowXborder) {
        // return to valid position to prevent stuck
        _shape->setPosition(sf::Vector2f(pos.x < 0 ? 0 : winSize.x - 2 * radius, pos.y));
        _velocity.x = -_velocity.x;
    }
    // and the same vertically
    bool inWindowYborder = pos.y >= 0 && pos.y + 2 * radius <= winSize.y;
    if (!inWindowYborder) {
        // return to valid position to prevent stuck
        _shape->setPosition(sf::Vector2f(pos.x, pos.y < 0 ? 0 : winSize.y - 2 * radius));
        _velocity.y = -_velocity.y;
    }
}

void Ball::draw() {
    _update();
    _parent.getWindow()->draw(*_shape);
}

std::function<void(void)> Ball::hitBy(const AttackerBall& ball) {
    if (&ball != this) 
        return [](void) -> void { return; }; // TODO
    return [](void) -> void { return; };
}

std::function<void(void)> Ball::hitBy(const AttackerBox& box) {
    return [](void) -> void { return; }; // TODO
}

std::function<void(void)> Ball::hitBy(const AttackerRacket& racket) {
    auto DistSqr = [](const sf::Vector2f& a, const sf::Vector2f& b) -> float {
        return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
    };
    float racketSpeed = racket.getSpeed();
    float racketShift = _parent.getTimeMsSinceLastFrame() * racketSpeed;
    sf::Vector2f racketPos = racket.getPosition();
    sf::Vector2f racketSize = racket.getSize();
    sf::Vector2f pos = _shape->getPosition();
    float radius = _shape->getRadius();
    sf::Vector2f midPoint = pos + sf::Vector2f(radius, radius);
    // zones of collision, not intersecting
    sf::FloatRect top = sf::FloatRect(
        racketPos - sf::Vector2f(0, radius),
        sf::Vector2f(racketSize.x, radius)
    );
    sf::FloatRect bot = sf::FloatRect(
        racketPos + sf::Vector2f(0, racketSize.y),
        sf::Vector2f(racketSize.x, radius)
    );
    sf::FloatRect left = sf::FloatRect(
        racketPos - sf::Vector2f(radius, 0),
        sf::Vector2f(radius, racketSize.y)
    );
    sf::FloatRect right = sf::FloatRect(
        racketPos + sf::Vector2f(racketSize.x, 0),
        sf::Vector2f(radius, racketSize.y)
    );
    // zones dont intersect so these ifs are independent
    if (top.contains(midPoint)) {
        float velocityNorm = sqrt(DistSqr(_velocity, sf::Vector2f(0, 0)));
        float angle = (pos.x - (racketPos.x + racketSize.x / 2.0f)) / (racketSize.x / 2.0f) * ANGLE_MAX;
        sf::Vector2f newVelocity = {
            velocityNorm * sin(angle),
            -velocityNorm * cos(angle)
        };
        return [=](void) -> void {
            _shape->setPosition(sf::Vector2f(pos.x, top.top - radius));
            _velocity = newVelocity;
        };
    }
    if (bot.contains(midPoint)) {
        return [=](void) -> void {
            _shape->setPosition(sf::Vector2f(pos.x, bot.top));
            if (_velocity.y < 0)
                _velocity.y = -_velocity.y;
        };
    }
    if (left.contains(midPoint)) {
        return [=](void) -> void {
            _shape->setPosition(sf::Vector2f(left.left - radius, pos.y));
            if (_velocity.x > 0)
                _velocity.x = -_velocity.x;
        };
    }
    if (right.contains(midPoint)) {
        return [=](void) -> void {
            _shape->setPosition(sf::Vector2f(right.left, pos.y));
            if (_velocity.x < 0)
                _velocity.x = -_velocity.x;
        };
    }
    // corners
    sf::Vector2f nw = racketPos;
    sf::Vector2f ne = racketPos + sf::Vector2f(racketSize.x, 0);
    sf::Vector2f sw = racketPos + sf::Vector2f(0, racketSize.y);
    sf::Vector2f se = racketPos + racketSize;
    // warning: magic math
    auto BallIsCloseTo = [&DistSqr, &midPoint, &radius](const sf::Vector2f& point) -> bool {
        return DistSqr(midPoint, point) < radius * radius;
    };
    sf::Vector2f corner = sf::Vector2f(-1, -1);
    for (auto& corn : { nw, ne, sw, se })
        if (BallIsCloseTo(corn))
            corner = corn;
    if (corner == sf::Vector2f(-1, -1))
        return [](void) -> void { return; };
    sf::Vector2f rad = corner - midPoint;
    bool isVelAnticlockwiseRad = (_velocity.x * rad.y - _velocity.y * rad.x) < 0;
    float velRadDotProd = _velocity.x * rad.x + _velocity.y * rad.y;
    // here if racket catches a ball right on a corner while moving
    // we change y velocity so ball just run away and push it
    // basically, like top/bot cases but push a bit less
    if (velRadDotProd < 0) { // racket just catched the ball ongoing
        // racket "push" the ball horizontally, also some magic
        float push = sqrt(radius * radius - rad.y * rad.y) - abs(rad.x);
        return [=](void) -> void {
            _shape->move(sf::Vector2f(_velocity.x > 0 ? push : -push, 0));
            // also could need to change y velocity
            if (rad.y * _velocity.y > 0)
                _velocity.y = -_velocity.y;
        };
    }
    float velNorm = sqrt(DistSqr(_velocity, sf::Vector2f(0, 0)));
    float radNorm = sqrt(DistSqr(rad, sf::Vector2f(0, 0)));
    float cosVelRadAngle = velRadDotProd / (velNorm * radNorm);
    float angle = static_cast<float>(M_PI) + 2 * acos(cosVelRadAngle);
    if (!isVelAnticlockwiseRad)
        angle = -angle;
    sf::Vector2f newVelocity = sf::Vector2f(
        _velocity.x * cos(angle) + _velocity.y * sin(angle),
        -_velocity.x * sin(angle) + _velocity.y * cos(angle)
    );
    return [=](void) -> void {
        _velocity = newVelocity;
    };
}

