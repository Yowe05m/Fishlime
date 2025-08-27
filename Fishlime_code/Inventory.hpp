#pragma once
#include <vector>
#include <memory>
#include "Item.hpp"
#include <SFML/Graphics/RenderTarget.hpp>

// 背包，只管理道具的添加/移除/访问
class Inventory {
public:
    void add(std::unique_ptr<Item> item) {
        m_items.push_back(std::move(item));
    }

    void remove(size_t index) {
        if (index < m_items.size())
            m_items.erase(m_items.begin() + index);
    }

    Item* get(size_t index) {
        if (index < m_items.size())
            return m_items[index].get();
        return nullptr;
    }

    size_t size() const { return m_items.size(); }

    // 占位：具体绘制由 InventoryState 负责
    void draw(sf::RenderTarget&) const {}

private:
    std::vector<std::unique_ptr<Item>> m_items;
};
