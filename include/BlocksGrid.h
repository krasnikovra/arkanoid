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
                        const unsigned bonusBlockHealth, const unsigned movingBlockHealth,
                        const float boostMultiplier, const float boostTimeMs, const float movingBlockSpeed);
    ~BlocksGrid() = default;
    std::shared_ptr<Block>& getBlock(const unsigned u, const unsigned v);
    std::shared_ptr<Block>& getBlock(const sf::Vector2u& uv);
    sf::Vector2u getUVSize() const;
    void draw();
    std::shared_ptr<Block> spawnMovingBlock();
    void destroyDeadBlocks();
private:
    sf::Vector2f _blockPos(const unsigned u, const unsigned v) const;
    sf::Vector2f _blockPos(const sf::Vector2u& uv) const;
    Game& _parent;
    sf::Vector2u _uvSize;
    std::vector<std::shared_ptr<Block>> _blocks;
    sf::FloatRect _rect;
    sf::Vector2f _blockSize;
    unsigned _movingBlockHealth;
    float _movingBlockSpeed;
};
