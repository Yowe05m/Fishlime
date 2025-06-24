#include <SFML/Graphics.hpp>
#include <optional>
#include <cstdlib>

enum class QTE { A, S, D, W, F };

sf::Keyboard::Key toKey(QTE q) {
    switch (q) {
    case QTE::A: return sf::Keyboard::Key::A;
    case QTE::W: return sf::Keyboard::Key::W;
    case QTE::S: return sf::Keyboard::Key::S;
    case QTE::D: return sf::Keyboard::Key::D;
    case QTE::F: return sf::Keyboard::Key::F;
    }
    return sf::Keyboard::Key::A;
}

int main() {
    // ——1) 窗口与视频模式
    // VideoMode 构造必须双大括号
    sf::VideoMode vm{ {800, 600} };
    // 标题直接用 string_view，不再接收 sf::String
    sf::RenderWindow window{ vm, sf::String(L"QTE 演示") };

    // ——2) 字体加载
    sf::Font font;
    // SFML3 中改为 openFromFile，不要再用 loadFromFile
    if (!font.openFromFile("font/msyh.ttf"))
        return EXIT_FAILURE;  // 建议用 EXIT_* 宏 :contentReference[oaicite:0]{index=0}

    // ——3) 文本对象
    // Text 在构造时必须绑定一个已加载的 Font
    sf::Text prompt{ font, "", 48 };
    prompt.setPosition({ 200.f, 250.f });
    prompt.setString(L"按 Space 开始 QTE");

    // ——QTE 状态
    bool      qteActive = false;
    QTE       qteKey = QTE::A;
    sf::Clock qteClock;
    sf::Time  qteTimeout = sf::seconds(3.f);

    while (window.isOpen()) {
        // ——4) 事件循环
        // pollEvent 返回 std::optional<sf::Event>
        while (const std::optional<sf::Event> evtOpt = window.pollEvent()) {
            // 访问方式：先用 is<T>() 判断，再用 getIf<T>() 拿到数据
            if (evtOpt->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (qteActive && evtOpt->is<sf::Event::KeyPressed>()) {
                // 正确拿到按键码
                auto keyCode = evtOpt->getIf<sf::Event::KeyPressed>()->code;
                if (keyCode == toKey(qteKey)) {
                    prompt.setString(L"QTE 成功！");
                }
                else {
                    prompt.setString(L"按错了！");
                }
                qteActive = false;
            }
        }

        // ——5) 超时判定
        if (qteActive && qteClock.getElapsedTime() > qteTimeout) {
            prompt.setString(L"QTE 失败！（超时）");
            qteActive = false;
        }

        // ——6) 触发 QTE（按 Space 开始）
        if (!qteActive && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
            qteActive = true;
            qteKey = static_cast<QTE>(std::rand() % 5);
            qteClock.restart();
            switch (qteKey) {
            case QTE::A: prompt.setString(L"按 A ！"); break;
            case QTE::W: prompt.setString(L"按 W ！"); break;
            case QTE::S: prompt.setString(L"按 S ！"); break;
            case QTE::D: prompt.setString(L"按 D ！"); break;
            case QTE::F: prompt.setString(L"按 F ！"); break;
            }
        }

        // ——7) 渲染
        window.clear(sf::Color::Black);
        window.draw(prompt);
        window.display();
    }

    return EXIT_SUCCESS;
}
