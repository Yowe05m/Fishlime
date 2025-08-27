#pragma once

#include <map>
#include <string>
#include <optional> 
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include "Animation.hpp"

// Animator：管理若干 Animation，并提供 play/update/draw 接口
class Animator {
public:
    // 默认构造函数
    Animator() = default;

    // 添加一套动画
    void addAnimation(const std::string& name, const Animation& anim);

    // 切换到指定动画
    void play(const std::string& name);

    // 推进当前动画
    void update(sf::Time dt);

    // 绘制当前帧
    void draw(sf::RenderTarget& target) const;

    // 位置控制
    void setPosition(const sf::Vector2f& pos);

    // —— 新增：获取当前帧索引，用于 “等一圈播完” 的判断 —— 
    std::size_t getCurrentFrameIndex() const { return mCurrentFrame; }

    // 获取当前位置
    sf::Vector2f getPosition() const;

private:
    std::map<std::string, Animation> mAnimations;
    Animation*                  mCurrentAnim = nullptr;
    std::size_t                 mCurrentFrame = 0;
    sf::Time                    mFrameTime = sf::Time::Zero;
    std::optional<sf::Sprite>   mSprite; // 延迟构造
};
