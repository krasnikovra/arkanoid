#include "BlocksGrid.h"

BlocksGrid::BlocksGrid(Game& parent, const sf::FloatRect& rect, const sf::Vector2u& uvSize,
                       const unsigned softBlockHealth, const unsigned boostBlockHealth,
                       const unsigned bonusBlockHealth, const unsigned movingBlockHealth,
                       const float boostMultiplier, const float boostTimeMs, const float movingBlockSpeed) :
    _parent(parent), _uvSize(uvSize), _rect(rect), _blockSize(rect.width / uvSize.x, rect.height / uvSize.y),
    _movingBlockHealth(movingBlockHealth), _movingBlockSpeed(movingBlockSpeed), _blocks(std::vector<std::shared_ptr<Block>>(uvSize.x* uvSize.y)) {
    for (unsigned i = 0; i < uvSize.x; i++)
        for (unsigned j = 0; j < uvSize.y; j++)
            switch (rand() % 4) {
            case 0:
                getBlock(i, j) = std::make_shared<Block>(parent, _blockPos(i, j), _blockSize);
                break;
            case 1:
                getBlock(i, j) = std::make_shared<SoftBlock>(parent, _blockPos(i, j), _blockSize, softBlockHealth);
                break;
            case 2:
                getBlock(i, j) = std::make_shared<BoostBlock>(parent, _blockPos(i, j), _blockSize, boostBlockHealth, boostMultiplier, boostTimeMs);
                break;
            case 3:
                getBlock(i, j) = std::make_shared<BonusBlock>(parent, _blockPos(i, j), _blockSize, bonusBlockHealth);
                break;
            }
}

std::shared_ptr<Block>& BlocksGrid::getBlock(const unsigned u, const unsigned v) {
    return _blocks[u + v * _uvSize.x];
}

std::shared_ptr<Block>& BlocksGrid::getBlock(const sf::Vector2u& uv) {
    return getBlock(uv.x, uv.y);
}

sf::Vector2u BlocksGrid::getUVSize() const {
    return _uvSize;
}

void BlocksGrid::draw() {
    for (auto& block : _blocks)
        if (block)
            block->draw();
}

std::shared_ptr<Block> BlocksGrid::spawnMovingBlock() {
    std::vector<sf::Vector2u> pos;
    for (unsigned i = 0; i < _uvSize.x; i++)
        for (unsigned j = 0; j < _uvSize.y; j++)
            if (!getBlock(i, j))
                pos.push_back(sf::Vector2u(i, j));
    if (pos.size() == 0)
        return nullptr;
    sf::Vector2u blockPos = pos[rand() % pos.size()];
    getBlock(blockPos) = std::make_shared<MovingBlock>(_parent, _blockPos(blockPos), _blockSize, _movingBlockSpeed, _movingBlockHealth);
    return getBlock(blockPos);
}

void BlocksGrid::destroyDeadBlocks() {
    for (auto& block : _blocks)
        if (block && block->isDead())
            block = nullptr;
}

sf::Vector2f BlocksGrid::_blockPos(const unsigned u, const unsigned v) const {
    return sf::Vector2f(_rect.left + u * _blockSize.x, _rect.top + v * _blockSize.y);
}

sf::Vector2f BlocksGrid::_blockPos(const sf::Vector2u& uv) const {
    return _blockPos(uv.x, uv.y);
}
