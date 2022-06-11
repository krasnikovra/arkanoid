#pragma once
#include <SFML/Graphics.hpp>

static float DistSqr(const sf::Vector2f& a, const sf::Vector2f& b) {
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
};

static float Dot(const sf::Vector2f& a, const sf::Vector2f& b) {
    return (a.x * b.x) + (a.y * b.y);
};