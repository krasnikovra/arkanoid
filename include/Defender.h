#pragma once
#include <functional>

class AttackerBall;
class AttackerBox;
class AttackerRacket;

// this is some kind of visitor pattern
// so the Defender is an element visited by the attacker
// he visits defender and change its state
class Defender {
public:
    virtual ~Defender() = default;
    virtual std::function<void(void)> hitBy(const AttackerBall& ball) = 0;
    virtual std::function<void(void)> hitBy(const AttackerBox& box) = 0;
    virtual std::function<void(void)> hitBy(const AttackerRacket& racket) = 0;
};
