#pragma once
#include <string>
class Player; // 前向声明

// 抽象道具基类
class Item {
public:
    virtual ~Item() = default;
    // 获取道具名称
    virtual std::string getName() const = 0;
    // 使用道具（作用于玩家）
    virtual void use(Player& p) = 0;
};
