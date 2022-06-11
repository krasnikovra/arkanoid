#pragma once

#include <SFML/Graphics.hpp>
#include "Defender.h"

class Game;

class Bonus : public DefenderBonus {
public:
    explicit Bonus() = delete;
    explicit Bonus(const Bonus& other) = default;
    explicit Bonus(Bonus&& other) = default;
    explicit Bonus(Game& parent, const sf::Vector2f& pos, const sf::Vector2f& size,
                   const float speed, const sf::Color& color);
    virtual ~Bonus() = default;
    void draw();
    // DefenderBonus's methods
    virtual bool isDead() const override;
    virtual void invokeBonusAction() = 0;
    virtual sf::Vector2f getPosition() const override;
    virtual sf::Vector2f getSize() const override;
protected:
    void _move();

    Game& _parent;
    sf::RectangleShape _shape;
    float _speed;
    bool _isDead;
};

class ExtraBallBonus : public Bonus {
public:
    explicit ExtraBallBonus() = delete;
    explicit ExtraBallBonus(const ExtraBallBonus& other) = default;
    explicit ExtraBallBonus(ExtraBallBonus&& other) = default;
    explicit ExtraBallBonus(Game& parent, const sf::Vector2f& pos, const sf::Vector2f& size,
                   const float speed, const sf::Color& color = sf::Color::Magenta);
    virtual ~ExtraBallBonus() = default;
    virtual void invokeBonusAction() override;
};

class MovingBlockBonus : public Bonus {
public:
    explicit MovingBlockBonus() = delete;
    explicit MovingBlockBonus(const MovingBlockBonus& other) = default;
    explicit MovingBlockBonus(MovingBlockBonus&& other) = default;
    explicit MovingBlockBonus(Game& parent, const sf::Vector2f& pos, const sf::Vector2f& size,
                              const float speed, const sf::Color& color = sf::Color(255, 165, 0, 255));
    virtual ~MovingBlockBonus() = default;
    virtual void invokeBonusAction() override;
};