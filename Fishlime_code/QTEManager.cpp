#include "QTEManager.hpp"
#include "AudioManager.hpp"
#include <random>

QTEManager::QTEManager()
    : m_text(m_font)
    , m_rng(std::random_device{}())
    , m_intervalDist(2.f, 3.f)
    , m_keyDist(0, 3)
{
}

bool QTEManager::loadFont(const std::string& filepath, unsigned int charSize) {
    if (!m_font.openFromFile(filepath))
        return false;

    m_text.setFont(m_font);
    m_text.setCharacterSize(charSize);
    m_text.setFillColor(sf::Color::White);

    // 把原点移到中心，便于后面 draw 时居中显示
    auto b = m_text.getLocalBounds();
    m_text.setOrigin(b.position + b.size * 0.5f);

    return true;
}

void QTEManager::start() {
    m_promptActive = false;
    m_finished = false;
    m_success = false;
    m_successCount = 0;
    m_timer.restart();
    scheduleNextQTE();
}

void QTEManager::scheduleNextQTE() {
    float sec = m_intervalDist(m_rng);
    m_nextTime = sf::seconds(sec);
}

QTEManager::Key QTEManager::randomKey() {
    return static_cast<Key>(m_keyDist(m_rng));
}

sf::Keyboard::Key QTEManager::toSfKey(Key k) {
    switch (k) {
    case Key::A: return sf::Keyboard::Key::A;
    case Key::W: return sf::Keyboard::Key::W;
    case Key::S: return sf::Keyboard::Key::S;
    case Key::D: return sf::Keyboard::Key::D;
    }
    return sf::Keyboard::Key::A;
}

void QTEManager::update(sf::Time /*dt*/) {
    if (m_finished)
        return;

    if (m_promptActive) {
        // 超时判定
        if (m_timer.getElapsedTime() > m_timeout) {
            m_finished = true;
            m_success = false;
        }
    }
    else {
        // 到时间触发一次提示
        if (m_timer.getElapsedTime() >= m_nextTime) {
            m_promptActive = true;
            m_currentKey = randomKey();
            // —— 播放 QTE 开始音 —— 
            AudioManager::get().playSound("QTEStart");

            // 设置文本
            switch (m_currentKey) {
            case Key::A: m_text.setString(L"按 A ！"); break;
            case Key::W: m_text.setString(L"按 W ！"); break;
            case Key::S: m_text.setString(L"按 S ！"); break;
            case Key::D: m_text.setString(L"按 D ！"); break;
            }
            // 居中原点
            auto b = m_text.getLocalBounds();
            m_text.setOrigin(b.position + b.size * 0.5f);
            m_timer.restart();
        }
    }
}

void QTEManager::handleEvent(const sf::Event& event) {
    if (m_finished || !m_promptActive)
        return;

    if (event.is<sf::Event::KeyPressed>()) {
        auto code = event.getIf<sf::Event::KeyPressed>()->code;
        if (code == toSfKey(m_currentKey)) {
            // 成功
            m_successCount++;
            if (m_successCount < 3) {
                AudioManager::get().playSound("QTESuccess");
                if (m_successCount == 2) {
                    AudioManager::get().playSound("QTESuccess2");
                }
                // 继续下一个
                m_promptActive = false;
                scheduleNextQTE();
                m_timer.restart();
            }
            if (m_successCount >= 3) {
                m_finished = true;
                m_success = true;
                AudioManager::get().playSound("FinishFishing");
            }
        }
        else {
            // 失败
            m_finished = true;
            m_success = false;
            AudioManager::get().playSound("FishingFailed");
        }
    }
}

void QTEManager::draw(sf::RenderTarget& target, const sf::Vector2f& position) const {
    if (m_promptActive) {
        // 拷一份让 const 函数里也能 setPosition
        sf::Text tmp = m_text;
        tmp.setPosition(position);
        target.draw(tmp);
    }
}

