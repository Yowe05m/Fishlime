#pragma once
#include <vector>
#include <SFML/System/Vector2.hpp>

// MovementBoundary：存储多边形顶点并进行“点在多边形内”检测
class MovementBoundary {
public:
    MovementBoundary() = default;
    explicit MovementBoundary(const std::vector<sf::Vector2f>& vertices);

    // 更新顶点
    void setVertices(const std::vector<sf::Vector2f>& vertices);

    // 射线法检测点是否在多边形内部
    bool contains(const sf::Vector2f& point) const;

private:
    std::vector<sf::Vector2f> m_vertices;
};
