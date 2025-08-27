#pragma once

#include "State.hpp"
#include "Button.hpp"
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Window/Mouse.hpp>
#include <memory>

class Game;

class PauseState : public State {
public:
    PauseState(StateStack& stack, Game& game);

    void onCreate() override;
    void onActivate() override;
    void onDeactivate() override;
    void onDestroy() override {}

    void handleEvent(const sf::Event& ev) override;
    void update(sf::Time dt) override;
    void draw(sf::RenderTarget& rt) const override;

private:
    Game& m_game;
    sf::Font m_font;
    sf::Text  m_title;
    std::unique_ptr<Button> m_resumeButton;   // “继续游戏”
    std::unique_ptr<Button> m_exitButton;     // “退出到菜单”
};
