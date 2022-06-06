#include "CollisionsManager.h"
#include "Attacker.h"
#include <functional>

void CollisionsManager::addAttacker(const std::shared_ptr<Attacker>& attacker) {
    _attackers.push_back(attacker);
}

void CollisionsManager::addDefender(const std::shared_ptr<Defender>& defender) {
    _defenders.push_back(defender);
}

void CollisionsManager::handleCollisions() {
    std::vector<std::function<void(void)>> callbacks;
    for (auto attackerIt = _attackers.begin(); attackerIt != _attackers.end(); attackerIt++)
        for (auto defenderIt = _defenders.begin(); defenderIt != _defenders.end(); defenderIt++) {
            if (attackerIt->expired()) {
                _attackers.erase(attackerIt);
                continue;
            }
            if (defenderIt->expired()) {
                _defenders.erase(defenderIt);
                continue;
            }
            auto defenderPtr = defenderIt->lock();
            auto attackerPtr = attackerIt->lock();
            callbacks.push_back(attackerPtr->collideWith(*defenderPtr));
        }
    for (auto& callback : callbacks)
        std::invoke(callback);
}
