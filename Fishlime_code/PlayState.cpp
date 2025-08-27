#include "PlayState.hpp"
#include "Game.hpp"
#include "MenuState.hpp"
#include "PauseState.hpp"
#include "Background.hpp"
#include "Player.hpp" 
#include "InventoryState.hpp"
#include "Context.hpp"
#include "MovementBoundary.hpp"
#include "FishItem.hpp"
#include "AudioManager.hpp"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

PlayState::PlayState(StateStack& stack, Game& game, Player& player)
    : State(stack)
    , m_game(game)
    , m_player(player)
{}

void PlayState::onCreate() {
    // 1) 清空旧对象
    m_gameObjects.clear();

    // 2) 背景对象
    auto& bgTex = m_game.textures().get("background");
    auto bg = std::make_unique<Background>(bgTex, m_game.window().getSize());
    m_gameObjects.add(std::move(bg));
    
    auto winSize = m_game.window().getSize();

    // 3) 玩家贴图 + 动画

    if (!m_player.loadAnimation("Idle",
        "img/Waiting",
        10,
        0.1f,
        m_game.textures(),
        { 1.f, 1.f }))
    {
        std::cerr << "PlayState: Failed to load Idle animation\n";
    }

    if (!m_player.loadAnimation("Move",
        "img/Moving",
        10,
        0.1f,
        m_game.textures(),
        { 0.1f, 0.1f }))
    {
        std::cerr << "PlayState: Failed to load Move animation\n";
    }

    if (!m_player.loadAnimation("Start",
        "img/Start",      // 抛竿帧图放在 img/Start/1.png…10.png
        17,
        0.1f,
        m_game.textures(),
        { 1.f,1.f }))
    {
        std::cerr << "PlayState: Failed to load Start animation\n";
    }

    if (!m_player.loadAnimation("Fishing",
        "img/Fishing",    // 等待帧图放在 img/Fishing/1.png…10.png
        10,
        0.1f,
        m_game.textures(),
        { 1.f,1.f }))
    {
        std::cerr << "PlayState: Failed to load Fishing animation\n";
    }

    if (!m_player.loadAnimation("Finish",
        "img/Finish",     // 收杆帧图放在 img/Finish/1.png…10.png
        10,
        0.1f,
        m_game.textures(),
        { 1.f,1.f }))
    {
        std::cerr << "PlayState: Failed to load Finish animation\n";
    }

    if (!m_player.loadQTEFont("font/msyh.ttf", 48)) {
        std::cerr << "PlayState: QTE 字体加载失败\n";
    }

    // 默认播放 Idle
    m_player.playAnimation("Idle");

    // 设置移动范围多边形 —— 
    std::vector<sf::Vector2f> boundary = {
        {1920.f - 40.f, 640.f - 160.f}, {1540.f, 640.f - 160.f}, {1440.f, 695.f - 160.f}, {1440.f, 710.f  - 160.f}, {1275.f       , 775.f  - 160.f},
        {980.f        , 920.f - 160.f}, {1170.f, 950.f - 160.f}, {1170.f ,990.f - 160.f}, {920.f , 1080.f - 160.f}, {1920.f - 40.f, 1080.f - 160.f}
    };
    m_player.setMovementBoundary(boundary);

    // 4) “返回菜单”按钮
    auto& font = m_game.fonts().get("main_font");
    m_backBtn = std::make_unique<Button>(L"返回菜单", font);
    m_backBtn->setOnClick([this] {
        m_game.scheduleAction([this] {
            m_game.changeState<MenuState>();
        });
    });
    m_backBtn->setPosition(50.f, winSize.y - 80.f);

    // 5) 按钮：背包
    m_invBtn = std::make_unique<Button>(L"", font);
    m_invBtn->setSize(90.f, 90.f);
    m_invBtn->setTexture(
        m_game.textures().get("inv_btn")
    );
    m_invBtn->setPosition(m_game.window().getSize().x - 120.f, 30.f);
    m_invBtn->setOnClick([this] {
        m_game.scheduleAction([this] {
            // 推入背包界面，传递玩家引用
            m_game.pushState<InventoryState>(m_player);
        });
    });

    // 6) 存档按钮（新增）
    m_saveBtn = std::make_unique<Button>(L"", font);
    m_saveBtn->setSize(90.f, 90.f);
    m_saveBtn->setTexture(
        m_game.textures().get("load_img")
    );
    // 放在背包按钮左侧 120px
    m_saveBtn->setPosition(m_game.window().getSize().x - 240.f, 30.f);
    m_saveBtn->setOnClick([this] {
        m_game.scheduleAction([this] {
            m_game.pushState<SaveState>(m_player);
        });
    });

    // 7) 绑定钓鱼成功回调
    m_player.setFishCaughtCallback([this](const std::string& fishId) {
        m_game.scheduleAction([this, fishId] {
            m_game.showTransientMessage(
                sf::String(L"成功获得：") + fishId
            );
        });
    });
}

void PlayState::onActivate() {
    AudioManager::get().playMusic("play");
}

void PlayState::onDeactivate() {
    // 暂停前可记录位置等（此处留空）
}

void PlayState::handleEvent(const sf::Event& ev) {
    // ESC 弹出 PauseState
    if (ev.is<sf::Event::KeyPressed>())                              // 1) 检查是不是 KeyPressed
    {
        auto const* key = ev.getIf<sf::Event::KeyPressed>();          // 2) 拿到 KeyPressed 事件数据
        if (key->scancode == sf::Keyboard::Scancode::Escape)          // 3) 用 Scancode::Escape
        {
            m_game.scheduleAction([this] {
                m_game.pushState<PauseState>();
            });
        }
    }

    // 事件先给玩家
    m_player.handleEvent(ev);
    // 然后给场景对象
    m_gameObjects.handleEventAll(ev);

    // UI 事件
    auto& win = m_game.window();
    auto mousePos = win.mapPixelToCoords(sf::Mouse::getPosition(win));
    if (m_backBtn) m_backBtn->handleEvent(ev, mousePos);
    if (m_invBtn) m_invBtn->handleEvent(ev, mousePos);
    if (m_saveBtn)  m_saveBtn->handleEvent(ev, mousePos);
}

void PlayState::update(sf::Time dt) {
    m_player.update(dt);
    m_gameObjects.updateAll(dt);
}

void PlayState::draw(sf::RenderTarget& rt) const {
    m_gameObjects.drawAll(rt);
    m_player.draw(rt);
    if (m_backBtn) m_backBtn->draw(rt);
    if (m_invBtn) m_invBtn->draw(rt);
    if (m_saveBtn)  m_saveBtn->draw(rt);
}
