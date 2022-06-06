#include "Game.h"
#include <iostream>

#define GAME_NAME "Arkanoid"

const sf::Vector2f BALL_VEL = sf::Vector2f(0.3f, 0.3f);
const sf::Color BALL_COLOR = sf::Color::Red;
constexpr float BALL_RADIUS = 10.0f;
constexpr float RACKET_Y = 0.8f;
constexpr float RACKET_SPEED = 1.0f;
const sf::Vector2f RACKET_SIZE = sf::Vector2f(0.2f, 0.02f);
const sf::Color RACKET_COLOR = sf::Color::Black;

Game::Game() : Game(800, 600) {}

Game::Game(const unsigned winWidth, const unsigned winHeight) :
    _window(std::make_unique<sf::RenderWindow>(sf::VideoMode(winWidth, winHeight), GAME_NAME)),
    _ball(std::make_shared<Ball>(*this, BALL_VEL, BALL_RADIUS, BALL_COLOR)),
    _racket(std::make_shared<Racket>(*this, winHeight * RACKET_Y, RACKET_SPEED,
            sf::Vector2f(winWidth * RACKET_SIZE.x, winHeight * RACKET_SIZE.y), RACKET_COLOR)),
    _collisionsManager(std::make_unique<CollisionsManager>()) {
    _collisionsManager->addAttacker(_racket);
    _collisionsManager->addDefender(_ball);
}

std::unique_ptr<sf::RenderWindow>& Game::getWindow() {
    return _window;
}

float Game::getTimeMsSinceLastFrame() {
    auto now = std::chrono::system_clock::now();
    float timeDeltaMs = std::chrono::duration<float, std::milli>(now - _lastTimePoint).count();
    return timeDeltaMs;
}

int Game::run() {
    while (_window->isOpen()) {
        sf::sleep(sf::milliseconds(1)); //smth awkward going on without this (seemes like timer becomes drunk or wtf)
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
        _racket->draw();
        _ball->draw();
        _window->display();
        _lastTimePoint = std::chrono::system_clock::now();
    }
    return 0;
}
