#include "CollisionsManager.h"
#include "Attacker.h"
#include "BlocksGrid.h"
#include <functional>

void CollisionsManager::addAttacker(const std::shared_ptr<Attacker>& attacker) {
    _attackers.push_back(attacker);
}

void CollisionsManager::addDefender(const std::shared_ptr<Defender>& defender) {
    _defenders.push_back(defender);
}

void CollisionsManager::addAttacker(BlocksGrid& attacker) {
    sf::Vector2u uvSize = attacker.getUVSize();
    for (unsigned i = 0; i < uvSize.x; i++)
        for (unsigned j = 0; j < uvSize.y; j++)
            _attackers.push_back(attacker.getBlock(i, j));
}

void CollisionsManager::addDefender(BlocksGrid& defender) {
    sf::Vector2u uvSize = defender.getUVSize();
    for (unsigned i = 0; i < uvSize.x; i++)
        for (unsigned j = 0; j < uvSize.y; j++)
            _defenders.push_back(defender.getBlock(i, j));
}

void CollisionsManager::handleCollisions() {
    std::vector<std::function<void(void)>> callbacks;
    auto defenderIt = _defenders.begin();
    while (defenderIt != _defenders.end()) {
        if (defenderIt->expired())
            defenderIt = _defenders.erase(defenderIt);
        else {
            auto attackerIt = _attackers.begin();
            while (attackerIt != _attackers.end()) {
                if (attackerIt->expired())
                    attackerIt = _attackers.erase(attackerIt);
                else {
                    auto defenderPtr = defenderIt->lock();
                    auto attackerPtr = attackerIt->lock();
                    callbacks.push_back(defenderPtr->hitBy(*attackerPtr));
                    attackerIt++;
                }
            }
            defenderIt++;
        }
    }
    for (auto& callback : callbacks)
        std::invoke(callback);
}
