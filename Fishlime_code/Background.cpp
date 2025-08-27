#include "Background.hpp"
#include <iostream>

Background::Background( const sf::Texture& texture,
                        const sf::Vector2u& windowSize)
    : mTexture(texture)        // 构造纹理
    , mSprite(texture)         // 直接用引用纹理
    , mWindowSize(windowSize)  // 初始化窗口尺寸
{
    // 在构造里设置缩放
    auto sz = mTexture.getSize();
    mSprite.setScale(
        { float(windowSize.x) / sz.x,
          float(windowSize.y) / sz.y }
    );
}

void Background::draw(sf::RenderTarget& target) const {
    target.draw(mSprite);
}