#pragma once
#include <SFML/Graphics.hpp>
#include "Ball.h"
#include "Racket.h"
#include "CollisionsManager.h"

class Game {
public:
    explicit Game();
    explicit Game(const unsigned winWidth, const unsigned winHeight);
    explicit Game(const Game& other) = delete;
    explicit Game(Game&& other) noexcept = delete;
    ~Game() = default;
    std::unique_ptr<sf::RenderWindow>& getWindow();
    float getTimeMsSinceLastFrame();
    int run();
private:
    std::unique_ptr<sf::RenderWindow> _window;
    std::shared_ptr<Ball> _ball;
    std::shared_ptr<Racket> _racket;
    std::unique_ptr<CollisionsManager> _collisionsManager;
    std::chrono::time_point<std::chrono::system_clock> _lastTimePoint;
};