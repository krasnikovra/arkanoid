#pragma once
#include "Attacker.h"
#include "Defender.h"
#include <SFML/Graphics.hpp>

class Game;

class Block : public DefenderBlock, public Attacker {
public:
    explicit Block() = delete;
    explicit Block(const Block& other) = default;
    explicit Block(Block&& other) noexcept = default;
    explicit Block(Game& parent, const sf::Vector2f& pos, const sf::Vector2f& size,
                   const sf::Color& color = sf::Color(100, 100, 100, 255));
    virtual ~Block() = default;
    virtual void draw();
    // DefenderBlock's methods
    virtual void setSpeed(const float speed) override;
    virtual float getSpeed() const override;
    virtual void setPosition(const sf::Vector2f& pos) override;
    virtual bool isDead() const override;
    virtual void damage() override;
    virtual sf::Vector2f getPosition() const override;
    virtual sf::Vector2f getSize() const override;
    // Attacker's methods
    virtual std::function<void(void)> collideWith(DefenderBall& ball) const override;
    virtual std::function<void(void)> collideWith(DefenderBlock& block) const override;
    virtual std::function<void(void)> collideWith(DefenderBonus& bonus) const override;
protected:
    bool _collideWithBall(const sf::Vector2f& ballBackupPos, const sf::Vector2f& selfBackupPos, DefenderBall& ball) const;

    Game& _parent;
    sf::RectangleShape _shape;
};

class SoftBlock : public Block {
public:
    explicit SoftBlock() = delete;
    explicit SoftBlock(const SoftBlock& other) = default;
    explicit SoftBlock(SoftBlock&& other) noexcept = default;
    explicit SoftBlock(Game& parent, const sf::Vector2f& pos, const sf::Vector2f& size,
                       const int health, const int hitScore = 10, const sf::Color& color = sf::Color(0, 220, 0, 255));
    virtual bool isDead() const override;
    virtual void damage() override;
    virtual std::function<void(void)> collideWith(DefenderBall& ball) const override;
protected:
    int _healthDefault;
    int _health;
    int _hitScore;
};

class DeathActionBlock : public SoftBlock {
public:
    explicit DeathActionBlock() = delete;
    explicit DeathActionBlock(const DeathActionBlock& other) = default;
    explicit DeathActionBlock(DeathActionBlock&& other) noexcept = default;
    explicit DeathActionBlock(Game& parent, const sf::Vector2f& pos, const sf::Vector2f& size,
                        const int health, const int hitScore, const sf::Color& color);
    virtual std::function<void(void)> collideWith(DefenderBall& ball) const override;
private:
    virtual void _deathAction(DefenderBall& ball) const = 0;
};

class BoostBlock : public DeathActionBlock {
public:
    explicit BoostBlock() = delete;
    explicit BoostBlock(const BoostBlock& other) = default;
    explicit BoostBlock(BoostBlock&& other) noexcept = default;
    explicit BoostBlock(Game& parent, const sf::Vector2f& pos, const sf::Vector2f& size,
                        const int health, const float multiplier, const float durationMs,
                        const int hitScore = 15, const sf::Color& color = sf::Color::Red);
private:
    virtual void _deathAction(DefenderBall& ball) const override;
    float _multiplier;
    float _durationMs;
};

class BonusBlock : public DeathActionBlock {
public:
    explicit BonusBlock() = delete;
    explicit BonusBlock(const BonusBlock& other) = default;
    explicit BonusBlock(BonusBlock&& other) noexcept = default;
    explicit BonusBlock(Game& parent, const sf::Vector2f& pos, const sf::Vector2f& size, const int health,
                        const int hitScore = 15, const sf::Color& color = sf::Color::Cyan);
private:
    virtual void _deathAction(DefenderBall& ball) const override;
};

class MovingBlock : public SoftBlock {
public:
    explicit MovingBlock() = delete;
    explicit MovingBlock(const MovingBlock& other) = default;
    explicit MovingBlock(MovingBlock&& other) noexcept = default;
    explicit MovingBlock(Game & parent, const sf::Vector2f & pos, const sf::Vector2f& size, const float speed,
                        const int health, const int hitScore = 30, const sf::Color & color = sf::Color::Yellow);
    virtual void setSpeed(const float speed) override;
    virtual float getSpeed() const override;
    virtual void setPosition(const sf::Vector2f& pos) override;
    virtual void draw() override;
private:
    void _move();
    void _update();
    void _handleWindowCollision();
    float _speed;
};