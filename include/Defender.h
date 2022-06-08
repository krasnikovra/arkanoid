#pragma once
#include "Attacker.h"
#include <SFML/Graphics.hpp>
#include "Utils.h"
#include <functional>

class Defender {
public:
    virtual ~Defender() = default;
    virtual std::function<void(void)> hitBy(const Attacker& attacker) = 0;
};

class DefenderBall : public Defender {
public:
    virtual ~DefenderBall() = default;
    virtual std::function<void(void)> hitBy(const Attacker& attacker) override {
        return attacker.collideWith(*this);
    }
    bool isCloseTo(const sf::Vector2f& point) const {
        return DistSqr(getMidPoint(), point) < getRadius() * getRadius();
    };
    sf::Vector2f getMidPoint() const {
        return getPosition() + sf::Vector2f(getRadius(), getRadius());
    };
    virtual sf::Vector2f getPosition() const = 0;
    virtual float getRadius() const = 0;
    virtual sf::Vector2f getVelocity() const = 0;
    virtual void setPosition(const sf::Vector2f&) = 0;
    virtual void setVelocity(const sf::Vector2f&) = 0;
    virtual void boostUp(const float multiplier, const float timeMs) = 0;
};

class DefenderBlock : public Defender {
public:
    virtual ~DefenderBlock() = default;
    virtual std::function<void(void)> hitBy(const Attacker& attacker) override {
        return attacker.collideWith(*this);
    }
    virtual bool isDead() const = 0;
    virtual void damage() = 0;
    virtual sf::Vector2f getPosition() const = 0;
    virtual sf::Vector2f getSize() const = 0;
};
