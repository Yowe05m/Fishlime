#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <stdexcept>


// 通用资源管理器
template<typename Resource>
class ResourceManager {
public:
    ResourceManager() = default;

    // 加载资源（只会真正 load 一次），id 用于后续 get 调用
    Resource& load(const std::string& id, const std::string& filepath) {
        // 如果已经加载过，直接返回
        if (auto it = m_resources.find(id); it != m_resources.end()) {
            return *it->second;
        }

        // 否则新建并加载
        auto res = std::make_unique<Resource>(filepath);

        Resource& ref = *res;
        m_resources.emplace(id, std::move(res));
        return ref;
    }

    // 获取已加载的资源
    Resource& get(const std::string& id) {
        auto it = m_resources.find(id);
        if (it == m_resources.end()) {
            throw std::runtime_error("ResourceManager: no resource with id " + id);
        }
        return *it->second;
    }

    // 清空所有缓存
    void clear() {
        m_resources.clear();
    }

private:
    std::unordered_map<std::string, std::unique_ptr<Resource>> m_resources;
};
