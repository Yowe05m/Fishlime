#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Time.hpp>
#include <random>

class QTEManager {
public:
    // QTE 可出提示的按键
    enum class Key { A, W, S, D };

    QTEManager();

    bool loadFont(const std::string& filepath, unsigned int charSize = 48);

    // 启动一次钓鱼 QTE 流程（重置内部状态）
    void start();

    // 每帧调用，处理定时与提示出现
    void update(sf::Time dt);

    // 处理键盘事件，判定 QTE 成败
    void handleEvent(const sf::Event& event);

    // 在指定位置绘制当前激活的提示
    void draw(sf::RenderTarget& target, const sf::Vector2f& position) const;

    // QTE已成功次数
    int getSuccessCount() const { return m_successCount; }
    // 流程是否已结束
    bool isFinished() const { return m_finished; }
    // 结束后，是否达到 3 连续成功
    bool isSuccessful() const { return m_finished && m_success; }
    // 当前是否有提示待按
    bool isPromptActive() const { return m_promptActive; }

private:
    void scheduleNextQTE();        // 安排下一次随机触发
    Key   randomKey();             // 随机一个 Key
    sf::Keyboard::Key toSfKey(Key k);

    sf::Font           m_font;
    sf::Text              m_text;

    std::mt19937          m_rng;
    std::uniform_real_distribution<float> m_intervalDist; // 2–3s
    std::uniform_int_distribution<int>    m_keyDist;      // 0–3

    sf::Clock             m_timer;
    sf::Time              m_nextTime;     // 下次触发时刻
    const sf::Time        m_timeout = sf::seconds(3.f);

    bool                  m_promptActive = false;
    Key                   m_currentKey = Key::A;

    int                   m_successCount = 0;
    bool                  m_finished = false;
    bool                  m_success = false;
};
