#define SFML_DYNAMIC           // 一定要在任何 SFML 头之前定义
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

int main() {
    // 1. 创建窗口
        // 1. 构造 VideoMode（内部是 Vector2u）
    sf::VideoMode vm({ 800, 600 });
    // 2. 准备好一个 UTF-8 编码的 std::string
    std::string utf8 = u8"SFML 3.0 示例";

    // 3. 用 fromUtf8 将字节流转成 sf::String（UTF-32）
    sf::String title = sf::String::fromUtf8(utf8.begin(), utf8.end());

    // 4. 用转换后的 title 构造窗口
    sf::RenderWindow window{ vm, (L"SFML 3.0 示例") };

    // 5. 准备一个圆形
    sf::CircleShape circle(50.f);    // 半径 50
    circle.setFillColor(sf::Color::Green);
    // 把圆心移动到窗口中央（800/2,600/2），注意 origin 也是中心
    circle.setOrigin({ 50.f, 50.f }); // 将 circle 的原点从默认的左上角移动到圆心
    circle.setPosition({ 400.f, 300.f });

    // 6. 主循环
    while (window.isOpen()) {
        // 6.1 事件处理
        while (auto evOpt = window.pollEvent()) {
            auto& ev = *evOpt;
            if (ev.is<sf::Event::Closed>())
                window.close();
        }

        // 6.2 清屏（黑色背景）
        window.clear();

        // 6.3 绘制（这里把 circle 画进来）
        window.draw(circle);

        // 6.4 更新到屏幕
        window.display();
    }

    return 0;
}