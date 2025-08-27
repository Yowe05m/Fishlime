#pragma once

#include "State.hpp"
#include "Button.hpp"
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Mouse.hpp>

class Game; // 前向声明

class MenuState : public State {

public:
    MenuState(StateStack& stack, Game& game);

    // —— 生命周期钩子 —— 
    void onCreate() override;      // 第一次 push 时加载资源
    void onActivate() override;    // 每次回到前台时布局
    void onDeactivate() override;  // 暂出前台时（可选）
    void onDestroy() override {}   // 最后一次 pop 时（可选）

    // —— 核心接口 —— 
    void handleEvent(const sf::Event& ev) override;
    void update(sf::Time dt) override {}
    void draw(sf::RenderTarget& rt) const override;

private:
    Game&                                  m_game;
    sf::Font                               m_font;
    sf::Sprite                             m_bgSprite;   // 菜单背景
    sf::Sprite                             m_txtSprite;
    std::vector<std::unique_ptr<Button>>   m_buttons;
};