#include "MenuState.hpp"
#include "Game.hpp"
#include "PlayState.hpp"
#include "HelpState.hpp"
#include "LoadState.hpp"
#include "Player.hpp"
#include "AudioManager.hpp"
#include <iostream>
#include <SFML/Window/Mouse.hpp>

MenuState::MenuState(StateStack& stack, Game& game)
    : State(stack)
    , m_game(game)
    , m_bgSprite{ game.textures().get("menu_bg") } 
    , m_txtSprite{ game.textures().get("menu_txt") }
{}

void MenuState::onCreate() {
    // 1) 加载字体
    if (!m_font.openFromFile("font/msyh.ttf"))
        std::cerr << "MenuState: 字体加载失败\n";

    // 2) 统一按钮目标尺寸
    const float btnWidth = 228.f;
    const float btnHeight = 100.f;

    // 3) “开始游戏”按钮
    {
        auto btn = std::make_unique<Button>(L"开始游戏", m_font);
        btn->setSize(btnWidth, btnHeight);  
        btn->setTextColor(sf::Color::Black);
        btn->setTexture(
            m_game.textures().get("menu_btn")
        );
        btn->setOnClick([this] {
            m_game.scheduleAction([this] {

                // —— 在这里重置全局玩家到新游戏状态 —— 
                auto& player = m_game.getPlayer();

                // 1) 清空背包
                while (player.inventory().size() > 0)
                    player.inventory().remove(0);

                // 2) 恢复默认位置（和之前 PlayState 里的一致）
                player.setPosition({ 1440.f, 940.f - 160.f });

                m_game.changeState<PlayState>(player);
            });
        });
        m_buttons.push_back(std::move(btn));
    }
    // 4) “帮助”按钮
    {
        auto btn = std::make_unique<Button>(L"帮助", m_font);
        btn->setSize(btnWidth, btnHeight);  
        btn->setTextColor(sf::Color::Black);
        btn->setTexture(
            m_game.textures().get("menu_btn")
        );
        btn->setOnClick([this] {
            m_game.scheduleAction([this] {
                m_game.pushState<HelpState>();
            });
        });
        m_buttons.push_back(std::move(btn));
    }
    // 5) “读取存档”按钮
    {
        auto btn = std::make_unique<Button>(L"读取存档", m_font);
        btn->setSize(btnWidth, btnHeight);  
        btn->setTextColor(sf::Color::Black);
        btn->setTexture(
            m_game.textures().get("menu_btn")
        );
        btn->setOnClick([this] {
            m_game.scheduleAction([this] {
                m_game.pushState<LoadState>( m_game.getPlayer() );
            });
        });
        m_buttons.push_back(std::move(btn));
    }
    // 6) “退出”按钮
    {
        auto btn = std::make_unique<Button>(L"退出", m_font);
        btn->setSize(btnWidth, btnHeight);  
        btn->setTextColor(sf::Color::Black);
        btn->setTexture(
            m_game.textures().get("menu_btn")
        );
        btn->setOnClick([this] {
            m_game.scheduleAction([this] {
                m_game.window().close();
            });
        });
        m_buttons.push_back(std::move(btn));
    }
}

void MenuState::onActivate() {
    AudioManager::get().playMusic("menu");

    // 1) 背景填满
    auto winSize = m_game.window().getSize();
    auto texSize = m_bgSprite.getTexture().getSize();
    m_bgSprite.setScale({
        float(winSize.x) / texSize.x,
        float(winSize.y) / texSize.y
    });

    // 2) 游戏标题
    auto txtSize = m_txtSprite.getTexture().getSize();
    m_txtSprite.setOrigin({ txtSize.x / 2.f, txtSize.y / 2.f });
    m_txtSprite.setPosition({
        1535.f, 168.f }
    );

    // 3) 按钮靠右纵向排列
    float x = 1421.f;
    float spacing = 36.f;
    float startY = 388.f;
    for (size_t i = 0; i < m_buttons.size(); ++i) {
        float y = 488.f + i * (100.f + spacing);
        m_buttons[i]->setPosition(x, startY + i * (100.f + 36.f));
    }
}

void MenuState::onDeactivate() {
    // 例如暂停动画、停止音乐等（此处留空）
}

void MenuState::handleEvent(const sf::Event& ev) {
    auto& win = m_game.window();
    auto mousePos = win.mapPixelToCoords(sf::Mouse::getPosition(win));
    for (auto& btn : m_buttons)
        btn->handleEvent(ev, mousePos);
}

void MenuState::draw(sf::RenderTarget& rt) const {
    // 先画背景
    rt.draw(m_bgSprite);
    // 再画标题
    rt.draw(m_txtSprite);
    // 再画按钮
    for (auto& btn : m_buttons)
        btn->draw(rt);
}