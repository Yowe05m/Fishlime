#include "SaveState.hpp"
#include "Game.hpp"
#include "FishItem.hpp"
#include "AudioManager.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

namespace fs = std::filesystem;

SaveState::SaveState(StateStack& stack, Game& game, Player& player)
    : State(stack)
    , m_game(game)
    , m_player(player)
    , m_text(m_font)    // 构造时绑定字体
    , m_bgSprite{ game.textures().get("load_bg") }
{
}

void SaveState::onCreate() {
    // 加载字体
    if (!m_font.openFromFile("font/msyh.ttf"))
        std::cerr << "SaveState: 字体加载失败\n";

    auto winSize = m_game.window().getSize();

    // 标题
    m_text.setFont(m_font);
    m_text.setString(L"");
    m_text.setCharacterSize(60);

    // 返回按钮
    m_backButton = std::make_unique<Button>(L"", m_font);
    m_backButton->setSize(90.f, 90.f);
    m_backButton->setTexture(
        m_game.textures().get("btnclose")
    );
    m_backButton->setPosition(winSize.x - 120.f, 30.f);
    m_backButton->setOnClick([this] {
        m_game.scheduleAction([this] {
            m_game.popState();
        });
    });

    // 四个存档槽位按钮
    for (int i = 1; i <= 4; ++i) {
        // 按钮
        auto btn = std::make_unique<Button>(L"", m_font);
        btn->setTextColor(sf::Color::Transparent);
        
        // —— 1) 按钮尺寸 900×125
        btn->setSize(900.f, 125.f);
        // —— 2) 将原点设到按钮中心，便于后续居中定位
        btn->setOrigin(450.f, 62.5f);
        // —— 3) 默认背景透明（RGBA = 0）
        btn->setDefaultFillColor(sf::Color::Transparent);
        // 半透明 hover（和其他按钮逻辑一致），白色+透明度
        btn->setHoverFillColor(sf::Color(255, 255, 255, 72));
        // —— 4) 文字颜色设为黑色
        btn->setTextColor(sf::Color::Black);

        int slot = i; // 捕获槽位号
        btn->setOnClick([this, slot] {
            saveToSlot(slot);
            updateSlotStatus();
        });
        m_slotButtons.push_back(std::move(btn));

        // 状态文本
        sf::Text status(m_font);
        status.setCharacterSize(30);
        status.setFillColor(sf::Color::Black);
        m_slotStatusTexts.push_back(status);
    }
}

void SaveState::onActivate() {
    AudioManager::get().playMusic("play");

    // 背景填满
    auto winSize = m_game.window().getSize();
    auto texSize = m_bgSprite.getTexture().getSize();
    m_bgSprite.setScale({
        float(winSize.x) / texSize.x,
        float(winSize.y) / texSize.y
    });

    // 布局标题
    auto tb = m_text.getLocalBounds();
    m_text.setOrigin(tb.size * 0.5f);
    m_text.setPosition({ winSize.x / 2.f, 100.f });

    // 读取 load/slotX.txt 并更新 m_slotStatusTexts
    updateSlotStatus();

    // 布局槽位按钮：两列两行
    float centerX = winSize.x / 2.f + 8.f;
    float yStart = 225.f;
    float yGap = 216.f;
    for (size_t i = 0; i < m_slotButtons.size(); ++i) {
        float x = centerX;
        float y = yStart + static_cast<float>(i) * yGap;
        if (i == m_slotButtons.size() - 3) {
            y += 3.f;
        }
        else if (i == m_slotButtons.size() - 1) {
            y += 8.f;
        }
        m_slotButtons[i]->setPosition(x, y);
        

        // 状态文本放在按钮中央
        m_slotStatusTexts[i].setPosition({ x -300.f, y - 12.f });
    }
}
void SaveState::onDeactivate() {

}

void SaveState::handleEvent(const sf::Event& ev) {
    auto& win = m_game.window();
    auto mousePos = win.mapPixelToCoords(sf::Mouse::getPosition(win));

    // 返回按钮
    m_backButton->handleEvent(ev, mousePos);

    // 槽位按钮
    for (auto& btn : m_slotButtons)
        btn->handleEvent(ev, mousePos);

    // ESC 快捷键 —— 同“继续游戏”
    if (const auto* key = ev.getIf<sf::Event::KeyPressed>(); key && key->code == sf::Keyboard::Key::Escape) {
        m_game.scheduleAction([this] {
            m_game.popState();  // 弹出 PauseState
        });
    }
}

void SaveState::draw(sf::RenderTarget& target) const {
    // 背景
    target.draw(m_bgSprite);
    // 标题
    target.draw(m_text);
    // 返回按钮
    m_backButton->draw(target);
    // 槽位按钮 + 状态文本
    for (auto& btn : m_slotButtons)
        btn->draw(target);
    for (auto& status : m_slotStatusTexts)
        target.draw(status);
}

void SaveState::updateSlotStatus() {
    // 所有鱼种 ID 及初始化计数
    std::vector<std::string> fishIds = { "fish1","fish2","fish3","fish4","fish5" };

    for (size_t i = 0; i < 4; ++i) {
        int slot = int(i) + 1;
        fs::path file = fs::path("load") / ("slot" + std::to_string(slot) + ".txt");
        std::wstring text;
        std::wstring prefix = L"槽位 " + std::to_wstring(slot) + L"      ";
        if (fs::exists(file)) {
            std::ifstream ifs(file.string());
            std::string line;
            std::getline(ifs, line);          // 跳过位置行
            std::getline(ifs, line);          // 读取数量行
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

void SaveState::saveToSlot(int slot) {
    // 确保存档目录存在
    fs::path dir("load");
    if (!fs::exists(dir)) {
        fs::create_directory(dir);
    }

    // 槽位文件名：load/slotX.txt
    fs::path file = dir / ("slot" + std::to_string(slot) + ".txt");
    std::ofstream ofs(file);
    if (!ofs) {
        std::cerr << "SaveState: 无法创建存档文件 " << file << "\n";
        return;
    }

    // 1) 存玩家位置
    auto pos = m_player.getPosition();
    ofs << pos.x << ' ' << pos.y << '\n';

    // 2) 统计各鱼种数量
    std::vector<std::string> fishIds = { "fish1","fish2","fish3","fish4","fish5" };
    std::map<std::string, int> counts;
    for (auto& id : fishIds) counts[id] = 0;

    auto& inv = m_player.inventory();
    for (size_t i = 0; i < inv.size(); ++i) {
        if (auto* fish = dynamic_cast<FishItem*>(inv.get(i))) {
            counts[fish->getTextureId()]++;
        }
    }

    // 3) 写入数量
    for (auto& id : fishIds)
        ofs << counts[id] << ' ';
    ofs << '\n';

    ofs.close();
    std::cout << "已保存到 槽位 " << slot << "\n";
}
