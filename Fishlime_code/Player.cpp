#include "Player.hpp"
#include "FishItem.hpp"
#include "AudioManager.hpp"
#include <filesystem>
#include <iostream>
#include <cmath>
#include <random>

Player::Player(float moveSpeed)
    : mMoveSpeed(moveSpeed)
    , m_rng(std::random_device{}())                 // 随机数引擎
    , m_fishDist(0, 4)                              // 下标 [0,4] 共 5 种鱼
    , m_fishIds{ "fish1","fish2","fish3","fish4","fish5" }  // 与资源管理中 ID 一一对应
    , m_moveSoundPlayed(false)
{}

bool Player::loadAnimation(const std::string& name,
    const std::string& folder,
    int frameCount,
    float secondsPerFrame,
    ResourceManager<sf::Texture>& textures,
    const sf::Vector2f& scale)
{
    Animation anim;
    anim.frameDuration = sf::seconds(secondsPerFrame);
    anim.scale = scale;

    for (int i = 1; i <= frameCount; ++i) {
        std::string path = folder + "/" + std::to_string(i) + ".png";
        std::string id = name + "_" + std::to_string(i);
        try {
            textures.load(id, path);
        }
        catch (const std::exception& e) {
            std::cerr << "Player::loadAnimation failed to load " << path
                << " : " << e.what() << "\n";
            return false;
        }
        anim.frames.push_back(&textures.get(id));
    }

    mAnimator.addAnimation(name, anim);
    return true;
}

void Player::handleEvent(const sf::Event& event) {
    // —— 钓鱼中按 Space 立即取消钓鱼 —— 
    if (mState == State::Fishing
        && event.is<sf::Event::KeyPressed>()
        && event.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Space)
        {
            // 切到收杆动画
            mState = State::FinishFishing;
            mFinishDone = false;
            playAnimation("Finish");
            return;   // 不再交给 QTEManager，也不做其它响应
    }

    // QTE 专用：优先交给 QTEManager
    if (mState == State::Fishing) {
        mQTEManager.handleEvent(event);
        // 成败判定后会在 update() 中切换状态
    }

    // 抛竿开始：Idle 且在最左边按 Space
    if (event.is<sf::Event::KeyPressed>()) {
        auto kp = event.getIf<sf::Event::KeyPressed>();
        if (kp->code == sf::Keyboard::Key::Space &&
            mState == State::Idle &&
            isAtLeftBoundary())
        {
            // 播放抛竿开始音
            AudioManager::get().playSound("StartFishing");
            // 切到抛竿动画
            mState = State::StartFishing;
            mStartDone = false;
            playAnimation("Start");
        }
    }
}

struct FishInfo {
    std::string textureId;
    int         weight;
    int         rarity;
};

std::vector<FishInfo> m_fishInfos{
    {"fish1", 1, 1},
    {"fish2", 2, 2},
    {"fish3", 3, 3},
    {"fish4", 4, 4},
    {"fish5", 5, 5},
};

void Player::update(sf::Time dt) {
    // Fish
    // —— 1) StartFishing 状态：等一圈播完后切到 Fishing —— 
    if (mState == State::StartFishing) {
        mAnimator.update(dt);
        // 播放到第1帧以上后视为已启动
        if (!mStartDone && mAnimator.getCurrentFrameIndex() > 0)
            mStartDone = true;
        // 一圈播完（从 >0 回到 0）
        if (mStartDone && mAnimator.getCurrentFrameIndex() == 0) {
            // 抛竿动画播完，播放第二段音
            AudioManager::get().playSound("StartFishing2");
            // 进入真正钓鱼状态
            mState = State::Fishing;
            playAnimation("Fishing");
            mQTEManager.start();
        }
        return;
    }

    // —— 2) Fishing 状态：保持等待动画 & QTE —— 
    if (mState == State::Fishing) {
        mAnimator.update(dt);
        mQTEManager.update(dt);

        if (mQTEManager.isFinished()) {
            // QTE 完成：成功则加鱼
            if (mQTEManager.isSuccessful()) {
                // —— 钓鱼成功，随机选择一种鱼 —— 
                int idx = m_fishDist(m_rng);
                auto& info = m_fishInfos[idx];
                m_inventory.add(
                    std::make_unique<FishItem>(
                        info.weight,
                        info.rarity,
                        info.textureId
                    )
                );
                std::cout << "成功获得：" << m_fishIds[idx] << "\n";

                // 钓到鱼回调
                if (onFishCaught)
                    onFishCaught( m_fishIds[idx] );
            }
            // 切 Finish
            mState = State::FinishFishing;
            mFinishDone = false;
            playAnimation("Finish");
        }
        return;
    }

    // —— 3) FinishFishing 状态：等一圈播完后回 Idle —— 
    if (mState == State::FinishFishing) {
        mAnimator.update(dt);
        if (!mFinishDone && mAnimator.getCurrentFrameIndex() > 0)
            mFinishDone = true;
        if (mFinishDone && mAnimator.getCurrentFrameIndex() == 0) {
            mState = State::Idle;
            playAnimation("Idle");
        }
        return;
    }

    // Move
    // —— 1) 先处理移动状态切换 —— 
    bool isMoving = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    {
        isMoving = true;

        if (mState == State::Idle)
        {
            // 立刻进入移动动画
            mState = State::Moving;
            mPendingIdleSwitch = false;
            playAnimation("Move");
            // 重置标志
            m_moveSoundPlayed = false;
        }
    }

    // 2) 状态切换逻辑
    if (!isMoving && mState == State::Moving && !mPendingIdleSwitch){
        mPendingIdleSwitch = true;
    }

    // 3) 推进动画
    mAnimator.update(dt);

    if (mState == State::Moving) {
        int frame = mAnimator.getCurrentFrameIndex();
        if (frame == 2 && !m_moveSoundPlayed) {
            AudioManager::get().playSound("Moving");
            m_moveSoundPlayed = true;
        }
        // 动画回到帧 0 时重置，让下一个循环还能触发
        if (frame == 0) {
            m_moveSoundPlayed = false;
        }
    }

    // 4) 如果在“等待切Idle”且动画已循环到帧0，才真正切回 Idle
    if (mPendingIdleSwitch &&
        mAnimator.getCurrentFrameIndex() == 0)
    {
        mPendingIdleSwitch = false;
        mState = State::Idle;
        playAnimation("Idle");
    }

    // 5) 在 Moving 状态下，计算新位置并做边界检测
    if (isMoving && mMoveSpeed > 0.f)
    {
        sf::Vector2f dir{ 0.f, 0.f };
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
            dir.y -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
            dir.y += 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            dir.x -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            dir.x += 1.f;

        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len != 0.f) dir /= len;

        auto pos = mAnimator.getPosition();
        sf::Vector2f newPos = pos + dir * mMoveSpeed * dt.asSeconds();

        // ← 边界检测：只有在多边形内部才移动
        if (mBoundary.contains(newPos)) {
            setPosition(newPos);
        }
    }
}

void Player::draw(sf::RenderTarget& target) const {
    mAnimator.draw(target);
    // Fishing 状态下绘制 QTE 提示
    if (mState == State::Fishing) {
        mQTEManager.draw(target, mAnimator.getPosition());
    }
}