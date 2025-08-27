#pragma once

#include "State.hpp"
#include "Button.hpp"
#include "Player.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class Game;

// 背包界面：覆盖在游戏画面之上，由 StateStack 负责半透明处理
class InventoryState : public State {
public:
    InventoryState(StateStack& stack, Game& game, Player& player);

    void onCreate() override;
    void onActivate() override;
    void onDeactivate() override;
    void onDestroy() override {}

    void handleEvent(const sf::Event& ev) override;
    void update(sf::Time dt) override {}
    void draw(sf::RenderTarget& target) const override;

private:
    Game& m_game;
    Player& m_player;
    sf::Font                    m_font;
    sf::Text                    m_title;
    std::unique_ptr<Button>     m_closeBtn;
    std::vector<sf::Text>       m_itemTexts;

    sf::Sprite                  m_bgSprite;

    // 五种鱼的 ID、Sprite 和数量文本
    std::vector<std::string>     m_fishIds;        // {"fish1", ..., "fish5"}
    std::vector<sf::Sprite>      m_fishSprites;    // 对应图标
    std::vector<sf::Text>        m_fishCountTexts; // 右下角的数量
};
