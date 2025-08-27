#pragma once

#include "State.hpp"
#include "Button.hpp"
#include "Game.hpp"
#include <vector>
#include <filesystem>
#include <SFML/Graphics.hpp>
#include <memory>

class Game;

class HelpState : public State {
public:
    HelpState(StateStack & stack, Game & game);
    
    void onCreate() override;
    void onActivate() override;
    void onDestroy() override;
    void update(sf::Time dt) override;
    void draw(sf::RenderTarget& target) const override;
    void handleEvent(const sf::Event& event) override;


private:
    Game&                        m_game;
    std::vector<std::string>     m_imageIds;

    // 载入当前索引对应的贴图，并更新精灵、缩放、文字
    void loadCurrentTexture();
    // 根据当前索引，更新按钮的显示/隐藏，以及底部的 "xx/yy" 文本
    void updateControls();

    sf::Sprite                    m_sprite;         // 显示当前图片
    sf::Font                      m_font;           // 字体
    sf::Text                      m_counterText;    // “xx/yy” 字符串

    std::unique_ptr<Button>       m_prevButton;     // “上一张” 按钮
    std::unique_ptr<Button>       m_nextButton;     // “下一张” 按钮
    std::unique_ptr<Button>       m_backButton;

    std::size_t                   m_currentIndex{0}; // 当前图片索引 (0…m_textures.size()-1)
};
