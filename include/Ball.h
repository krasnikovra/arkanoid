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
    explicit Ball(Game& parent, const sf::Vector2f& velocity, const float radius, 
                  const int fallScoreIncrease = -15, const sf::Color& color = sf::Color::Red);
    virtual ~Ball() = default;
    virtual bool isDead() const;
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
    virtual std::function<void(void)> collideWith(DefenderBonus& bonus) const override;
protected:
    void _move();
    void _update();
    virtual void _handleWindowCollision();
    void _handleBoost();

    Game& _parent;
    sf::CircleShape _shape;
    sf::Vector2f _velocity;
    int _fallScoreIncrease;
    std::chrono::time_point<std::chrono::system_clock> _lastBoostTimePoint;
    float _lastBoostDuration;
    float _velocityNormDefault;
    sf::Color _colorDefault;
    bool _boosted;
};

class ExtraBall : public Ball {
public:
    explicit ExtraBall() = delete;
    explicit ExtraBall(const ExtraBall& other) = default;
    explicit ExtraBall(ExtraBall&& other) noexcept = default;
    explicit ExtraBall(Game& parent, const sf::Vector2f& pos, const sf::Vector2f& velocity, const float radius,
                       const int fallScoreIncrease = 0, const sf::Color& color = sf::Color::Green);
    virtual ~ExtraBall() = default;
    virtual bool isDead() const override;
protected:
    virtual void _handleWindowCollision() override;
private:
    bool _isDead;
};