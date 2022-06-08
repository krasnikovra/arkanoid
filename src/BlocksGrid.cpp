#include "BlocksGrid.h"

BlocksGrid::BlocksGrid(Game& parent, const sf::FloatRect& rect, const sf::Vector2u& uvSize,
                       const unsigned softBlockHealth, const unsigned boostBlockHealth,
                       const float boostMultiplier, const float boostTimeMs):
    _parent(parent), _uvSize(uvSize), _blocks(std::vector<std::shared_ptr<Block>>(uvSize.x * uvSize.y)) {
    const sf::Vector2f blockSize(rect.width / uvSize.x, rect.height / uvSize.y);
    auto blockPos = [&](const unsigned u, const unsigned v) -> sf::Vector2f {
        return sf::Vector2f(rect.left + u * blockSize.x, rect.top + v * blockSize.y);
    };
    for (unsigned i = 0; i < uvSize.x; i++)
        for (unsigned j = 0; j < uvSize.y; j++)
            switch (rand() % 3) {
            case 0:
                getBlock(i, j) = std::make_unique<Block>(parent, blockPos(i, j), blockSize);
                break;
            case 1:
                getBlock(i, j) = std::make_unique<SoftBlock>(parent, blockPos(i, j), blockSize, softBlockHealth);
                break;
            case 2:
                getBlock(i, j) = std::make_unique<BoostBlock>(parent, blockPos(i, j), blockSize, boostBlockHealth, boostMultiplier, boostTimeMs);
                break;
            }
}

std::shared_ptr<Block>& BlocksGrid::getBlock(const unsigned u, const unsigned v) {
    return _blocks[u + v * _uvSize.x];
}

sf::Vector2u BlocksGrid::getUVSize() const {
    return _uvSize;
}

void BlocksGrid::draw() {
    for (auto& block : _blocks)
        if (block)
            block->draw();
}

void BlocksGrid::destroyDeadBlocks() {
    for (auto& block : _blocks)
        if (block && block->isDead())
            block = nullptr;
}
