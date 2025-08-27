#include "HelpState.hpp"
#include "Game.hpp"
#include "AudioManager.hpp"
#include <iostream>
#include <memory> 

HelpState::HelpState(StateStack& stack, Game& game)
    : State(stack)
    , m_game(game)
    , m_sprite(game.textures().get("help1"))
    , m_counterText(m_font)
{ }

void HelpState::onCreate() {

    // 1. 加载字体
    if (!m_font.openFromFile("font/msyh.ttf")) {
        std::cerr << "HelpState: 字体加载失败\n";
    }

    // 2. 初始化图片 ID 列表（与 Game 中 preload 保持一致）
    const int total = 9; // 根据实际图片数量修改
    for (int i = 1; i <= total; ++i) {
        m_imageIds.push_back("help" + std::to_string(i));
    }

    // 3. 创建按钮
    m_prevButton = std::make_unique<Button>(L"上一张", m_font);
    m_nextButton = std::make_unique<Button>(L"下一张", m_font);
    // 返回按钮
    m_backButton = std::make_unique<Button>(L"返回菜单", m_font);
    m_backButton->setOnClick([this] {
        m_game.scheduleAction([this] {
            m_game.popState();
        });
    });
    m_backButton->setPosition(50.f, m_game.window().getSize().y - 80.f);

    
    // 4. 设置按钮回调：改变 m_currentIndex，并刷新显示
    m_prevButton->setOnClick([this] {
        if (m_currentIndex > 0) {
            --m_currentIndex;
            loadCurrentTexture();
            updateControls();
        }
    });
    m_nextButton->setOnClick([this] {
        if (m_currentIndex + 1 < m_imageIds.size()) {
            ++m_currentIndex;
            loadCurrentTexture();
            updateControls();
        }
    });

    // 5. 初始化底部“xx/yy”文本属性
    m_counterText.setFont(m_font);
    m_counterText.setCharacterSize(30);
    m_counterText.setFillColor(sf::Color::White);

    // 6. 一开始就加载第 1 张图并更新控件
    loadCurrentTexture();
    updateControls();
}

void HelpState::onActivate() {
    AudioManager::get().playMusic("menu");
    m_prevButton->setHoverSound("BtnHovered");
    m_prevButton->setPressSound("HelpStateBtn");
    m_nextButton->setHoverSound("BtnHovered");
    m_nextButton->setPressSound("HelpStateBtn");
}

void HelpState::onDestroy() {
    // 清理资源
    m_prevButton.reset();
    m_nextButton.reset();
    m_imageIds.clear();
}

void HelpState::loadCurrentTexture() {
    // 从 ResourceManager 获取当前纹理
    const std::string& id = m_imageIds[m_currentIndex];
    const sf::Texture& tex = m_game.textures().get(id);
    m_sprite.setTexture(tex, true);

    // 计算缩放：保持图片最大在窗口的 70%
    auto winSize = m_game.window().getSize();
    auto imgSize = tex.getSize();
    float scaleX = (winSize.x * 0.7f) / imgSize.x;
    float scaleY = (winSize.y * 0.7f) / imgSize.y;
    m_sprite.setScale({ scaleX, scaleY });              // 设置缩放 

    // 居中显示（偏上）
    auto bounds = m_sprite.getGlobalBounds();
    m_sprite.setPosition({
        (winSize.x - bounds.size.x) / 2.f,
        (winSize.y - bounds.size.y) / 2.f - 50.f
    });
}

void HelpState::updateControls() {
    // 更新“xx/yy”文本
    std::wstring counter =
        (m_currentIndex < 9 ? L"0" : L"") + std::to_wstring(m_currentIndex + 1)
        + L"/"
        + (m_imageIds.size() < 10 ? L"0" : L"") + std::to_wstring(m_imageIds.size());
    m_counterText.setString(counter);

    // 布局到窗口底部
    auto winSize = m_game.window().getSize();
    float y = winSize.y * 0.85f;
    m_prevButton->setPosition( winSize.x * 0.3f, y );
    m_nextButton->setPosition( winSize.x * 0.7f - 300.f, y );
    // 文本居中
    auto txtBounds = m_counterText.getLocalBounds();
    m_counterText.setPosition({
        (winSize.x - txtBounds.size.x) / 2.f,
        y + 40.f
    });
}

void HelpState::update(sf::Time) {
    
}

void HelpState::handleEvent(const sf::Event& event) {
    auto mousePos = m_game.window().mapPixelToCoords(sf::Mouse::getPosition(m_game.window()));
    // 仅当之前/之后有页面时才传事件给按钮
    if (m_currentIndex > 0)
        m_prevButton->handleEvent(event, mousePos);
    if (m_currentIndex + 1 < m_imageIds.size())
        m_nextButton->handleEvent(event, mousePos);

    m_backButton->handleEvent(event, mousePos);

    // ESC 快捷键（返回主页）
    if (const auto* key = event.getIf<sf::Event::KeyPressed>(); key && key->code == sf::Keyboard::Key::Escape) {
        m_game.scheduleAction([this] {
            m_game.popState(); 
        });
    }
}

void HelpState::draw(sf::RenderTarget& target) const {
    target.draw(m_sprite);
    target.draw(m_counterText);

    // 只有当存在上一页/下一页时才绘制对应按钮
    if (m_currentIndex > 0)
        m_prevButton->draw(target);
    if (m_currentIndex + 1 < m_imageIds.size())
        m_nextButton->draw(target);
    m_backButton->draw(target);
}