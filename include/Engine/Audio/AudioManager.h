#pragma once

// ===================================================================
// AudioManager.h — 音效管理器
// 使用程序生成的音效（不依赖外部音频文件）
// 通过数学公式生成不同频率/时长的正弦波来模拟各种音效
// ===================================================================

#include "Engine/Common.h"
#include <functional>

class AudioManager {
public:
    AudioManager();

    // —— 播放各种音效 ——
    void PlayMoveSound();            // 走子音效（轻短的滴答声）
    void PlayCaptureSound();         // 吃子音效（较重的双音）
    void PlayWinSound();             // 胜利音效（上升的欢快旋律）
    void PlayLoseSound();            // 失败音效（下降的低沉旋律）
    void PlayDrawSound();            // 和棋音效（平稳的和弦）
    void PlayClickSound();           // 按钮点击音效（极短的咔嗒声）

private:
    // 生成所有音效（在构造函数中调用一次）
    void GenerateSounds();

    // —— SFML 音频对象 ——
    // SoundBuffer 存储音频数据（波形采样）
    // Sound 用于播放 SoundBuffer 中的音频
    sf::SoundBuffer moveSoundBuffer;
    sf::SoundBuffer captureSoundBuffer;
    sf::SoundBuffer winSoundBuffer;
    sf::SoundBuffer loseSoundBuffer;
    sf::SoundBuffer drawSoundBuffer;
    sf::SoundBuffer clickSoundBuffer;
    sf::Sound moveSound;
    sf::Sound captureSound;
    sf::Sound winSound;
    sf::Sound loseSound;
    sf::Sound drawSound;
    sf::Sound clickSound;
    bool soundsLoaded;               // 音效是否已加载完毕
};
