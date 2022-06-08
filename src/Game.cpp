#include "Game.h"
#include <iostream>

#define GAME_NAME "Arkanoid"

const sf::Vector2f BALL_VEL = sf::Vector2f(0.5f, 0.5f);
const sf::Color BALL_COLOR = sf::Color::Red;
constexpr float BALL_RADIUS = 10.0f;
constexpr float RACKET_Y = 0.8f;
constexpr float RACKET_SPEED = 1.0f;
const sf::Vector2f RACKET_SIZE = sf::Vector2f(0.2f, 0.02f);
const sf::Color RACKET_COLOR = sf::Color::Black;
constexpr float BLOCKS_GRID_Y = 0.1f;
constexpr float BLOCKS_GRID_HEIGHT = 0.3f;
constexpr float BLOCKS_GRID_MARGINX = 0.15f;
const sf::Vector2u BLOCKS_GRID_UV = sf::Vector2u(5, 7);
constexpr unsigned SOFT_BLOCK_HEALTH = 1;
constexpr unsigned BOOST_BLOCK_HEALTH = 1;
constexpr float BOOST_BLOCK_MULTIPLIER = 1.5f;
constexpr float BOOST_BLOCK_DURATIONMS = 1000.0f;

Game::Game() : Game(800, 600) {}

Game::Game(const unsigned winWidth, const unsigned winHeight) :
    _window(std::make_unique<sf::RenderWindow>(sf::VideoMode(winWidth, winHeight), GAME_NAME)),
    _ball(std::make_shared<Ball>(*this, BALL_VEL, BALL_RADIUS, BALL_COLOR)),
    _racket(std::make_shared<Racket>(*this, winHeight * RACKET_Y, RACKET_SPEED,
            sf::Vector2f(winWidth * RACKET_SIZE.x, winHeight * RACKET_SIZE.y), RACKET_COLOR)),
    _collisionsManager(std::make_unique<CollisionsManager>()), 
    _blocksGrid(std::make_unique<BlocksGrid>(
                *this,
                sf::FloatRect(BLOCKS_GRID_MARGINX * winWidth,
                              BLOCKS_GRID_Y * winHeight,
                              winWidth * (1.0f - 2.0f * BLOCKS_GRID_MARGINX),
                              BLOCKS_GRID_HEIGHT * winHeight
                             ),
                BLOCKS_GRID_UV, 
                SOFT_BLOCK_HEALTH,
                BOOST_BLOCK_HEALTH,
                BOOST_BLOCK_MULTIPLIER,
                BOOST_BLOCK_DURATIONMS)) {
    _collisionsManager->addAttacker(_racket);
    _collisionsManager->addAttacker(_ball);
    _collisionsManager->addDefender(_ball);
    _collisionsManager->addAttacker(*_blocksGrid);
    _collisionsManager->addDefender(*_blocksGrid);
}

std::unique_ptr<sf::RenderWindow>& Game::getWindow() {
    return _window;
}

float Game::getTimeMsSinceLastFrame() const {
    return _timeDeltaMs;
}

int Game::run() {
    while (_window->isOpen()) {
        _refreshTimeDeltaMs();
        sf::Event event;
        while (_window->pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                _window->close();
                return 0;
            default:
                _racket->handleEvent(event);
            }
        }
        _window->clear(sf::Color::White);
        _collisionsManager->handleCollisions();
        _blocksGrid->draw();
        _racket->draw();
        _ball->draw();
        _window->display();
        _blocksGrid->destroyDeadBlocks();
    }
    return 0;
}

void Game::_refreshTimeDeltaMs() {
    auto now = std::chrono::high_resolution_clock::now();
    _timeDeltaMs = std::chrono::duration_cast<std::chrono::nanoseconds>(now - _lastTimePoint).count() / 1e6f;
    _lastTimePoint = now;
}
