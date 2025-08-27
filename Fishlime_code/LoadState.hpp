#pragma once

#include "State.hpp"
#include "Button.hpp"
#include "Player.hpp"
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Mouse.hpp>
#include <filesystem>
#include <fstream>
#include <vector>
#include <iostream>
#include <memory>

class Game;

class LoadState : public State {
public:

    LoadState(StateStack& stack, Game& game, Player& player);

    void onCreate() override;
    void onActivate() override;
    void onDeactivate() override;
    void onDestroy() override {}

    void handleEvent(const sf::Event& event) override;
    void update(sf::Time deltaTime) override;
    void draw(sf::RenderTarget& target) const override;

private:
    void loadFromSlot(int slot);
    void updateSlotStatus();

    Game&                           m_game;
    Player&                         m_player;

    sf::Font                        m_font;
    sf::Text                        m_text;       // 标题 “读取存档”
    sf::Sprite                      m_bgSprite;
    std::unique_ptr<Button>         m_backButton;

    std::vector<std::unique_ptr<Button>>   m_slotButtons;   // 四个槽位按钮
    std::vector<sf::Text>                  m_slotStatusTexts; // 对应状态文本

};
