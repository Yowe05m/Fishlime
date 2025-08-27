#pragma once

#include <vector>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

// Animation：封装一条动画序列的数据（帧、时长、缩放）
struct Animation {
    std::vector<const sf::Texture*> frames;   // 每帧的纹理指针
    sf::Time            frameDuration = sf::seconds(0.1f);  // 每帧时长
    sf::Vector2f        scale = { 1.f, 1.f };        // 整体缩放
};
