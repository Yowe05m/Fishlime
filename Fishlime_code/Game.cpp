#include "Game.hpp"
#include "MenuState.hpp"   // 初始界面
#include "AudioManager.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window/Event.hpp>
#include <functional>
#include <iostream>

Game::Game()
    : m_window(sf::VideoMode({ 1920u, 1080u }), L"Fishlime")
    , m_textureManager()
    , m_fontManager()
    , m_context{ &m_window, &m_textureManager, &m_fontManager }
    , m_states{ m_context }
    , m_player{ 200.f }
    , m_transientText(
        m_fontManager.load("main_font", "font/msyh.ttf"),
        sf::String(L""),
        30u
    )
{
    m_window.setFramerateLimit(60);

    // —— 在这里统一预加载“全局”资源 —— 
    // （以后所有模块都从管理器里拿，不再重复 loadFromFile）
    try {
        (void)m_context.fonts->get("main_font");
        m_context.textures->load("background", "img/Play/bg.png");
        // 如果有其他全局字体/图集，这里一并 preload
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to preload resources: " << e.what() << "\n";
    }

    try {
        auto& T = *m_context.textures;
        T.load("menu_bg", "img/Menu/Menu.png");
        T.load("menu_btn", "img/Menu/Menu_btn.png");
        // T.load("menu_btn", "img/Menu/Menu_btn.png");
        T.load("menu_txt", "img/Menu/Menu_text.png");
        T.load("inv_btn", "img/Play/Bag.png");
        // T.load("inv_btn_hover", "img/Play/Bag.png");
        T.load("inventory_bg", "img/Play/Bag_in.png");
        T.load("load_bg", "img/load/load_bg.png");
        T.load("load_img", "img/load/load_img.png");
        T.load("btnclose", "img/Play/Close.png");
        T.load("fish1", "img/Fish/Fish1.png");
        T.load("fish2", "img/Fish/Fish2.png");
        T.load("fish3", "img/Fish/Fish3.png");
        T.load("fish4", "img/Fish/Fish4.png");
        T.load("fish5", "img/Fish/Fish5.png");

        for (int i = 1; i <= 9; ++i) {
            std::string id = "help" + std::to_string(i);
            std::string path = "img/Help/help" + std::to_string(i) + ".jpg";
            T.load(id, path);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to preload UI resources: " << e.what() << "\n";
    }

    // —— 加载音效 —— 
    auto& am = AudioManager::get();
    // BGM
    am.loadMusic("menu", "Sounds/BGM/Menu.MP3");
    am.loadMusic("play", "Sounds/BGM/PlayState.MP3");
    // 通用音效
    am.loadSound("BtnHovered", "Sounds/SoundEffect/BtnHovered.mp3");
    am.loadSound("BtnPressed", "Sounds/SoundEffect/BtnPressed.mp3");
    am.loadSound("FinishFishing", "Sounds/SoundEffect/FinishFishing.MP3");
    am.loadSound("FishingFailed", "Sounds/SoundEffect/FishingFailed.MP3");
    am.loadSound("HelpStateBtn", "Sounds/SoundEffect/HelpStateBtn.MP3");
    am.loadSound("QTEStart", "Sounds/SoundEffect/QTEStart.wav");
    am.loadSound("QTESuccess", "Sounds/SoundEffect/QTESuccess.wav");
    am.loadSound("QTESuccess2", "Sounds/SoundEffect/QTESuccess2times.mp3");
    am.loadSound("StartFishing", "Sounds/SoundEffect/StartFishing.MP3");
    am.loadSound("StartFishing2", "Sounds/SoundEffect/StartFishing2.mp3");
    am.loadSound("Moving", "Sounds/SoundEffect/Moving.mp3");

    // —— 初始化临时消息文本 
    m_transientText.setFillColor(sf::Color::White);

    // 用新的状态接口推入第一个界面
    changeState<MenuState>();
}

void Game::showTransientMessage(const sf::String& msg, const sf::Color& color)
{
    // 先设置好颜色
    m_transientText.setFillColor(color);

    m_transientText.setString(msg);
    auto bounds = m_transientText.getLocalBounds();
    m_transientText.setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f });
    m_transientText.setPosition({
        m_window.getSize().x / 2.f,
        m_window.getSize().y * 0.25f
    });
    m_messageClock.restart();
    m_showMessage = true;
}

void Game::scheduleAction(std::function<void()> action) {
    m_scheduledAction = std::move(action);
}

void Game::run() {
    sf::Clock clk;
    while (m_window.isOpen()) {
        // 1) 事件轮询
        while (auto evOpt = m_window.pollEvent()) {
            auto& ev = *evOpt;
            if (ev.is<sf::Event::Closed>()) {
                m_window.close();
                continue;
            }
            // 数字键 1-9 控制音量
            if (const auto* kp = ev.getIf<sf::Event::KeyPressed>()) {
                switch (kp->code) {
                    // 总音量
                case sf::Keyboard::Key::Num1:
                    AudioManager::get().setMasterVolume(30.f);
                    std::cout << "总音量 切换为 30%\n";
                    showTransientMessage(L"总音量 切换为 30%", sf::Color::Black);
                    break;
                case sf::Keyboard::Key::Num2:
                    AudioManager::get().setMasterVolume(60.f);
                    std::cout << "总音量 切换为 60%\n";
                    showTransientMessage(L"总音量 切换为 60%", sf::Color::Black);
                    break;
                case sf::Keyboard::Key::Num3:
                    AudioManager::get().setMasterVolume(100.f);
                    std::cout << "总音量 切换为 100%\n";
                    showTransientMessage(L"总音量 切换为 100%", sf::Color::Black);
                    break;
                    // BGM 音量
                case sf::Keyboard::Key::Num4:
                    AudioManager::get().setMusicVolume(30.f);
                    std::cout << "BGM 音量 切换为 30%\n";
                    showTransientMessage(L"BGM 音量 切换为 30%", sf::Color::Black);
                    break;
                case sf::Keyboard::Key::Num5:
                    AudioManager::get().setMusicVolume(60.f);
                    std::cout << "BGM 音量 切换为 60%\n";
                    showTransientMessage(L"BGM 音量 切换为 60%", sf::Color::Black);
                    break;
                case sf::Keyboard::Key::Num6:
                    AudioManager::get().setMusicVolume(100.f);
                    std::cout << "BGM 音量 切换为 100%\n";
                    showTransientMessage(L"BGM 音量 切换为 100%", sf::Color::Black);
                    break;
                    // SFX 音量
                case sf::Keyboard::Key::Num7:
                    AudioManager::get().setSfxVolume(30.f);
                    std::cout << "音效音量 切换为 30%\n";
                    showTransientMessage(L"音效音量 切换为 30%", sf::Color::Black);
                    break;
                case sf::Keyboard::Key::Num8:
                    AudioManager::get().setSfxVolume(60.f);
                    std::cout << "音效音量 切换为 60%\n";
                    showTransientMessage(L"音效音量 切换为 60%", sf::Color::Black);
                    break;
                case sf::Keyboard::Key::Num9:
                    AudioManager::get().setSfxVolume(100.f);
                    std::cout << "音效音量 切换为 100%\n";
                    showTransientMessage(L"音效音量 切换为 100%", sf::Color::Black);
                    break;
                default: break;
                }
            }
            // 转发给状态机
            m_states.handleEvent(*evOpt);
        }

        // 2) 更新
        sf::Time dt = clk.restart();
        m_states.update(dt);

        // 3) 渲染
        m_window.clear();
        m_states.draw(m_window);

        // 绘制临时消息（显示 3 秒后自动隐藏）
        if (m_showMessage)
        {
            if (m_messageClock.getElapsedTime() < sf::seconds(3.f))
                m_window.draw(m_transientText);
            else
                m_showMessage = false;
        }

        m_window.display();

        // 4) 本帧结束后，执行一次延迟任务（如果有）
        if (m_scheduledAction) {
            auto action = std::move(m_scheduledAction);
            m_scheduledAction = nullptr;
            action();
        }

        // 5) 应用所有挂起的状态操作（push/pop/clear）
        m_states.applyPending();
    }
}
