#pragma once
#include "SFML/Graphics.hpp"
#include "Attacker.h"
#include "Defender.h"

class Game;

// basically, racket is not defender because its not needed
// due to the game logic
class Racket : public Attacker {
public:
    explicit Racket() = delete;
    explicit Racket(const Racket& other) = default;
    explicit Racket(Racket&& other) noexcept = default;
    explicit Racket(Game& parent, const float y, const float speed, const sf::Vector2f& size, const sf::Color& color);
    virtual ~Racket() = default;
    void draw();
    void handleEvent(const sf::Event& event);
    // Attacker's methods
    virtual std::function<void(void)> collideWith(DefenderBall& ball) const override;
    virtual std::function<void(void)> collideWith(DefenderBlock& block) const override;
private:
    void _move();
    void _update();
    void _handleWindowCollision();

    Game& _parent;
    sf::RectangleShape _shape;
    float _speedDefault;
    float _speed;
};