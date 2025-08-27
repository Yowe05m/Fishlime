#pragma once
#include <vector>
#include <memory>
#include "State.hpp"
#include "Context.hpp"

class StateStack {
public:
    enum Action { Push, Pop, Clear };

    explicit StateStack(Context context);

    // 推入新状态（模板方法）
    template <typename T, typename... Args>
    void pushState(Args&&... args) {
        m_pending.emplace_back(Push, std::make_unique<T>(*this, std::forward<Args>(args)...));
    }

    // 弹出当前状态
    void popState() { m_pending.emplace_back(Pop, nullptr); }

    // 清空所有状态
    void clearStates() { m_pending.emplace_back(Clear, nullptr); }

    // 真正把“挂起”操作应用到栈上（每帧末尾调用一次）
    void applyPending();

    // 每帧分发
    void handleEvent(const sf::Event& e);
    void update(sf::Time dt);
    void draw(sf::RenderTarget& target) const;

private:
    std::vector<std::unique_ptr<State>>          m_stack;
    std::vector<std::pair<Action, std::unique_ptr<State>>> m_pending;
    Context                                      m_context;
};
