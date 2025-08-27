#pragma once

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

// 所有可由管理器批量管理的游戏对象都应继承自此类
class GameObject {
public:
    virtual ~GameObject() = default;

    // 每帧调用，处理自身逻辑
    virtual void update(sf::Time dt) = 0;

    // 在渲染阶段调用，负责绘制
    virtual void draw(sf::RenderTarget& target) const = 0;

    // 事件分发：默认空实现，不需要处理事件的子类可以不重写
    virtual void handleEvent(const sf::Event& event) {}
};
