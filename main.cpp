#define SFML_DYNAMIC
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

int main() {
    // 1. 创建窗口
    sf::VideoMode vm{ {800, 600} };
    sf::RenderWindow window{vm, (L"SFML 3.0 示例") };

    // 2. 加载字体
    sf::Font font;
    if (!font.openFromFile("font/msyh.ttf")) {
        std::cerr << "无法加载字体文件！\n";
        return -1;
    }   

    // 3. 准备中文
    sf::Text prompt{ font, "", 48 };
    prompt.setPosition({ 20.f, 20.f });
    prompt.setString(L"你好，SFML 3.0！");

    // 4. 主循环
    while (window.isOpen()) {
        // 4.1 事件处理
        while (auto evOpt = window.pollEvent()) {
            auto& ev = *evOpt;
            if (ev.is<sf::Event::Closed>())
                window.close();
        }

        // 4.2 清屏（黑色背景）
        window.clear();

        // 4.3 绘制
        window.draw(prompt);

        // 4.4 更新到屏幕
        window.display();
    }

    return 0;
}
