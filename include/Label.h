#pragma once

#include <string>
#include <SFML/Graphics.hpp>

class Game;

class Label {
public:
    explicit Label() = delete;
    explicit Label(const Label& other) = default;
    explicit Label(Label&& other) = default;
    explicit Label(Game& parent, const std::string& fontPath, const sf::Vector2f& pos = sf::Vector2f(10,10),
                   const unsigned charSize = 24, const sf::Color& color = sf::Color::Black);
    ~Label() = default;
    void draw();
    void setText(const std::wstring& text);
private:
    Game& _parent;
    sf::Font _font;
    sf::Text _text;
};