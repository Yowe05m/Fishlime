#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "StateStack.hpp"
#include "Context.hpp"
#include "ResourceManager.hpp"
#include "Player.hpp"
#include <functional>

class Game {
public:
    Game();
    void run();

    // —— 临时消息接口
    // 在窗口中间偏上显示一条消息，自动浮现 3 秒
    void showTransientMessage(const sf::String& msg, const sf::Color& color = sf::Color::White);

    // —— 在本帧事件/渲染都跑完后再执行 action
    void scheduleAction(std::function<void()> action);

    // —— 状态访问入口
    sf::RenderWindow&       window()        { return m_window; }
    const sf::RenderWindow& window() const  { return m_window; }

    // —— 资源管理器访问接口
    ResourceManager<sf::Texture>& textures() { return m_textureManager; }
    ResourceManager<sf::Font>& fonts() { return m_fontManager; }

    // —— 状态切换接口
    // 完全清空旧栈并推入新状态（相当于“重置游戏”）
    template <class StateT, class... Args>
    void changeState(Args&&... args) {
        m_states.clearStates();
        m_states.pushState<StateT>(
            *this,
            std::forward<Args>(args)...
        );
    }

    // 压入新状态（保留旧状态数据）
    template <class StateT, class... Args>
    void pushState(Args&&... args) {
        m_states.pushState<StateT>(
            *this,
            std::forward<Args>(args)...
        );
    }

    // 弹出当前状态，恢复上一个状态
    void popState() {
        m_states.popState();
    }

    // —— 全局 Player 实例和访问接口
    Player& getPlayer() { return m_player; }

private:
    sf::RenderWindow        m_window;

    ResourceManager<sf::Texture> m_textureManager; // 纹理管理器
    ResourceManager<sf::Font>    m_fontManager; // 字体管理器

    Context                 m_context{ &m_window, &m_textureManager, &m_fontManager };
    
    StateStack              m_states;

    Player                  m_player{ 200.f };

    // 存放单次的延迟回调
    std::function<void()>   m_scheduledAction;

    // 音量切换显示 临时消息 
    sf::Text    m_transientText;
    sf::Clock   m_messageClock;
    bool        m_showMessage = false;
};