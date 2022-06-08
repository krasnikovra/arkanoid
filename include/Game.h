#pragma once
#include <SFML/Graphics.hpp>
#include "Ball.h"
#include "Racket.h"
#include "Block.h"
#include "BlocksGrid.h"
#include "CollisionsManager.h"

class Game {
public:
    explicit Game();
    explicit Game(const unsigned winWidth, const unsigned winHeight);
    explicit Game(const Game& other) = delete;
    explicit Game(Game&& other) noexcept = delete;
    ~Game() = default;
    std::unique_ptr<sf::RenderWindow>& getWindow();
    float getTimeMsSinceLastFrame() const;
    int run();
private:
    void _refreshTimeDeltaMs();

    std::unique_ptr<sf::RenderWindow> _window;
    std::shared_ptr<Ball> _ball;
    std::shared_ptr<Racket> _racket;
    std::unique_ptr<CollisionsManager> _collisionsManager;
    std::unique_ptr<BlocksGrid> _blocksGrid;
    std::chrono::high_resolution_clock::time_point _lastTimePoint;
    float _timeDeltaMs;
};