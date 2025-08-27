#pragma once

#include "State.hpp"
#include "Button.hpp"
#include "Player.hpp"
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Time.hpp>
#include <filesystem>
#include <fstream>
#include <map>
#include <vector>
#include <memory>
#include <iostream>

class Game;

// 存档页面：点击槽位将当前游戏状态（鱼的数量与位置）写入 load/slotX.txt
class SaveState : public State {
public:
    SaveState(StateStack& stack, Game& game, Player& player);

    void onCreate() override;
    void onActivate() override;
    void onDeactivate() override;
    void onDestroy() override {}

    void handleEvent(const sf::Event& ev) override;
    void update(sf::Time) override {}
    void draw(sf::RenderTarget& target) const override;

private:
    void saveToSlot(int slot);
    void updateSlotStatus();

    Game& m_game;
    Player& m_player;

    sf::Font                             m_font;
    sf::Text                             m_text;        // 标题 “存档”
    sf::Sprite                           m_bgSprite;
    std::unique_ptr<Button>              m_backButton;  // 返回按钮

    std::vector<std::unique_ptr<Button>> m_slotButtons; // 四个槽位按钮
    std::vector<sf::Text>                m_slotStatusTexts; // 对应的状态文本
};
