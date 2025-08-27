#pragma once

#include <SFML/Graphics.hpp>

#include "GameObject.hpp"
#include "Animator.hpp"
#include "ResourceManager.hpp"
#include "Inventory.hpp"
#include "MovementBoundary.hpp"
#include "QTEManager.hpp"
#include "AudioManager.hpp"
#include <vector>
#include <string>


class Player : public GameObject {
public:
    // moveSpeed: 每秒像素速度
    explicit Player(float moveSpeed);

    // 加载一套动画，name 用于后续 play
    bool loadAnimation( const std::string& name,
                        const std::string& folder,
                        int frameCount,
                        float secondsPerFrame,
                        ResourceManager<sf::Texture>& textures,
                        const sf::Vector2f& scale = { 1.f, 1.f });
    
    // 切换到指定动画
    void playAnimation(const std::string& name) {
        mAnimator.play(name);
        // 第一次创建或切换动画后，强制把精灵摆到我们保存在 mPosition 的位置
        mAnimator.setPosition(mPosition);
    }

    // 处理事件（可选扩展）
    void handleEvent(const sf::Event& event) override;

    // 移动 + 动画推进
    void update(sf::Time dt) override;

    // 绘制当前动画帧
    void draw(sf::RenderTarget& target) const override;

    // 设置玩家初始/后续位置
    void setPosition(const sf::Vector2f& pos) {
        mPosition = pos;
        mAnimator.setPosition(pos);
    }

    sf::Vector2f getPosition() const {
        return mPosition;
    }

    void setMovementBoundary(const std::vector<sf::Vector2f>& vertices) {
        mBoundary.setVertices(vertices);
    }

    // 让 PlayState 能调用 QTEManager 加载字体
    bool loadQTEFont(const std::string& filepath, unsigned int charSize = 48) {
        return mQTEManager.loadFont(filepath, charSize);
    }

    // 访问背包
    Inventory& inventory() { return m_inventory; }
    const Inventory& inventory() const { return m_inventory; }

    // 新增：钓到鱼回调 
    using FishCallback = std::function<void(const std::string&)>;
    void setFishCaughtCallback(FishCallback cb) { onFishCaught = std::move(cb); }

private:
    // 判断：向左 1 像素就出界，则在左边界
    bool isAtLeftBoundary() const {
        auto pos = mAnimator.getPosition();
        sf::Vector2f test{ pos.x - 5.f, pos.y };
        return !mBoundary.contains(test);
    }

    sf::Vector2f mPosition{ 1440.f,  940.f - 160.f }; // 默认起点


    // 状态
    enum class State { Idle,
                       Moving,
                       StartFishing,
                       Fishing,
                       FinishFishing };

    State           mState = State::Idle;

    float        mMoveSpeed;
    Animator     mAnimator;

    Inventory m_inventory;  // 背包组件

    MovementBoundary mBoundary;

    // —— QTE 管理器 —— 
    QTEManager      mQTEManager;

    // 随机鱼种相关
    std::mt19937                            m_rng;
    std::uniform_int_distribution<int>      m_fishDist;
    std::vector<std::string>                m_fishIds;

    // Start 动画完成标志
    bool             mStartDone = false;
    // Finish 动画完成标志
    bool            mFinishDone = false;
    // 从 Moving 切回 Idle 前的“等待一圈播完”标志 
    bool     mPendingIdleSwitch = false;

    // 移动音效播放标志
    bool m_moveSoundPlayed = false;

    // 钓到鱼回调
    FishCallback onFishCaught;
};
