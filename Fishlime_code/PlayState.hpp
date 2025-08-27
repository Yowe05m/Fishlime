#pragma once
#include "State.hpp"
#include "Button.hpp"
#include "GameObjectManager.hpp"
#include "Player.hpp"          
#include "FishItem.hpp"        
#include "InventoryState.hpp"  
#include "SaveState.hpp" 
#include <memory>

class Game;

class PlayState : public State {
public:
    PlayState(StateStack& stack, Game& game, Player& player);

    void onCreate() override;      // 第一次加载纹理、字体、按钮等
    void onActivate() override;    // 每次恢复时布局玩家&按钮
    void onDeactivate() override;  // 暂停前台时可选
    void onDestroy() override {}   // 最后销毁时可选

    void handleEvent(const sf::Event& ev) override;
    void update(sf::Time dt) override;
    void draw(sf::RenderTarget& rt) const override;

private:

    Game&                       m_game;
    Player&                     m_player;
    // 场景元素 -----------------------------------------------
    GameObjectManager           m_gameObjects; // 对象管理器

    // UI – 返回菜单按钮 ------------------------------------
    std::unique_ptr<Button>     m_backBtn;

    // “背包”按钮
    std::unique_ptr<Button>     m_invBtn; 

    // 存档
    std::unique_ptr<Button>     m_saveBtn;
};