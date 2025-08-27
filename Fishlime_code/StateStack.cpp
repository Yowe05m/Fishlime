#include "StateStack.hpp"
#include "LoadState.hpp"
#include <SFML/Graphics/RectangleShape.hpp>  // 暗色罩

StateStack::StateStack(Context context)
    : m_context(std::move(context))
{
}

void StateStack::applyPending() {
    for (auto& p : m_pending) {
        switch (p.first) {
        case Push:
            m_stack.push_back(std::move(p.second));
            m_stack.back()->onCreate();      // 第一次创建
            m_stack.back()->onActivate();    // 第一次激活
            break;
        case Pop:
            if (!m_stack.empty()) {
                m_stack.back()->onDeactivate();
                m_stack.back()->onDestroy();
                m_stack.pop_back();
            }
            if (!m_stack.empty())
                m_stack.back()->onActivate();  // 恢复下层状态
            break;
        case Clear:
            for (auto& s : m_stack) {
                s->onDeactivate();
                s->onDestroy();
            }
            m_stack.clear();
            break;
        }
    }
    m_pending.clear();
}

void StateStack::handleEvent(const sf::Event& e) {
    if (!m_stack.empty())
        m_stack.back()->handleEvent(e);
}
void StateStack::update(sf::Time dt) {
    if (!m_stack.empty())
        m_stack.back()->update(dt);
}
void StateStack::draw(sf::RenderTarget& target) const {
    if (m_stack.empty()) return;

    const std::size_t sz = m_stack.size();
    bool isLoadOverlay = false;



    if (sz > 1) {

        // 判断栈顶是不是 LoadState
        isLoadOverlay = dynamic_cast<const LoadState*>(m_stack.back().get()) != nullptr;

        if (!isLoadOverlay) {
            // 半透明模式：先把 倒数第二个状态 画出来
            m_stack[sz - 2]->draw(target);
        }

        // 然后一层黑幕：
        auto windowSize = target.getSize();
        sf::RectangleShape fade{ sf::Vector2f{
            float(windowSize.x), float(windowSize.y)
        } };

        // LoadState 用 255（全黑），其他 overlay 用 150（半黑）
        fade.setFillColor(sf::Color(0, 0, 0,
            isLoadOverlay ? 255 : 150));
        target.draw(fade);
    }

    // 最后绘制栈顶状态
    m_stack.back()->draw(target);
}
