#pragma once
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <memory>
#include <string>
#include <iostream>
#include <algorithm>

class AudioManager {
public:
    // 单例访问
    static AudioManager& get() {
        static AudioManager instance;
        return instance;
    }

    // 禁止拷贝
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    // 加载音效（一次性全部载入到内存）
    bool loadSound(const std::string& name, const std::string& filename) {
        auto buf = std::make_unique<sf::SoundBuffer>();
        if (!buf->loadFromFile(filename)) {
            std::cerr << "AudioManager: 无法加载音效 " << filename << "\n";
            return false;
        }
        auto snd = std::make_unique<sf::Sound>(*buf);
        snd->setVolume(sfxVolume * masterVolume / 100.f);
        soundBuffers[name] = std::move(buf);
        sounds[name] = std::move(snd);
        return true;
    }

    // 注册 BGM 文件（不立即打开）
    bool loadMusic(const std::string& name, const std::string& filename) {
        musicFiles[name] = filename;
        return true;
    }

    // 播放某条音效
    void playSound(const std::string& name) {
        auto it = sounds.find(name);
        if (it != sounds.end())
            it->second->play();
    }

    // 停止某条音效
    void stopSound(const std::string& name) {
        auto it = sounds.find(name);
        if (it != sounds.end())
            it->second->stop();
    }

    // 播放 BGM
    void playMusic(const std::string& name, bool loop = true) {
        // 如果要播放的正是当前这条 BGM
        if (name == currentMusicName) {
            auto status = backgroundMusic.getStatus();
            if (status == sf::SoundSource::Status::Paused) {
                backgroundMusic.play();    // 从 pause 恢复
            }
            // 如果已经在播放（Playing），什么都不做
            return;
        }

        // 否则切换到新 BGM
        auto it = musicFiles.find(name);
        if (it == musicFiles.end()) {
            std::cerr << "AudioManager: 未注册 BGM " << name << "\n";
            return;
        }
        if (!backgroundMusic.openFromFile(it->second)) {
            std::cerr << "AudioManager: 打开 BGM 文件失败 " << it->second << "\n";
            return;
        }

        currentMusicName = name;
        backgroundMusic.setLooping(loop);
        backgroundMusic.setVolume(musicVolume * masterVolume / 100.f);
        backgroundMusic.play();
    }

    // 停止/暂停/恢复
    void stopMusic() { backgroundMusic.stop(); }
    void pauseMusic() { backgroundMusic.pause(); }
    void resumeMusic() { backgroundMusic.play(); }

    // 音量控制接口
    void setMasterVolume(float v) {
        masterVolume = std::clamp(v, 0.f, 100.f);
        // 更新所有音效
        for (auto& [k, snd] : sounds)
            snd->setVolume(sfxVolume * masterVolume / 100.f);
        // 更新 BGM
        backgroundMusic.setVolume(musicVolume * masterVolume / 100.f);
    }
    void setMusicVolume(float v) {
        musicVolume = std::clamp(v, 0.f, 100.f);
        backgroundMusic.setVolume(musicVolume * masterVolume / 100.f);
    }
    void setSfxVolume(float v) {
        sfxVolume = std::clamp(v, 0.f, 100.f);
        for (auto& [k, snd] : sounds)
            snd->setVolume(sfxVolume * masterVolume / 100.f);
    }

    float getMasterVolume() const { return masterVolume; }
    float getMusicVolume()  const { return musicVolume; }
    float getSfxVolume()    const { return sfxVolume; }

private:
    AudioManager()
        : masterVolume(100.f), musicVolume(70.f), sfxVolume(80.f) {
    }

    std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer>> soundBuffers;
    std::unordered_map<std::string, std::unique_ptr<sf::Sound>>       sounds;
    std::unordered_map<std::string, std::string>                     musicFiles;
    sf::Music                                                         backgroundMusic;

    float masterVolume;
    float musicVolume;
    float sfxVolume;

    // 记录当前播放的 BGM 名称 —— 
    std::string currentMusicName;
};
