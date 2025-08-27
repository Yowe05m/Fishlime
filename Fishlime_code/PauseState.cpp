#include "PauseState.hpp"
#include "Game.hpp"
#include "MenuState.hpp"
#include "AudioManager.hpp"
#include <iostream>
#include <SFML/Window/Mouse.hpp>

PauseState::PauseState(StateStack& stack, Game& game)
    : State(stack)
    , m_game(game)
    , m_title(m_font)
{}

void PauseState::onCreate() {
    if (!m_font.openFromFile("font/msyh.ttf"))
        std::cerr << "PauseState: 字体加载失败\n";

    m_title.setFont(m_font);
    m_title.setString(L"游戏已暂停");
    m_title.setCharacterSize(60);

    m_resumeButton = std::make_unique<Button>(L"继续游戏", m_font);
    m_resumeButton->setOnClick([this] {
        m_game.scheduleAction([this] {
            m_game.popState();
            });
        });

    m_exitButton = std::make_unique<Button>(L"退出到菜单", m_font);
    m_exitButton->setOnClick([this] {
        m_game.scheduleAction([this] {
            m_game.changeState<MenuState>();
            });
        });
}

void PauseState::onActivate() {
    AudioManager::get().playMusic("play");

    auto winSize = m_game.window().getSize();
    auto tb = m_title.getLocalBounds();
    m_title.setOrigin({ tb.size.x / 2.f, tb.size.y / 2.f });
    m_title.setPosition({ winSize.x / 2.f, winSize.y / 3.f });

    float x = winSize.x / 2.f - 150.f;
    float y = winSize.y / 2.f;
    m_resumeButton->setPosition(x, y);
    m_exitButton->setPosition(x, y + 100.f);
}

void PauseState::onDeactivate() {
    // 可选
}

void PauseState::handleEvent(const sf::Event& ev) {
    auto& window = m_game.window();
    auto mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    // 处理按钮点击
    m_resumeButton->handleEvent(ev, mousePos);
    m_exitButton->handleEvent(ev, mousePos);

    // ESC 快捷键 —— 同“继续游戏”
    if (const auto* key = ev.getIf<sf::Event::KeyPressed>(); key && key->code == sf::Keyboard::Key::Escape) {
        m_game.scheduleAction([this] {
            m_game.popState();  // 弹出 PauseState
        });
    }
}

void PauseState::update(sf::Time) {
    // 暂停界面不需要额外更新
}

void PauseState::draw(sf::RenderTarget& rt) const {
    rt.draw(m_title);
    m_resumeButton->draw(rt);
    m_exitButton->draw(rt);
}
