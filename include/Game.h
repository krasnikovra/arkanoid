#pragma once
#include <SFML/Graphics.hpp>
#include "Ball.h"
#include "Racket.h"
#include "Block.h"
#include "BlocksGrid.h"
#include "CollisionsManager.h"
#include "Bonus.h"
#include "Label.h"

class Game {
public:
    explicit Game();
    explicit Game(const unsigned winWidth, const unsigned winHeight);
    explicit Game(const Game& other) = delete;
    explicit Game(Game&& other) noexcept = delete;
    ~Game() = default;
    std::unique_ptr<sf::RenderWindow>& getWindow();
    float getTimeMsSinceLastFrame() const;
    void spawnExtraBall();
    void spawnRandomBonus(const sf::Vector2f& pos);
    void spawnMovingBlock();
    void increaseScore(const int score);
    int run();
private:
    void _refreshTimeDeltaMs();
    void _destroyDeadBalls();
    void _destroyDeadBonuses();
    void _updateScoreLabel();

    std::unique_ptr<sf::RenderWindow> _window;
    std::vector<std::shared_ptr<Ball>> _balls;
    std::shared_ptr<Racket> _racket;
    std::unique_ptr<CollisionsManager> _collisionsManager;
    std::unique_ptr<BlocksGrid> _blocksGrid;
    std::vector<std::shared_ptr<Bonus>> _bonuses;
    std::unique_ptr<Label> _scoreLabel;
    std::chrono::high_resolution_clock::time_point _lastTimePoint;
    float _timeDeltaMs;
    int _score;
};