// ===================================================================
// AudioManager.cpp — 音效管理器的实现
// 不使用外部音频文件，而是通过数学公式生成音频波形数据
// 使用正弦波（sine wave）+ 包络（envelope）来模拟各种音效
// ===================================================================

#include "Engine/Audio/AudioManager.h"
#include <cmath>
#include <vector>

// 构造函数：初始化音效未加载，然后调用 GenerateSounds() 生成所有音效
AudioManager::AudioManager() : soundsLoaded(false) {
    GenerateSounds();
}

// ==================== 生成所有音效 ====================
// 原理：创建一段音频采样数据（16位整型数组），然后用 SFML 加载播放
// 每个音效由 频率(frequency) × 包络(envelope) 组成
// 包络 = 音量随时间的衰减曲线，用于避免爆音
void AudioManager::GenerateSounds() {
    const int sampleRate = 44100;                    // 采样率 44100Hz（CD 音质）
    const float PI = 3.14159265f;

    // —— 走子音效（0.1秒，800Hz，短促的"滴"声）——
    std::vector<sf::Int16> moveSamples(sampleRate * 0.1f);
    for (size_t i = 0; i < moveSamples.size(); i++) {
        float t = (float)i / sampleRate;             // 当前时间（秒）
        float envelope = 1.0f - t / 0.1f;            // 线性衰减包络（从1降到0）
        float wave = std::sin(2 * PI * 800 * t) * 0.3f;  // 800Hz 正弦波，音量30%
        moveSamples[i] = (sf::Int16)(wave * envelope * 32767); // 转为16位整数
    }
    moveSoundBuffer.loadFromSamples(&moveSamples[0], moveSamples.size(), 1, sampleRate);
    moveSound.setBuffer(moveSoundBuffer);

    // —— 吃子音效（0.15秒，600+900Hz 双音，更有"打击感"）——
    std::vector<sf::Int16> captureSamples(sampleRate * 0.15f);
    for (size_t i = 0; i < captureSamples.size(); i++) {
        float t = (float)i / sampleRate;
        float envelope = 1.0f - t / 0.15f;
        // 两个频率叠加：600Hz（低沉）+ 900Hz（清脆）
        float wave = std::sin(2 * PI * 600 * t) * 0.4f + std::sin(2 * PI * 900 * t) * 0.3f;
        captureSamples[i] = (sf::Int16)(wave * envelope * 32767);
    }
    captureSoundBuffer.loadFromSamples(&captureSamples[0], captureSamples.size(), 1, sampleRate);
    captureSound.setBuffer(captureSoundBuffer);

    // —— 胜利音效（0.8秒，频率从 523Hz 上升到 785Hz，欢快上升感）——
    std::vector<sf::Int16> winSamples(sampleRate * 0.8f);
    for (size_t i = 0; i < winSamples.size(); i++) {
        float t = (float)i / sampleRate;
        // 两端淡入淡出中间保持的包络
        float envelope = (t < 0.1f) ? t / 0.1f : (t > 0.7f ? (0.8f - t) / 0.1f : 1.0f);
        // 频率从 523.25Hz(C5) 逐渐升至 784.88Hz(G5)，模拟"胜利号角"
        float freq = 523.25f + (t / 0.8f) * 261.63f;
        float wave = std::sin(2 * PI * freq * t) * 0.3f + std::sin(2 * PI * freq * 1.5f * t) * 0.2f;
        winSamples[i] = (sf::Int16)(wave * envelope * 32767);
    }
    winSoundBuffer.loadFromSamples(&winSamples[0], winSamples.size(), 1, sampleRate);
    winSound.setBuffer(winSoundBuffer);

    // —— 失败音效（0.6秒，频率从 440Hz 下降到 330Hz，低沉下降感）——
    std::vector<sf::Int16> loseSamples(sampleRate * 0.6f);
    for (size_t i = 0; i < loseSamples.size(); i++) {
        float t = (float)i / sampleRate;
        float envelope = (t < 0.05f) ? t / 0.05f : (t > 0.5f ? (0.6f - t) / 0.1f : 1.0f);
        // 频率逐渐下降，模拟"沮丧"的感觉
        float freq = 440.0f - (t / 0.6f) * 110.0f;
        float wave = std::sin(2 * PI * freq * t) * 0.35f;
        loseSamples[i] = (sf::Int16)(wave * envelope * 32767);
    }
    loseSoundBuffer.loadFromSamples(&loseSamples[0], loseSamples.size(), 1, sampleRate);
    loseSound.setBuffer(loseSoundBuffer);

    // —— 和棋音效（0.5秒，440+554Hz 双音，平稳的和弦）——
    std::vector<sf::Int16> drawSamples(sampleRate * 0.5f);
    for (size_t i = 0; i < drawSamples.size(); i++) {
        float t = (float)i / sampleRate;
        float envelope = (t < 0.05f) ? t / 0.05f : (t > 0.4f ? (0.5f - t) / 0.1f : 1.0f);
        // 两个频率构成大三度和弦，听起来"平稳和谐"
        float wave = std::sin(2 * PI * 440 * t) * 0.25f + std::sin(2 * PI * 554.37f * t) * 0.25f;
        drawSamples[i] = (sf::Int16)(wave * envelope * 32767);
    }
    drawSoundBuffer.loadFromSamples(&drawSamples[0], drawSamples.size(), 1, sampleRate);
    drawSound.setBuffer(drawSoundBuffer);

    // —— 按钮点击音效（0.05秒，1200Hz，极短的"咔嗒"声）——
    std::vector<sf::Int16> clickSamples(sampleRate * 0.05f);
    for (size_t i = 0; i < clickSamples.size(); i++) {
        float t = (float)i / sampleRate;
        float envelope = 1.0f - t / 0.05f;
        float wave = std::sin(2 * PI * 1200 * t) * 0.2f;
        clickSamples[i] = (sf::Int16)(wave * envelope * 32767);
    }
    clickSoundBuffer.loadFromSamples(&clickSamples[0], clickSamples.size(), 1, sampleRate);
    clickSound.setBuffer(clickSoundBuffer);

    soundsLoaded = true;                             // 标记音效已就绪
}

// —— 播放方法 ——
// 每个方法先检查音效是否加载完毕，然后调用 play() 播放
void AudioManager::PlayMoveSound() { if (soundsLoaded) moveSound.play(); }
void AudioManager::PlayCaptureSound() { if (soundsLoaded) captureSound.play(); }
void AudioManager::PlayWinSound() { if (soundsLoaded) winSound.play(); }
void AudioManager::PlayLoseSound() { if (soundsLoaded) loseSound.play(); }
void AudioManager::PlayDrawSound() { if (soundsLoaded) drawSound.play(); }
void AudioManager::PlayClickSound() { if (soundsLoaded) clickSound.play(); }
