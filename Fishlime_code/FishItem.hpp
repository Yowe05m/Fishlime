#pragma once
#include "Item.hpp"
#include <string>

// 具体道具：鱼
class FishItem : public Item {
public:
    // weight: 回血量，rarity: 稀有度（测试用）
    FishItem(int weight, int rarity, std::string textureId)
        : m_weight(weight)
        , m_rarity(rarity)
        , m_textureId(std::move(textureId))
    {}

    std::string getName() const override {
        return "Fish " + std::to_string(m_weight);
    }

    void use(Player& p) override {
        // 这里你可以调用 Player 的回复血量接口，例如：
        // p.heal(m_weight);
    }

    // 提供贴图 ID 给 InventoryState
    const std::string & getTextureId() const { return m_textureId; }

private:
    int m_weight;
    int m_rarity;
    std::string m_textureId;
};
