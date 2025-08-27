#include "GameObjectManager.hpp"
#include "iostream"

void GameObjectManager::add(std::unique_ptr<GameObject> object) {
    m_objects.emplace_back(std::move(object));
}

void GameObjectManager::updateAll(sf::Time dt) {
    for (auto& obj : m_objects)
        obj->update(dt);
}

void GameObjectManager::drawAll(sf::RenderTarget& target) const {
    for (auto& obj : m_objects)
        obj->draw(target);
}

void GameObjectManager::handleEventAll(const sf::Event& event) {
    for (auto& obj : m_objects)
        obj->handleEvent(event);
}

void GameObjectManager::clear() {
    m_objects.clear();
}

const std::vector<std::unique_ptr<GameObject>>&
GameObjectManager::getObjects() const {
    return m_objects;
}