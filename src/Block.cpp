#define _USE_MATH_DEFINES

#include "Block.h"
#include "Game.h"
#include <cmath>
#include <iostream> //debugging

constexpr float OUTLINE_THICKNESS_RATIO = 0.05f;

Block::Block(Game& parent, const sf::Vector2f& pos, const sf::Vector2f& size,
             const sf::Color& color):
    _parent(parent) {
    _shape.setPosition(pos);
    _shape.setSize(size);
    _shape.setFillColor(color);
    _shape.setOutlineColor(sf::Color::Black);
    const float outlineThickness = OUTLINE_THICKNESS_RATIO * (size.x > size.y ? size.y : size.x);
    _shape.setOutlineThickness(-round(outlineThickness)); // rounding bcoz non-int outline looks awful
}

void Block::draw() {
    _parent.getWindow()->draw(_shape);
}

bool Block::isDead() const {
    return false;
}

void Block::damage() {}

sf::Vector2f Block::getPosition() const {
    return _shape.getPosition();
}

sf::Vector2f Block::getSize() const {
    return _shape.getSize();
}

std::function<void(void)> Block::collideWith(DefenderBall& ball) const {
    const sf::Vector2f pos = ball.getPosition();
    const sf::Vector2f racketPos = _shape.getPosition();
    return [=, &ball](void) -> void {
        _collideWithBall(pos, racketPos, ball);
    };
}

std::function<void(void)> Block::collideWith(DefenderBlock& block) const {
    if (&block == this)
        return [](void) -> void { return; };
    return [](void) -> void { return; };
}

bool Block::_collideWithBall(const sf::Vector2f& ballBackupPos, const sf::Vector2f& selfBackupPos, DefenderBall& ball) const {
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
        ball.setPosition(sf::Vector2f(pos.x, top.top - radius));
        if (velocity.y > 0)
            ball.setVelocity(sf::Vector2f(velocity.x, -velocity.y));
        return true;
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

SoftBlock::SoftBlock(Game& parent, const sf::Vector2f& pos, const sf::Vector2f& size,
                     const int health, const sf::Color& color): 
    Block(parent, pos, size, color), _health(health), _healthDefault(health) {}

bool SoftBlock::isDead() const {
    return _health < 1;
}

void SoftBlock::damage() {
    if (_health < 1)
        return;
    _health--;
    const sf::Color fillColor = _shape.getFillColor();
    const sf::Color outlineColor = _shape.getOutlineColor();
    const sf::Uint8 newAlpha = static_cast<sf::Uint8>(255 * (_health / static_cast<float>(_healthDefault)));
    _shape.setFillColor(sf::Color(fillColor.r, fillColor.g, fillColor.b, newAlpha));
    _shape.setOutlineColor(sf::Color(outlineColor.r, outlineColor.g, outlineColor.b, newAlpha));
}

std::function<void(void)> SoftBlock::collideWith(DefenderBall& ball) const {
    if (_health < 1)
        return [](void) -> void { return; };
    return Block::collideWith(ball);
}

DeathActionBlock::DeathActionBlock(Game& parent, const sf::Vector2f& pos, const sf::Vector2f& size,
                                   const int health, const sf::Color& color):
    SoftBlock(parent, pos, size, health, color) {}

std::function<void(void)> DeathActionBlock::collideWith(DefenderBall& ball) const {
    const unsigned health = _health;
    const sf::Vector2f pos = ball.getPosition();
    const sf::Vector2f racketPos = _shape.getPosition();
    return [=, &ball](void) -> void {
        if (_collideWithBall(pos, racketPos, ball) && health == 1)
            _deathAction(ball);
    };
}

BoostBlock::BoostBlock(Game& parent, const sf::Vector2f& pos, const sf::Vector2f& size,
                       const int health, const float multiplier, const float durationMs,
                       const sf::Color& color):
    DeathActionBlock(parent, pos, size, health, color), _multiplier(multiplier), _durationMs(durationMs) {}

void BoostBlock::_deathAction(DefenderBall& ball) const {
    ball.boostUp(_multiplier, _durationMs);
}
