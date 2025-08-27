#include "InventoryState.hpp"
#include "Game.hpp"
#include "FishItem.hpp" 
#include "AudioManager.hpp"
#include <iostream>
#include <SFML/Window/Mouse.hpp>
#include <map>
#include <string> 

InventoryState::InventoryState(StateStack& stack, Game& game, Player& player)
    : State(stack)
    , m_game(game)
    , m_player(player)
    , m_title(m_font)
    , m_bgSprite{ game.textures().get("inventory_bg") }
{
}

void InventoryState::onCreate() {
    // 1) 字体
    if (!m_font.openFromFile("font/msyh.ttf"))
        std::cerr << "PauseState: 字体加载失败\n";

    // 2) 标题
    m_title.setString(L"背包");
    m_title.setCharacterSize(40);

    // 3) 关闭按钮
    m_closeBtn = std::make_unique<Button>(L"", m_font);
    m_closeBtn->setSize(90.f, 90.f);
    m_closeBtn->setTexture(
        m_game.textures().get("btnclose")
    );
    m_closeBtn->setPosition(m_game.window().getSize().x - 120.f, 30.f);
    m_closeBtn->setOnClick([this] {
        m_game.scheduleAction([this] {
            m_game.popState();
        });
    });

    // 4) 鱼的贴图 ID 列表
    m_fishIds = { "fish1", "fish2", "fish3", "fish4", "fish5" };

    // 5) 根据 ID 构造 Sprite 和 数量 Text
    for (auto& id : m_fishIds) {
        // 图标
        sf::Sprite spr{ m_game.textures().get(id) };
        m_fishSprites.push_back(spr);

        // 数量文本
        sf::Text cnt{ m_font };
        cnt.setCharacterSize(24);
        cnt.setFillColor(sf::Color::White);
        m_fishCountTexts.push_back(cnt);
    }
}

void InventoryState::onActivate() {
    AudioManager::get().playMusic("play");

    // 背景缩放铺满
    auto winSize = m_game.window().getSize();
    auto texSize = m_bgSprite.getTexture().getSize();
    m_bgSprite.setScale({
        float(winSize.x) / texSize.x,
        float(winSize.y) / texSize.y
        });

    // 标题居中
    auto tb = m_title.getLocalBounds();
    m_title.setOrigin(tb.size * 0.5f);
    m_title.setPosition({ winSize.x / 2.f, 50.f });

    // —— 统计每种鱼的数量 —— 
    std::map<std::string, int> counts;
    for (auto& id : m_fishIds) counts[id] = 0;
    // 遍历背包里的所有 Item
    for (size_t i = 0; i < m_player.inventory().size(); ++i) {
        if (auto* fish = dynamic_cast<FishItem*>(m_player.inventory().get(i))) {
            counts[fish->getTextureId()]++;
        }
    }

    // —— 布局图标和右下角的数量 —— 
    float paddingx = 525.f;
    float paddingy = 405.f;
    float xStart = 350.f;
    float yStart = 340.f;
    for (size_t i = 0; i < m_fishIds.size(); ++i) {
        // 1) 图标
        auto& spr = m_fishSprites[i];
        auto fishlb = spr.getLocalBounds();    // sf::FloatRect { position, size }
        spr.setOrigin({ fishlb.size.x * 0.5f,
                        fishlb.size.y * 0.5f });

        float sx = 400.f / fishlb.size.x;
        float sy = 400.f / fishlb.size.y;
        spr.setScale({ sx, sy });

        spr.setPosition({ xStart + paddingx * (i % 3), yStart + paddingy * (i / 3) });

        // 2) 数量文本（右下角对齐）
        auto& txt = m_fishCountTexts[i];
        txt.setString(std::to_string(counts[m_fishIds[i]]));
        auto lb = txt.getLocalBounds();
        // sprite 的 global bounds
        auto gb = spr.getGlobalBounds();
        txt.setPosition({
            gb.position.x + gb.size.x - lb.size.x,
            gb.position.y + gb.size.y - lb.size.y - 30.f
        });
    }
}

void InventoryState::onDeactivate() {
    // 可选：比如暂停动画之类
}

void InventoryState::handleEvent(const sf::Event& ev) {
    auto& win = m_game.window();
    auto mousePos = win.mapPixelToCoords(sf::Mouse::getPosition(win));
    m_closeBtn->handleEvent(ev, mousePos);

    // ESC 快捷键 —— 同“继续游戏”
    if (const auto* key = ev.getIf<sf::Event::KeyPressed>(); key && key->code == sf::Keyboard::Key::Escape) {
        m_game.scheduleAction([this] {
            m_game.popState();  // 弹出 PauseState
        });
    }
}

void InventoryState::draw(sf::RenderTarget& target) const {
    // 1) 背景
    target.draw(m_bgSprite);

    // 2) 标题
    target.draw(m_title);

    // 3) 关闭按钮
    m_closeBtn->draw(target);

    // 4) 五种鱼的图标 + 数量
    for (auto& spr : m_fishSprites)
        target.draw(spr);
    for (auto& txt : m_fishCountTexts)
        target.draw(txt);
}
