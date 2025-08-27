#include "Animator.hpp"
#include <stdexcept>

void Animator::addAnimation(const std::string& name, const Animation& anim) {
    mAnimations.emplace(name, anim);
}

void Animator::play(const std::string& name) {
    auto it = mAnimations.find(name);
    if (it == mAnimations.end())
        throw std::runtime_error("Animator: no animation named '" + name + "'");
    if (mCurrentAnim != &it->second) {
        mCurrentAnim = &it->second;
        mCurrentFrame = 0;
        mFrameTime = sf::Time::Zero;

        // —— 切换动画时保留当前位置 —— 
        sf::Vector2f prevPos = getPosition();

        // 首次构造 or 之后都用 setTexture 保留状态
        if (!mSprite.has_value())
            mSprite.emplace(*mCurrentAnim->frames[0]);  // 构造一次
        else
            mSprite->setTexture(*mCurrentAnim->frames[0], true);

        // 设原点 & 缩放
        auto bounds = mSprite->getLocalBounds();
        mSprite->setOrigin(bounds.size * 0.5f);
        mSprite->setScale(mCurrentAnim->scale);

        // 恢复位置
        mSprite->setPosition(prevPos);
    }
}

void Animator::update(sf::Time dt) {
    if (!mCurrentAnim || mCurrentAnim->frames.empty())
        return;

    mFrameTime += dt;
    while (mFrameTime >= mCurrentAnim->frameDuration) {
        mFrameTime -= mCurrentAnim->frameDuration;
        mCurrentFrame = (mCurrentFrame + 1) % mCurrentAnim->frames.size();
        mSprite->setTexture(*mCurrentAnim->frames[mCurrentFrame], true);
    }
}

void Animator::draw(sf::RenderTarget& target) const {
    if (mSprite.has_value())
        target.draw(*mSprite);
}

void Animator::setPosition(const sf::Vector2f& pos) {
    if (mSprite.has_value())
        mSprite->setPosition(pos);
}

sf::Vector2f Animator::getPosition() const {
    if (mSprite.has_value())
        return mSprite->getPosition();
    return {};
}
