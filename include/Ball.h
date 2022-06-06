#pragma once
#include <SFML/Graphics.hpp>
#include "Attacker.h"
#include <chrono>

class Game;

class Ball : public Defender, public AttackerBall {
public:
    explicit Ball() = default;
    explicit Ball(const Ball& other) = default;
    explicit Ball(Ball&& other) = default;
    explicit Ball(Game& parent, const sf::Vector2f& velocity, const float radius, const sf::Color& color);
    virtual ~Ball() = default;
    void draw();
    // Defender's methods
    virtual std::function<void(void)> hitBy(const AttackerBall& ball);
    virtual std::function<void(void)> hitBy(const AttackerBox& box);
    virtual std::function<void(void)> hitBy(const AttackerRacket& racket);
private:
    void _move();
    void _update();
    void _handleWindowCollision();
    void _handleRacketCollision();

    Game& _parent;
    std::unique_ptr<sf::CircleShape> _shape;
    sf::Vector2f _velocity;
};