#pragma once
#include "SFML/Graphics.hpp"
#include "Attacker.h"

class Game;

// basically, racket is not defender because its not needed
// due to the game logic
class Racket : public AttackerRacket {
public:
    explicit Racket() = default;
    explicit Racket(const Racket& other) = default;
    explicit Racket(Racket&& other) = default;
    explicit Racket(Game& parent, const float y, const float speed, const sf::Vector2f& size, const sf::Color& color);
    virtual ~Racket() = default;
    void draw();
    void handleEvent(const sf::Event& event);
    // AttackerRacket's methods
    virtual float getSpeed() const;
    virtual sf::Vector2f getPosition() const;
    virtual sf::Vector2f getSize() const;
private:
    void _move();
    void _update();
    void _handleWindowCollision();

    Game& _parent;
    std::unique_ptr<sf::RectangleShape> _shape;
    float _speedDefault;
    float _speed;
};