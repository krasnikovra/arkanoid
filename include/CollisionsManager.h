#pragma once
#include <vector>
#include <memory>

class Attacker;
class Defender;

class CollisionsManager {
public:
    explicit CollisionsManager() = default;
    explicit CollisionsManager(const CollisionsManager& other) = default;
    explicit CollisionsManager(CollisionsManager&& other) = default;
    void addAtacker(const std::shared_ptr<Attacker>& attacker);
    void addDefender(const std::shared_ptr<Defender>& defender);
    void handleCollisions();
private:
    std::vector<std::weak_ptr<Attacker>> _attackers;
    std::vector<std::weak_ptr<Defender>> _defenders;
};