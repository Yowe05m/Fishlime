#pragma once

#include <SFML/Graphics.hpp>
#include "GameObject.hpp"
#include <string>

class Background : public GameObject {
public:
    Background(const sf::Texture& texture, const sf::Vector2u& windowSize);

    void update(sf::Time) override {}  // 无需每帧逻辑，空实现

    // 在指定的 RenderTarget 上绘制
    void draw(sf::RenderTarget& target) const;

private:
    sf::Texture    mTexture;
    sf::Sprite     mSprite;
    sf::Vector2u   mWindowSize;
};
