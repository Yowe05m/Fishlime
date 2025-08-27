#include "MovementBoundary.hpp"

MovementBoundary::MovementBoundary(const std::vector<sf::Vector2f>& vertices)
    : m_vertices(vertices)
{
}

void MovementBoundary::setVertices(const std::vector<sf::Vector2f>& vertices) {
    m_vertices = vertices;
}

bool MovementBoundary::contains(const sf::Vector2f& point) const {
    bool inside = false;
    size_t n = m_vertices.size();
    for (size_t i = 0, j = n - 1; i < n; j = i++) {
        const auto& vi = m_vertices[i];
        const auto& vj = m_vertices[j];
        // 检测射线和边的相交
        bool intersect = ((vi.y > point.y) != (vj.y > point.y))
            && (point.x < (vj.x - vi.x) * (point.y - vi.y) / (vj.y - vi.y) + vi.x);
        if (intersect)
            inside = !inside;
    }
    return inside;
}

