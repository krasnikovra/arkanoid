#include "Ball.h"
#include "Game.h"
#include "Utils.h"

Ball::Ball(Game& parent, const sf::Vector2f& velocity, const float radius, const sf::Color& color): 
    _parent(parent), _velocity(velocity), _boosted(false), _velocityNormDefault(sqrt(DistSqr(velocity, sf::Vector2f(0,0)))),
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
    if (_boosted)
        return;
    _velocity *= multiplier;
    _boosted = true;
    _lastBoostTimePoint = std::chrono::system_clock::now();
    _lastBoostDuration = timeMs;
    //debugging
    _shape.setFillColor(sf::Color::Blue);
}

std::function<void(void)> Ball::collideWith(DefenderBall& ball) const {
    if (&ball == this)
        return [](void) -> void { return; };
    return [](void) -> void { return; };
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
