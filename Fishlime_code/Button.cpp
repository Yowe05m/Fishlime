#include "Button.hpp"
#include "AudioManager.hpp"

Button::Button(const sf::String& label, const sf::Font& font, unsigned int fontSize)
    : m_text(font, label, fontSize)   // 直接把 Unicode 字符串给 Text
    , m_buttonText(label.toAnsiString())
{
    m_defaultColor = sf::Color(70, 130, 180);// 默认蓝色
    m_rect.setFillColor(m_defaultColor);

    // 初始化 hover 蒙版：半透明黑
    m_hoverMask.setFillColor(sf::Color(0, 0, 0, 72));

    m_text.setFillColor(sf::Color::White);

    // 预设一个“悬浮色”，也可以后续用接口覆盖
    m_hoverColor = sf::Color(100, 149, 237);

    // 设置文本居中
    sf::FloatRect textBounds = m_text.getLocalBounds();
    m_text.setOrigin({ textBounds.position.x + textBounds.size.x / 2.0f,
                       textBounds.position.y + textBounds.size.y / 2.0f });

    // 初始大小
    setSize(300, 60);

    // 默认音效
    hoverSound = "BtnHovered";
    pressSound = "BtnPressed";
}

void Button::setPosition(float x, float y) {
    sf::Vector2f pos{ x,y };
    m_rect.setPosition(pos);
    m_text.setPosition({ x + m_rect.getSize().x / 2, y + m_rect.getSize().y / 2 });

    if (m_sprite.has_value())
        m_sprite->setPosition({ x, y });
    m_hoverMask.setPosition({ x, y });
}

void Button::setSize(float width, float height) {
    m_rect.setSize(sf::Vector2f(width, height));
    m_hoverMask.setSize({ width, height });      // 同步蒙版大小
    // 重新设置位置，使文本和贴图都跟着更新
    auto pos = m_rect.getPosition();
    setPosition(pos.x, pos.y);
}

// 设置原点只适用于存档、读档页面
void Button::setOrigin(float x, float y) {
    // 背景的原点
    m_rect.setOrigin(sf::Vector2f{ x, y });

    // 蒙版的原点
    m_hoverMask.setOrigin({ x, y });

    // 文字的原点
    m_text.setOrigin(sf::Vector2f{ x, y });
}

void Button::setFillColor(const sf::Color& color) {
    m_rect.setFillColor(color);
}

void Button::setTextColor(const sf::Color& color) {
    m_text.setFillColor(color);
}

void Button::setDefaultFillColor(const sf::Color& c) {
    m_defaultColor = c;
    if (!m_isHovered)    // 只有在非 hover 状态下才立即生效
        m_rect.setFillColor(m_defaultColor);
}

void Button::setHoverFillColor(const sf::Color& c) {
    m_hoverColor = c;
    if (m_isHovered)     // 如果此时正悬浮 -> 立即应用
        m_rect.setFillColor(m_hoverColor);
}


void Button::setTexture(const sf::Texture& texture) {
    // 构造或更新贴图
    if (!m_sprite.has_value())
        m_sprite.emplace(texture);
    else
        m_sprite->setTexture(texture, true);

    // 按钮大小与贴图等比缩放
    auto texSize = m_sprite->getTexture().getSize();
    auto rectSize = m_rect.getSize();
    m_sprite->setScale({
        rectSize.x / float(texSize.x),
        rectSize.y / float(texSize.y) }
    );
    m_sprite->setPosition(m_rect.getPosition());
}

void Button::setTextures(const sf::Texture& normal, const sf::Texture& hover) {
    m_normalTexture = &normal;
    m_hoverTexture = &hover;
    setTexture(normal);
}

bool Button::contains(const sf::Vector2f& point) const {
    return m_rect.getGlobalBounds().contains(point);
}

void Button::draw(sf::RenderTarget& target) const {
    // 1. 先画矩形或贴图
    if (m_sprite.has_value())
        target.draw(*m_sprite);
    else
        target.draw(m_rect);

    // 2. 如果 hover，就加一层半透明蒙版
    if (m_isHovered)
        target.draw(m_hoverMask);

    // 3. 最后画文字
    target.draw(m_text);
}

void Button::setOnClick(std::function<void()> callback) {
    m_onClick = callback;
}

void Button::handleEvent(const sf::Event& event, const sf::Vector2f& mousePos) {
    bool containsNow = contains(mousePos);

    // —— 悬浮处理 —— 
    if (containsNow && !m_isHovered) {
        m_isHovered = true;
        m_rect.setFillColor(m_hoverColor);
        // 播放悬浮音效
        AudioManager::get().playSound(hoverSound);
    }
    else if (!containsNow && m_isHovered) {
        m_isHovered = false;
        m_rect.setFillColor(m_defaultColor);
    }

    // —— 点击处理 —— 
    if (m_isHovered) {
        if (const auto* mp = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (mp->button == sf::Mouse::Button::Left) {
                // 播放点击音效
                AudioManager::get().playSound(pressSound);
                if (m_onClick) m_onClick();
            }
        }
    }
}

const std::string& Button::getText() const {
    return m_buttonText;
}