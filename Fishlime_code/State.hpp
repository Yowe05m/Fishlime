#pragma once
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

class StateStack;

class State {
public:

    explicit State(StateStack& stack)
        : m_stack(stack)
    {}

    virtual ~State() = default;


    // —— 生命周期钩子 —— 
    virtual void onCreate() {}      // 第一次 push 时调用
    virtual void onActivate() {}    // 每次进入（包括第一次）时调用
    virtual void onDeactivate() {}  // 每次离开（被覆盖或 pop）时调用
    virtual void onDestroy() {}     // 最后一次 pop 时调用

    virtual void handleEvent(const sf::Event& ev) = 0;
    virtual void update(sf::Time dt) = 0;
    virtual void draw(sf::RenderTarget& rt) const = 0;

protected:
    // 如果需要在 State 内部操作状态栈，可通过此方法访问
    StateStack& getStack() { return m_stack; }

private:
    // ← 新增：保存传入的状态栈引用
    StateStack& m_stack;
};