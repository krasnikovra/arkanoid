#define _USE_MATH_DEFINES

#include "Racket.h"
#include "Game.h"
#include <cmath>

constexpr float ANGLE_MAX = 1.3f; // [-ANGLE_MAX, ANGLE_MAX] from normal

Racket::Racket(Game& parent, const float y, const float speed, const sf::Vector2f& size, const sf::Color& color):
    _parent(parent), _speedDefault(speed), _speed(0) {
    _shape.setFillColor(color);
    _shape.setSize(size);
    _shape.setPosition(sf::Vector2f(0, y));
}

void Racket::handleEvent(const sf::Event& event) {
    switch (event.type) {
    case sf::Event::KeyPressed:
        switch (event.key.code) {
        case sf::Keyboard::Left:
            if (_speed == 0)
                _speed = -_speedDefault;
            break;
        case sf::Keyboard::Right:
            if (_speed == 0)
                _speed = _speedDefault;
            break;
        }
        break;
    case sf::Event::KeyReleased:
        switch (event.key.code) {
        case sf::Keyboard::Left:
            if (_speed < 0)
                _speed = 0;
            break;
        case sf::Keyboard::Right:
            if (_speed > 0)
                _speed = 0;
            break;
        }
        break;
    }
}

sf::Vector2f Racket::getPosition() const {
    return _shape.getPosition();
}

sf::Vector2f Racket::getSize() const {
    return _shape.getSize();
}

std::function<void(void)> Racket::collideWith(DefenderBall& ball) const {
    const sf::Vector2f pos = ball.getPosition();
    const sf::Vector2f racketPos = _shape.getPosition();
    return [=, &ball](void) -> void {
        _collideWithBall(pos, racketPos, ball);
    };
}

std::function<void(void)> Racket::collideWith(DefenderBlock& block) const {
    return [](void) -> void { return; };
}

std::function<void(void)> Racket::collideWith(DefenderBonus& bonus) const {
    sf::FloatRect self = sf::FloatRect(getPosition(), getSize());
    sf::FloatRect bonusRect = sf::FloatRect(bonus.getPosition(), bonus.getSize());
    if (!self.intersects(bonusRect) || bonus.isDead())
        return [](void) -> void { return; };
    return [&bonus](void) -> void { bonus.invokeBonusAction(); };
}

bool Racket::_collideWithBall(const sf::Vector2f& ballBackupPos, const sf::Vector2f& selfBackupPos, DefenderBall& ball) const {
    sf::Vector2f pos = ballBackupPos;
    sf::Vector2f racketPos = selfBackupPos;
    sf::Vector2f racketSize = _shape.getSize();
    sf::Vector2f velocity = ball.getVelocity();
    float radius = ball.getRadius();
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
        float velocityNorm = sqrt(DistSqr(velocity, sf::Vector2f(0, 0)));
        float angle = (pos.x + radius - (racketPos.x + racketSize.x / 2.0f)) / (racketSize.x / 2.0f) * ANGLE_MAX;
        sf::Vector2f newVelocity = {
            velocityNorm * sin(angle),
            -velocityNorm * cos(angle)
        };
        ball.setPosition(sf::Vector2f(pos.x, top.top - radius));
        ball.setVelocity(newVelocity);
    }
    if (bot.contains(midPoint)) {
        ball.setPosition(sf::Vector2f(pos.x, bot.top));
        if (velocity.y < 0)
            ball.setVelocity(sf::Vector2f(velocity.x, -velocity.y));
        return true;
    }
    if (left.contains(midPoint)) {
        ball.setPosition(sf::Vector2f(left.left - radius, pos.y));
        if (velocity.x > 0)
            ball.setVelocity(sf::Vector2f(-velocity.x, velocity.y));
        return true;
    }
    if (right.contains(midPoint)) {
        ball.setPosition(sf::Vector2f(right.left, pos.y));
        if (velocity.x < 0)
            ball.setVelocity(sf::Vector2f(-velocity.x, velocity.y));
        return true;
    }
    // corners
    sf::Vector2f nw = racketPos;
    sf::Vector2f ne = racketPos + sf::Vector2f(racketSize.x, 0);
    sf::Vector2f sw = racketPos + sf::Vector2f(0, racketSize.y);
    sf::Vector2f se = racketPos + racketSize;
    // warning: magic math
    auto BallIsCloseTo = [&midPoint, &radius](const sf::Vector2f& point) -> bool {
        return DistSqr(midPoint, point) < radius * radius;
    };
    sf::Vector2f corner = sf::Vector2f(-1, -1);
    for (auto& corn : { nw, ne, sw, se })
        if (BallIsCloseTo(corn))
            corner = corn;
    if (corner == sf::Vector2f(-1, -1))
        return false;
    sf::Vector2f rad = corner - midPoint;
    bool isVelAnticlockwiseRad = (velocity.x * rad.y - velocity.y * rad.x) < 0;
    float velRadDotProd = velocity.x * rad.x + velocity.y * rad.y;
    // here if racket catches a ball right on a corner while moving
    // we change y velocity so ball just run away and push it
    // basically, like top/bot cases but push a bit less
    if (velRadDotProd < 0) { // racket just catched the ball ongoing
        // racket "push" the ball horizontally, also some magic
        float push = sqrt(radius * radius - rad.y * rad.y) - abs(rad.x);
        ball.setPosition(pos + sf::Vector2f(velocity.x > 0 ? push : -push, 0));
        // also could need to change y velocity
        if (rad.y * velocity.y > 0)
            ball.setVelocity(sf::Vector2f(velocity.x, -velocity.y));
        return true;
    }
    float velNorm = sqrt(DistSqr(velocity, sf::Vector2f(0, 0)));
    float radNorm = sqrt(DistSqr(rad, sf::Vector2f(0, 0)));
    float cosVelRadAngle = velRadDotProd / (velNorm * radNorm);
    float angle = static_cast<float>(M_PI) + 2 * acos(cosVelRadAngle);
    if (!isVelAnticlockwiseRad)
        angle = -angle;
    sf::Vector2f newVelocity = sf::Vector2f(
        velocity.x * cos(angle) + velocity.y * sin(angle),
        -velocity.x * sin(angle) + velocity.y * cos(angle)
    );
    ball.setVelocity(newVelocity);
    return true;
}

void Racket::_move() {
    float timeDeltaMs = _parent.getTimeMsSinceLastFrame();
    sf::Vector2f velocity = sf::Vector2f(_speed, 0);
    _shape.move(timeDeltaMs * velocity);
}

void Racket::_update() {
    _move();
    _handleWindowCollision();
}

void Racket::_handleWindowCollision() {
    sf::Vector2f pos = _shape.getPosition();
    float width = _shape.getSize().x;
    unsigned winWidth = _parent.getWindow()->getSize().x;
    if (pos.x < 0)
        _shape.setPosition(sf::Vector2f(0, pos.y));
    if (pos.x + width > winWidth)
        _shape.setPosition(sf::Vector2f(winWidth - width, pos.y));
}

void Racket::draw() {
    _update();
    _parent.getWindow()->draw(_shape);
}
