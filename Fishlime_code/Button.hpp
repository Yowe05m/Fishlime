#pragma once
#include <optional>
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

// 前置声明
class AudioManager;

class Button {
public:
    Button(const sf::String& label, const sf::Font& font, unsigned int fontSize = 30);

    void setPosition(float x, float y);
    void setSize(float width, float height);
    void setOrigin(float x, float y);

    void setFillColor(const sf::Color& color);
    // 设置“默认”背景色（会在 unhover 时还原）
    void setDefaultFillColor(const sf::Color& color);
    // 设置“悬浮”背景色（hover 时使用）
    void setHoverFillColor(const sf::Color& color);
    void setTextColor(const sf::Color& color);

    // 单一贴图
    void setTexture(const sf::Texture& texture);
    // 正常态 + 悬停态贴图
    void setTextures(const sf::Texture& normal, const sf::Texture& hover);

    bool contains(const sf::Vector2f& point) const;
    void draw(sf::RenderTarget& target) const;

    // 点击回调
    void setOnClick(std::function<void()> callback);

    // 音效名称  
    void setHoverSound(const std::string& name) { hoverSound = name; }
    void setPressSound(const std::string& name) { pressSound = name; }

    void handleEvent(const sf::Event& event, const sf::Vector2f& mousePos);

    const std::string& getText() const;

private:
    sf::RectangleShape       m_rect;
    sf::Text                 m_text;

    std::function<void()>    m_onClick;
    bool                     m_isHovered = false;
    std::string              m_buttonText; // 存储按钮文本

    sf::Color m_defaultColor;   // 存你调用 setDefaultFillColor 时的值
    sf::Color m_hoverColor;     // 存你调用 setHoverFillColor 时的值

    // 音效字段
    std::string hoverSound;
    std::string pressSound;

    // 贴图部分
    std::optional<sf::Sprite>   m_sprite;
    const sf::Texture*          m_normalTexture = nullptr;
    const sf::Texture*          m_hoverTexture = nullptr;
    // hover 蒙版
    sf::RectangleShape          m_hoverMask;
};