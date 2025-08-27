#pragma once

#include <vector>
#include <memory>
#include "GameObject.hpp"

class GameObjectManager {
public:
    // 添加新对象（接受 any 继承 GameObject 的类）
    void add(std::unique_ptr<GameObject> object);

    // 每帧更新所有对象
    void updateAll(sf::Time dt);

    // 渲染所有对象
    void drawAll(sf::RenderTarget& target) const;

    // 把事件分发给每个对象
    void handleEventAll(const sf::Event& event);

    // 清空并销毁所有对象
    void clear();

    // 提供只读访问内部对象列表  
    const std::vector<std::unique_ptr<GameObject>>& getObjects() const;

private:
    std::vector<std::unique_ptr<GameObject>> m_objects;
};
