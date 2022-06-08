#pragma once
#include <SFML/Graphics.hpp>
#include "Attacker.h"
#include "Defender.h"
#include <chrono>

class Game;

class Ball : public DefenderBall, public Attacker {
public:
    explicit Ball() = delete;
    explicit Ball(const Ball& other) = default;
    explicit Ball(Ball&& other) noexcept = default;
    explicit Ball(Game& parent, const sf::Vector2f& velocity, const float radius, const sf::Color& color);
    virtual ~Ball() = default;
    void draw();
    // DefenderBall's methods
    virtual sf::Vector2f getPosition() const override;
    virtual float getRadius() const override;
    virtual sf::Vector2f getVelocity() const override;
    virtual void setPosition(const sf::Vector2f& pos) override;
    virtual void setVelocity(const sf::Vector2f& velocity) override;
    virtual void boostUp(const float multiplier, const float timeMs) override;
    // Attacker's methods
    virtual std::function<void(void)> collideWith(DefenderBall& ball) const override;
    virtual std::function<void(void)> collideWith(DefenderBlock& block) const override;
private:
    void _move();
    void _update();
    void _handleWindowCollision();
    void _handleBoost();

    Game& _parent;
    sf::CircleShape _shape;
    sf::Vector2f _velocity;
    std::chrono::time_point<std::chrono::system_clock> _lastBoostTimePoint;
    float _lastBoostDuration;
    float _velocityNormDefault;
    sf::Color _colorDefault;
    bool _boosted;
};