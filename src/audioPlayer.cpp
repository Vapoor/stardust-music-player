// NOTE: This implementation requires FMOD library
// Download FMOD from: https://www.fmod.com/download
// Add fmod.dll to your project directory and link against fmod_vc.lib

#include "../headers/audioPlayer.hpp"
#include <iostream>

// Include FMOD headers - you'll need to download and include these
#ifdef FMOD_AVAILABLE
#include "fmod.hpp"
#include "fmod_errors.h"
#endif

AudioPlayer::AudioPlayer() 
    : fmodSystem(nullptr), currentSound(nullptr), currentChannel(nullptr),
      state(PlaybackState::STOPPED), volume(1.0f) {
}

AudioPlayer::~AudioPlayer() {
    cleanup();
}

bool AudioPlayer::initialize() {
#ifdef FMOD_AVAILABLE
    return initializeFMOD();
#else
    std::cout << "FMOD not available. Audio playback will be simulated." << std::endl;
    return true;
#endif
}

void AudioPlayer::cleanup() {
#ifdef FMOD_AVAILABLE
    if (currentSound) {
        currentSound->release();
        currentSound = nullptr;
    }
    
    if (fmodSystem) {
        fmodSystem->release();
        fmodSystem = nullptr;
    }
#endif
}

bool AudioPlayer::loadSong(const Song& song) {
    currentSong = song;
    
#ifdef FMOD_AVAILABLE
    if (!fmodSystem) {
        std::cout << "FMOD system not initialized!" << std::endl;
        return false;
    }
    
    // Release previous sound
    if (currentSound) {
        currentSound->release();
        currentSound = nullptr;
    }
    
    // Load new sound
    FMOD_RESULT result = fmodSystem->createSound(song.filePath.c_str(), FMOD_DEFAULT, 0, &currentSound);
    if (result != FMOD_OK) {
        std::cout << "Failed to load song: " << song.filePath << std::endl;
        return false;
    }
    
    std::cout << "Loaded: " << song.getDisplayName() << std::endl;
    return true;
#else
    std::cout << "Simulated loading: " << song.getDisplayName() << std::endl;
    return true;
#endif
}

void AudioPlayer::play() {
#ifdef FMOD_AVAILABLE
    if (!currentSound || !fmodSystem) {
        std::cout << "No song loaded or FMOD not initialized!" << std::endl;
        return;
    }
    
    FMOD_RESULT result = fmodSystem->playSound(currentSound, 0, false, &currentChannel);
    if (result == FMOD_OK) {
        state = PlaybackState::PLAYING;
        currentChannel->setVolume(volume);
        std::cout << "Playing: " << currentSong.getDisplayName() << std::endl;
    } else {
        std::cout << "Failed to play song!" << std::endl;
    }
#else
    state = PlaybackState::PLAYING;
    std::cout << "Simulated playing: " << currentSong.getDisplayName() << std::endl;
#endif
}

void AudioPlayer::pause() {
#ifdef FMOD_AVAILABLE
    if (currentChannel && state == PlaybackState::PLAYING) {
        currentChannel->setPaused(true);
        state = PlaybackState::PAUSED;
        std::cout << "Paused" << std::endl;
    }
#else
    if (state == PlaybackState::PLAYING) {
        state = PlaybackState::PAUSED;
        std::cout << "Simulated pause" << std::endl;
    }
#endif
}

void AudioPlayer::resume() {
#ifdef FMOD_AVAILABLE
    if (currentChannel && state == PlaybackState::PAUSED) {
        currentChannel->setPaused(false);
        state = PlaybackState::PLAYING;
        std::cout << "Resumed" << std::endl;
    }
#else
    if (state == PlaybackState::PAUSED) {
        state = PlaybackState::PLAYING;
        std::cout << "Simulated resume" << std::endl;
    }
#endif
}

void AudioPlayer::stop() {
#ifdef FMOD_AVAILABLE
    if (currentChannel) {
        currentChannel->stop();
        currentChannel = nullptr;
        state = PlaybackState::STOPPED;
        std::cout << "Stopped" << std::endl;
    }
#else
    state = PlaybackState::STOPPED;
    std::cout << "Simulated stop" << std::endl;
#endif
}

PlaybackState AudioPlayer::getState() const {
    return state;
}

bool AudioPlayer::isPlaying() const {
    return state == PlaybackState::PLAYING;
}

bool AudioPlayer::isPaused() const {
    return state == PlaybackState::PAUSED;
}

void AudioPlayer::setVolume(float vol) {
    volume = vol;
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    
#ifdef FMOD_AVAILABLE
    if (currentChannel) {
        currentChannel->setVolume(volume);
    }
#endif
}

float AudioPlayer::getVolume() const {
    return volume;
}

unsigned int AudioPlayer::getPosition() const {
#ifdef FMOD_AVAILABLE
    if (currentChannel) {
        unsigned int position = 0;
        currentChannel->getPosition(&position, FMOD_TIMEUNIT_MS);
        return position;
    }
#endif
    return 0;
}

unsigned int AudioPlayer::getLength() const {
#ifdef FMOD_AVAILABLE
    if (currentSound) {
        unsigned int length = 0;
        currentSound->getLength(&length, FMOD_TIMEUNIT_MS);
        return length;
    }
#endif
    return 0;
}

void AudioPlayer::setPosition(unsigned int positionMs) {
#ifdef FMOD_AVAILABLE
    if (currentChannel) {
        currentChannel->setPosition(positionMs, FMOD_TIMEUNIT_MS);
    }
#endif
}

std::string AudioPlayer::getCurrentSongName() const {
    return currentSong.getDisplayName();
}

void AudioPlayer::update() {
#ifdef FMOD_AVAILABLE
    if (fmodSystem) {
        fmodSystem->update();
        
        // Check if song finished playing
        if (currentChannel && state == PlaybackState::PLAYING) {
            bool isPlaying = false;
            currentChannel->isPlaying(&isPlaying);
            if (!isPlaying) {
                state = PlaybackState::STOPPED;
                currentChannel = nullptr;
            }
        }
    }
#endif
}

#ifdef FMOD_AVAILABLE
bool AudioPlayer::initializeFMOD() {
    FMOD_RESULT result = FMOD::System_Create(&fmodSystem);
    if (result != FMOD_OK) {
        std::cout << "Failed to create FMOD system!" << std::endl;
        return false;
    }
    
    result = fmodSystem->init(32, FMOD_INIT_NORMAL, 0);
    if (result != FMOD_OK) {
        std::cout << "Failed to initialize FMOD system!" << std::endl;
        return false;
    }
    
    std::cout << "FMOD initialized successfully!" << std::endl;
    return true;
}
#else
bool AudioPlayer::initializeFMOD() {
    return true;
}
#endif