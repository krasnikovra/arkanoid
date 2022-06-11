#pragma once
#include <functional>

class DefenderBall;
class DefenderBlock;
class DefenderBonus;

class Attacker {
public:
    virtual ~Attacker() = default;
    virtual std::function<void(void)> collideWith(DefenderBall& ball) const = 0;
    virtual std::function<void(void)> collideWith(DefenderBlock& block) const = 0;
    virtual std::function<void(void)> collideWith(DefenderBonus& bonus) const = 0;
};
