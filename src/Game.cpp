#include "Game.h"
#include <iostream>

#define GAME_NAME "Arkanoid"

const sf::Vector2f BALL_VEL = sf::Vector2f(0.4f, 0.4f);
constexpr float BALL_RADIUS = 10.0f;
constexpr float RACKET_Y = 0.8f;
constexpr float RACKET_SPEED = 1.0f;
const sf::Vector2f RACKET_SIZE = sf::Vector2f(0.2f, 0.02f);
constexpr float BLOCKS_GRID_Y = 0.1f;
constexpr float BLOCKS_GRID_HEIGHT = 0.3f;
constexpr float BLOCKS_GRID_MARGINX = 0.15f;
const sf::Vector2u BLOCKS_GRID_UV = sf::Vector2u(5, 7);
constexpr unsigned SOFT_BLOCK_HEALTH = 2;
constexpr unsigned BOOST_BLOCK_HEALTH = 1;
constexpr unsigned BONUS_BLOCK_HEALTH = 1;
constexpr float BOOST_BLOCK_MULTIPLIER = 1.5f;
constexpr float BOOST_BLOCK_DURATIONMS = 1000.0f;
const sf::Vector2f BONUS_SIZE = sf::Vector2f(0.02f, 0.02f);
constexpr float BONUS_SPEED = 0.1f;
constexpr unsigned MOVING_BLOCK_HEALTH = 3;
constexpr float MOVING_BLOCK_SPEED = 0.1f;
const std::string ARIAL_FONT_PATH = "res/fonts/arial.ttf";

Game::Game() : Game(800, 600) {}

Game::Game(const unsigned winWidth, const unsigned winHeight) :
    _timeDeltaMs(0), _score(0),
    _window(std::make_unique<sf::RenderWindow>(sf::VideoMode(winWidth, winHeight), GAME_NAME)),
    _balls({ std::make_shared<Ball>(*this, BALL_VEL, BALL_RADIUS) }),
    _racket(std::make_shared<Racket>(*this, winHeight * RACKET_Y, RACKET_SPEED,
            sf::Vector2f(winWidth * RACKET_SIZE.x, winHeight * RACKET_SIZE.y))),
    _collisionsManager(std::make_unique<CollisionsManager>()), 
    _scoreLabel(std::make_unique<Label>(*this, ARIAL_FONT_PATH)),
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
                BONUS_BLOCK_HEALTH,
                MOVING_BLOCK_HEALTH,
                BOOST_BLOCK_MULTIPLIER,
                BOOST_BLOCK_DURATIONMS,
                MOVING_BLOCK_SPEED)) {
    _collisionsManager->addAttacker(_balls[0]);
    _collisionsManager->addDefender(_balls[0]);
    _collisionsManager->addAttacker(_racket);
    _collisionsManager->addAttacker(*_blocksGrid);
    _collisionsManager->addDefender(*_blocksGrid);
}

std::unique_ptr<sf::RenderWindow>& Game::getWindow() {
    return _window;
}

float Game::getTimeMsSinceLastFrame() const {
    return _timeDeltaMs;
}

void Game::spawnExtraBall() {
    float radius = BALL_RADIUS;
    sf::Vector2f ballPos = _racket->getPosition() + sf::Vector2f(_racket->getSize().x / 2 - radius, -2 * radius);
    std::shared_ptr<Ball> newBall = std::make_shared<ExtraBall>(*this, ballPos, BALL_VEL, radius);
    _balls.push_back(newBall);
    _collisionsManager->addAttacker(newBall);
    _collisionsManager->addDefender(newBall);
}

void Game::spawnRandomBonus(const sf::Vector2f& pos) {
    sf::Vector2f bonusSize(
        BONUS_SIZE.x * _window->getSize().x,
        BONUS_SIZE.y * _window->getSize().y
    );
    std::shared_ptr<Bonus> newBonus;
    switch (rand() % 2) {
    case 0:
        newBonus = std::make_shared<ExtraBallBonus>(*this, pos - bonusSize / 2.0f, bonusSize, BONUS_SPEED);
        break;
    case 1:
        newBonus = std::make_shared<MovingBlockBonus>(*this, pos - bonusSize / 2.0f, bonusSize, BONUS_SPEED);
        break;
    }
    _bonuses.push_back(newBonus);
    _collisionsManager->addDefender(newBonus);
}

void Game::spawnMovingBlock() {
    auto block = _blocksGrid->spawnMovingBlock();
    if (!block)
        return;
    _collisionsManager->addAttacker(block);
    _collisionsManager->addDefender(block);
}

void Game::increaseScore(const int score) {
    _score += score;
}

int Game::run() {
    _lastTimePoint = std::chrono::high_resolution_clock::now();
    while (_window->isOpen()) {
        _refreshTimeDeltaMs();
        sf::Event event;
        while (_window->pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                _window->close();
                return 0;
            case sf::Event::MouseButtonReleased: //debug
                spawnMovingBlock();
                break;
            default:
                _racket->handleEvent(event);
            }
        }
        _window->clear(sf::Color::White);
        _collisionsManager->handleCollisions();
        _updateScoreLabel();
        _blocksGrid->draw();
        _racket->draw();
        for (auto& ball : _balls)
            ball->draw();
        for (auto& bonus : _bonuses)
            bonus->draw();
        _scoreLabel->draw();
        _window->display();
        _blocksGrid->destroyDeadBlocks();
        _destroyDeadBalls();
        _destroyDeadBonuses();
    }
    return 0;
}

void Game::_refreshTimeDeltaMs() {
    auto now = std::chrono::high_resolution_clock::now();
    _timeDeltaMs = std::chrono::duration_cast<std::chrono::nanoseconds>(now - _lastTimePoint).count() / 1e6f;
    _lastTimePoint = now;
}

void Game::_destroyDeadBalls() {
    auto ballIt = _balls.begin();
    while (ballIt != _balls.end()) {
        if ((*ballIt)->isDead())
            ballIt = _balls.erase(ballIt);
        else
            ballIt++;
    }
}

void Game::_destroyDeadBonuses() {
    auto bonusIt = _bonuses.begin();
    while (bonusIt != _bonuses.end()) {
        if ((*bonusIt)->isDead())
            bonusIt = _bonuses.erase(bonusIt);
        else
            bonusIt++;
    }
}

void Game::_updateScoreLabel() {
    _scoreLabel->setText(L"Score: " + std::to_wstring(_score));
}
