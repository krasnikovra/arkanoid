#pragma once
#include "Block.h"
// wrapper for the grid of blocks

class BlocksGrid {
public:
    explicit BlocksGrid() = delete;
    explicit BlocksGrid(const BlocksGrid& other) = default;
    explicit BlocksGrid(BlocksGrid&& other) = default;
    explicit BlocksGrid(Game& parent, const sf::FloatRect& rect, const sf::Vector2u& uvSize,
                        const unsigned softBlockHealth, const unsigned boostBlockHealth,
                        const float boostMultiplier, const float boostTimeMs);
    ~BlocksGrid() = default;
    std::shared_ptr<Block>& getBlock(const unsigned u, const unsigned v);
    sf::Vector2u getUVSize() const;
    void draw();
    void destroyDeadBlocks();
private:
    Game& _parent;
    sf::Vector2u _uvSize;
    std::vector<std::shared_ptr<Block>> _blocks;
};
