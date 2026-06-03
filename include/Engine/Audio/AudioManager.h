#pragma once
#include "Engine/Common.h"
#include <functional>

class AudioManager {
public:
    AudioManager();

    void PlayMoveSound();
    void PlayCaptureSound();
    void PlayWinSound();
    void PlayLoseSound();
    void PlayDrawSound();
    void PlayClickSound();

private:
    void GenerateSounds();

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
    bool soundsLoaded;
};
