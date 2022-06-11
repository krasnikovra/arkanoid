#define _USE_MATH_DEFINES

#include "Ball.h"
#include "Game.h"
#include "Utils.h"
#include <cmath>
#include <iostream>

Ball::Ball(Game& parent, const sf::Vector2f& velocity, const float radius,
           const int fallScoreIncrease, const sf::Color& color):
    _parent(parent), _velocity(velocity), _boosted(false), _fallScoreIncrease(fallScoreIncrease),
    _velocityNormDefault(sqrt(DistSqr(velocity, sf::Vector2f(0,0)))),
    _lastBoostDuration(FLT_MAX), _colorDefault(color) {
    _shape.setRadius(radius);
    _shape.setFillColor(color);
}

void Ball::_move() {
    float timeDeltaMs = _parent.getTimeMsSinceLastFrame();
    _shape.move(timeDeltaMs * _velocity);
}

void Ball::_update() {
    // after all move the ball
    _move();
    // dummy collisions implementation
    _handleWindowCollision();
    _handleBoost();
}

void Ball::_handleWindowCollision() {
    std::unique_ptr<sf::RenderWindow>& window = _parent.getWindow();
    sf::Vector2u winSize = window->getSize();
    sf::Vector2f pos = _shape.getPosition();
    float radius = _shape.getRadius();
    bool inWindowXborder = pos.x >= 0 && pos.x + 2 * radius <= winSize.x;
    if (!inWindowXborder) {
        // return to valid position to prevent stuck
        _shape.setPosition(sf::Vector2f(pos.x < 0 ? 0 : winSize.x - 2 * radius, pos.y));
        _velocity.x = -_velocity.x;
    }
    // and the same vertically
    bool inWindowYborder = pos.y >= 0 && pos.y + 2 * radius <= winSize.y;
    if (!inWindowYborder) {
        // return to valid position to prevent stuck
        _shape.setPosition(sf::Vector2f(pos.x, pos.y < 0 ? 0 : winSize.y - 2 * radius));
        _velocity.y = -_velocity.y;
        if (pos.y + 2 * radius > winSize.y)
            _parent.increaseScore(_fallScoreIncrease);
    }
}

void Ball::_handleBoost() {
    if (!_boosted)
        return;
    auto now = std::chrono::system_clock::now();
    float timeDeltaMs = std::chrono::duration<float, std::milli>(now - _lastBoostTimePoint).count();
    if (timeDeltaMs > _lastBoostDuration) {
        _velocity *= _velocityNormDefault / sqrt(DistSqr(_velocity, sf::Vector2f(0,0)));
        _boosted = false;
        //debugging
        _shape.setFillColor(_colorDefault);
    }
}

bool Ball::isDead() const {
    return false;
}

void Ball::draw() {
    _update();
    _parent.getWindow()->draw(_shape);
}

sf::Vector2f Ball::getPosition() const {
    return _shape.getPosition();
}

float Ball::getRadius() const {
    return _shape.getRadius();
}

sf::Vector2f Ball::getVelocity() const {
    return _velocity;
}

void Ball::setPosition(const sf::Vector2f& pos) {
    _shape.setPosition(pos);
}

void Ball::setVelocity(const sf::Vector2f& velocity) {
    _velocity = velocity;
}

void Ball::boostUp(const float multiplier, const float timeMs) {
    _lastBoostTimePoint = std::chrono::system_clock::now();
    _lastBoostDuration = timeMs;
    if (_boosted) 
        return;
    _velocity *= multiplier;
    _boosted = true;
    _shape.setFillColor(sf::Color::Blue);
}

std::function<void(void)> Ball::collideWith(DefenderBall& ball) const {
    if (&ball == this)
        return [](void) -> void { return; };
    sf::Vector2f myPos = getPosition();
    sf::Vector2f otherPos = ball.getPosition();
    return [=, &ball](void) -> void {
        sf::Vector2f velocity = ball.getVelocity();
        float myRadius = getRadius();
        sf::Vector2f myMidPoint = myPos + sf::Vector2f(myRadius, myRadius);
        float otherRadius = ball.getRadius();
        sf::Vector2f otherMidPoint = otherPos + sf::Vector2f(otherRadius, otherRadius);
        float distance = sqrt(DistSqr(myMidPoint, otherMidPoint));
        if (distance >= myRadius + otherRadius)
            return;
        sf::Vector2f axis = otherMidPoint - myMidPoint;
        if (Dot(velocity, axis) < 0) {
            sf::Vector2f velProjAxis = Dot(velocity, axis) / Dot(axis, axis) * axis;
            sf::Vector2f velProjConj = velocity - velProjAxis;
            sf::Vector2f newVelocity = velProjConj - velProjAxis;
            ball.setVelocity(newVelocity);
        }
        float push = (distance - myRadius - otherRadius) / 2.0f;
        sf::Vector2f shift = -axis / sqrt(Dot(axis, axis)) * push;
        ball.setPosition(otherPos + shift);
    };
}

std::function<void(void)> Ball::collideWith(DefenderBlock& block) const {
    float ballRadius = _shape.getRadius();
    sf::Vector2f ballMidPoint = getMidPoint();
    sf::Vector2f size = block.getSize();
    sf::Vector2f pos = block.getPosition();
    // zones of collision, not intersecting
    sf::FloatRect top = sf::FloatRect(
        pos - sf::Vector2f(0, ballRadius),
        sf::Vector2f(size.x, ballRadius)
    );
    sf::FloatRect bot = sf::FloatRect(
        pos + sf::Vector2f(0, size.y),
        sf::Vector2f(size.x, ballRadius)
    );
    sf::FloatRect left = sf::FloatRect(
        pos - sf::Vector2f(ballRadius, 0),
        sf::Vector2f(ballRadius, size.y)
    );
    sf::FloatRect right = sf::FloatRect(
        pos + sf::Vector2f(size.x, 0),
        sf::Vector2f(ballRadius, size.y)
    );
    // corners
    sf::Vector2f nw = pos;
    sf::Vector2f ne = pos + sf::Vector2f(size.x, 0);
    sf::Vector2f sw = pos + sf::Vector2f(0, size.y);
    sf::Vector2f se = pos + size;
    bool ballIsInBlock = false;
    for (auto& rect : { top, bot, left, right })
        ballIsInBlock = ballIsInBlock || rect.contains(ballMidPoint);
    for (auto& corn : { nw, ne, sw, se })
        ballIsInBlock = ballIsInBlock || isCloseTo(corn);
    if (!ballIsInBlock)
        return [](void) -> void { return; };
    return [=, &block](void) -> void { block.damage(); };
}

std::function<void(void)> Ball::collideWith(DefenderBonus& bonus) const {
    return [](void) -> void { return; };
}

ExtraBall::ExtraBall(Game& parent, const sf::Vector2f& pos, const sf::Vector2f& velocity, const float radius,
                     const int fallScoreIncrease, const sf::Color& color) :
    Ball(parent, velocity, radius, fallScoreIncrease, color), _isDead(false) {
    _shape.setPosition(pos);
}

bool ExtraBall::isDead() const {
    return _isDead;
}

void ExtraBall::_handleWindowCollision() {
    std::unique_ptr<sf::RenderWindow>& window = _parent.getWindow();
    sf::Vector2u winSize = window->getSize();
    sf::Vector2f pos = _shape.getPosition();
    float radius = _shape.getRadius();
    bool inWindowXborder = pos.x >= 0 && pos.x + 2 * radius <= winSize.x;
    if (!inWindowXborder) {
        // return to valid position to prevent stuck
        _shape.setPosition(sf::Vector2f(pos.x < 0 ? 0 : winSize.x - 2 * radius, pos.y));
        _velocity.x = -_velocity.x;
    }
    // and the same vertically
    bool inWindowYborder = pos.y >= 0 && pos.y + 2 * radius <= winSize.y;
    if (!inWindowYborder) {
        _isDead = pos.y + 2 * radius > winSize.y;
        // return to valid position to prevent stuck
        _shape.setPosition(sf::Vector2f(pos.x, pos.y < 0 ? 0 : winSize.y - 2 * radius));
        _velocity.y = -_velocity.y;
    }
}
