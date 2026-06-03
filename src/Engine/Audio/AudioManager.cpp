#include "Engine/Audio/AudioManager.h"
#include <cmath>
#include <vector>

AudioManager::AudioManager() : soundsLoaded(false) {
    GenerateSounds();
}

void AudioManager::GenerateSounds() {
    const int sampleRate = 44100;
    const float PI = 3.14159265f;
    
    std::vector<sf::Int16> moveSamples(sampleRate * 0.1f);
    for (size_t i = 0; i < moveSamples.size(); i++) {
        float t = (float)i / sampleRate;
        float envelope = 1.0f - t / 0.1f;
        float wave = std::sin(2 * PI * 800 * t) * 0.3f;
        moveSamples[i] = (sf::Int16)(wave * envelope * 32767);
    }
    moveSoundBuffer.loadFromSamples(&moveSamples[0], moveSamples.size(), 1, sampleRate);
    moveSound.setBuffer(moveSoundBuffer);
    
    std::vector<sf::Int16> captureSamples(sampleRate * 0.15f);
    for (size_t i = 0; i < captureSamples.size(); i++) {
        float t = (float)i / sampleRate;
        float envelope = 1.0f - t / 0.15f;
        float wave = std::sin(2 * PI * 600 * t) * 0.4f + std::sin(2 * PI * 900 * t) * 0.3f;
        captureSamples[i] = (sf::Int16)(wave * envelope * 32767);
    }
    captureSoundBuffer.loadFromSamples(&captureSamples[0], captureSamples.size(), 1, sampleRate);
    captureSound.setBuffer(captureSoundBuffer);
    
    std::vector<sf::Int16> winSamples(sampleRate * 0.8f);
    for (size_t i = 0; i < winSamples.size(); i++) {
        float t = (float)i / sampleRate;
        float envelope = (t < 0.1f) ? t / 0.1f : (t > 0.7f ? (0.8f - t) / 0.1f : 1.0f);
        float freq = 523.25f + (t / 0.8f) * 261.63f;
        float wave = std::sin(2 * PI * freq * t) * 0.3f + std::sin(2 * PI * freq * 1.5f * t) * 0.2f;
        winSamples[i] = (sf::Int16)(wave * envelope * 32767);
    }
    winSoundBuffer.loadFromSamples(&winSamples[0], winSamples.size(), 1, sampleRate);
    winSound.setBuffer(winSoundBuffer);
    
    std::vector<sf::Int16> loseSamples(sampleRate * 0.6f);
    for (size_t i = 0; i < loseSamples.size(); i++) {
        float t = (float)i / sampleRate;
        float envelope = (t < 0.05f) ? t / 0.05f : (t > 0.5f ? (0.6f - t) / 0.1f : 1.0f);
        float freq = 440.0f - (t / 0.6f) * 110.0f;
        float wave = std::sin(2 * PI * freq * t) * 0.35f;
        loseSamples[i] = (sf::Int16)(wave * envelope * 32767);
    }
    loseSoundBuffer.loadFromSamples(&loseSamples[0], loseSamples.size(), 1, sampleRate);
    loseSound.setBuffer(loseSoundBuffer);
    
    std::vector<sf::Int16> drawSamples(sampleRate * 0.5f);
    for (size_t i = 0; i < drawSamples.size(); i++) {
        float t = (float)i / sampleRate;
        float envelope = (t < 0.05f) ? t / 0.05f : (t > 0.4f ? (0.5f - t) / 0.1f : 1.0f);
        float wave = std::sin(2 * PI * 440 * t) * 0.25f + std::sin(2 * PI * 554.37f * t) * 0.25f;
        drawSamples[i] = (sf::Int16)(wave * envelope * 32767);
    }
    drawSoundBuffer.loadFromSamples(&drawSamples[0], drawSamples.size(), 1, sampleRate);
    drawSound.setBuffer(drawSoundBuffer);
    
    std::vector<sf::Int16> clickSamples(sampleRate * 0.05f);
    for (size_t i = 0; i < clickSamples.size(); i++) {
        float t = (float)i / sampleRate;
        float envelope = 1.0f - t / 0.05f;
        float wave = std::sin(2 * PI * 1200 * t) * 0.2f;
        clickSamples[i] = (sf::Int16)(wave * envelope * 32767);
    }
    clickSoundBuffer.loadFromSamples(&clickSamples[0], clickSamples.size(), 1, sampleRate);
    clickSound.setBuffer(clickSoundBuffer);
    
    soundsLoaded = true;
}

void AudioManager::PlayMoveSound() { if (soundsLoaded) moveSound.play(); }
void AudioManager::PlayCaptureSound() { if (soundsLoaded) captureSound.play(); }
void AudioManager::PlayWinSound() { if (soundsLoaded) winSound.play(); }
void AudioManager::PlayLoseSound() { if (soundsLoaded) loseSound.play(); }
void AudioManager::PlayDrawSound() { if (soundsLoaded) drawSound.play(); }
void AudioManager::PlayClickSound() { if (soundsLoaded) clickSound.play(); }
