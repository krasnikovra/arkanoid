#pragma once
#include <functional>
#include "SFML/Graphics.hpp"
#include "Defender.h"

// this is some kind of visitor pattern
// so the Attacker is a visitor of defender
// we visit defender and change its state
// here we return std::function
// to handle all the collisions
// and only after confirm defenders' changes
class Attacker {
public:
    virtual ~Attacker() = default;
    virtual std::function<void(void)> collideWith(Defender& defender) const = 0;
};

class AttackerBall : public Attacker {
public:
    virtual ~AttackerBall() = default;
    virtual std::function<void(void)> collideWith(Defender& defender) const {
        return defender.hitBy(*this);
    }
};

class AttackerBox : public Attacker {
public:
    virtual ~AttackerBox() = default;
    virtual std::function<void(void)> collideWith(Defender& defender) const {
        return defender.hitBy(*this);
    }
};

class AttackerRacket : public Attacker {
public:
    virtual ~AttackerRacket() = default;
    virtual std::function<void(void)> collideWith(Defender& defender) const {
        return defender.hitBy(*this);
    }
    virtual float getSpeed() const = 0;
    virtual sf::Vector2f getPosition() const = 0;
    virtual sf::Vector2f getSize() const = 0;
};