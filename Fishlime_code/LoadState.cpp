#include "LoadState.hpp"
#include "Game.hpp"
#include "MenuState.hpp"
#include "PlayState.hpp"
#include "FishItem.hpp"
#include "AudioManager.hpp"
#include <SFML/Window/Mouse.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;

LoadState::LoadState(StateStack& stack, Game& game, Player& player)
    : State(stack)
    , m_game(game)
    , m_player(player)
    , m_text(m_font)
    , m_bgSprite{ game.textures().get("load_bg") }
{
}

void LoadState::onCreate() {
    // 字体与标题
    if (!m_font.openFromFile("font/msyh.ttf"))
        std::cerr << "LoadState: 字体加载失败\n";

    m_text.setFont(m_font);
    m_text.setString(L"");
    m_text.setCharacterSize(60);

    // 返回按钮
    m_backButton = std::make_unique<Button>(L"返回菜单", m_font);
    m_backButton->setOnClick([this] {
        m_game.scheduleAction([this] {
            m_game.changeState<MenuState>();
        });
    });
    m_backButton->setPosition(50.f, m_game.window().getSize().y - 80.f);

    // 四个槽位按钮 + 状态文本
    for (int i = 1; i <= 4; ++i) {
        // 按钮
        auto btn = std::make_unique<Button>(L"", m_font);
        // 隐藏文字，只用背景高亮
        btn->setTextColor(sf::Color::Transparent);
        // 尺寸 900×125，原点居中
        btn->setSize(900.f, 125.f);
        btn->setOrigin(450.f, 62.5f);
        // 默认全透明，hover 半透明白
        btn->setDefaultFillColor(sf::Color::Transparent);
        btn->setHoverFillColor(sf::Color(255, 255, 255, 72));

        int slot = i;
        btn->setOnClick([this, slot] {
            fs::path file = fs::path("load") / ("slot" + std::to_string(slot) + ".txt");
            if (fs::exists(file)) {
                loadFromSlot(slot);
                m_game.scheduleAction([this] { m_game.changeState<PlayState>( m_game.getPlayer() ); });
            }
        });
        m_slotButtons.push_back(std::move(btn));

        // 状态文本
        sf::Text status(m_font);
        status.setCharacterSize(30);
        status.setFillColor(sf::Color::Black);
        m_slotStatusTexts.push_back(status);
    }
}

void LoadState::onActivate() {
    AudioManager::get().playMusic("menu");

    // —— 背景填满
    auto winSize = m_game.window().getSize();
    auto texSize = m_bgSprite.getTexture().getSize();
    m_bgSprite.setScale({
        float(winSize.x) / texSize.x,
        float(winSize.y) / texSize.y
    });

    // 标题布局
    auto bounds = m_text.getLocalBounds();
    m_text.setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f });
    m_text.setPosition({ winSize.x / 2.f, 100.f });

    // —— 更新并布局槽位状态
    updateSlotStatus(); 

    float centerX = winSize.x / 2.f + 8.f;
    float yStart = 225.f;
    float yGap = 216.f;
    for (size_t i = 0; i < m_slotButtons.size(); ++i) {
        float x = centerX;
        float y = yStart + static_cast<float>(i) * yGap;
        // 最后一项稍微下移 10 像素
        if (i == m_slotButtons.size() - 1) {
            y += 8.f;
        }
        m_slotButtons[i]->setPosition(x, y);

        m_slotStatusTexts[i].setPosition({ x - 300.f, y - 12.f });
    }
}

void LoadState::onDeactivate() {
    // 可选
}

void LoadState::handleEvent(const sf::Event& event) {
    auto& window = m_game.window();
    auto mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    
    m_backButton->handleEvent(event, mousePos);
    for (auto& btn : m_slotButtons)
        btn->handleEvent(event, mousePos);


    // Esc 快捷键返回
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Escape) {
            m_game.scheduleAction([this] {
                m_game.popState();
            });
        }
    }
}

void LoadState::update(sf::Time) {}

void LoadState::draw(sf::RenderTarget& target) const {
    // —— 先画背景
    target.draw(m_bgSprite);
    // —— 再画标题
    target.draw(m_text);
    // —— 然后返回按钮
    m_backButton->draw(target);
    // —— 然后是四个槽位按钮
    for (auto& btn : m_slotButtons)
        btn->draw(target);
    // —— 最后画状态文本
    for (auto& status : m_slotStatusTexts)
        target.draw(status);
}

void LoadState::updateSlotStatus() {
    std::vector<std::string> fishIds = { "fish1","fish2","fish3","fish4","fish5" };
    for (size_t i = 0; i < 4; ++i) {
        int slot = int(i) + 1;
        fs::path file = fs::path("load") / ("slot" + std::to_string(slot) + ".txt");
        std::wstring text;
        std::wstring prefix = L"槽位 " + std::to_wstring(slot) + L"    ";
        if (fs::exists(file)) {
            std::ifstream ifs(file.string());
            std::string line;
            std::getline(ifs, line);  // 跳过位置
            std::getline(ifs, line);  // 读取数量
            std::istringstream iss(line);
            int cnt, total = 0;
            while (iss >> cnt) total += cnt;
            text = prefix + L"鱼：" + std::to_wstring(total);
        }
        else {
            text = prefix + L"空";
        }
        m_slotStatusTexts[i].setString(text);
    }
}

void LoadState::loadFromSlot(int slot) {
    fs::path file = fs::path("load") / ("slot" + std::to_string(slot) + ".txt");
    std::ifstream ifs(file.string());

    // 1) 设置玩家位置
    std::string line;
    std::getline(ifs, line);
    std::istringstream issPos(line);
    float x, y;
    issPos >> x >> y;

    m_player.setPosition({ x, y });

    // 2) 清空背包（无 clear() 时可循环 remove）
    while (m_player.inventory().size() > 0)
        m_player.inventory().remove(0);

    // 3) 按存档数量重建鱼道具
    std::getline(ifs, line);
    std::istringstream issCnt(line);
    std::vector<std::string> fishIds = { "fish1","fish2","fish3","fish4","fish5" };
    int cnt;
    for (auto& id : fishIds) {
        if (!(issCnt >> cnt)) break;
        for (int k = 0; k < cnt; ++k) {
            int val = id.back() - '0';
            m_player.inventory().add(std::make_unique<FishItem>(val, val, id));
        }
    }
}