// NOTE: This implementation requires FMOD library
// Download FMOD from: https://www.fmod.com/download
// Add fmod.dll to your project directory and link against fmod_vc.lib

#include "../headers/audioPlayer.hpp"
#include <iostream>
#include <chrono>

// Include FMOD headers - you'll need to download and include these
#ifdef FMOD_AVAILABLE
#include "fmod.h"
#include "fmod_errors.h"
#endif

AudioPlayer::AudioPlayer() 
    : fmodSystem(nullptr), currentSound(nullptr), currentChannel(nullptr),
      state(PlaybackState::STOPPED), volume(1.0f), songFinished(false) {
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
        FMOD_Sound_Release(currentSound);
        currentSound = nullptr;
    }
    
    if (fmodSystem) {
        FMOD_System_Release(fmodSystem);
        fmodSystem = nullptr;
    }
#endif
}

bool AudioPlayer::loadSong(const Song& song) {
    currentSong = song;
    songFinished = false;  // Reset finished flag
    
#ifdef FMOD_AVAILABLE
    if (!fmodSystem) {
        std::cout << "FMOD system not initialized!" << std::endl;
        return false;
    }
    
    // Release previous sound
    if (currentSound) {
        FMOD_Sound_Release(currentSound);
        currentSound = nullptr;
    }
    
    // Load new sound
    FMOD_RESULT result = FMOD_System_CreateSound(fmodSystem, song.filePath.c_str(), FMOD_DEFAULT, 0, &currentSound);
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
    
    FMOD_RESULT result = FMOD_System_PlaySound(fmodSystem, currentSound, 0, 0, &currentChannel);
    if (result == FMOD_OK) {
        state = PlaybackState::PLAYING;
        songFinished = false;  // Reset finished flag when starting playback
        FMOD_Channel_SetVolume(currentChannel, volume);
        std::cout << "Playing: " << currentSong.getDisplayName() << std::endl;
    } else {
        std::cout << "Failed to play song!" << std::endl;
    }
#else
    state = PlaybackState::PLAYING;
    songFinished = false;
    std::cout << "Simulated playing: " << currentSong.getDisplayName() << std::endl;
#endif
}

void AudioPlayer::pause() {
#ifdef FMOD_AVAILABLE
    if (currentChannel && state == PlaybackState::PLAYING) {
        FMOD_Channel_SetPaused(currentChannel, 1);
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
        FMOD_Channel_SetPaused(currentChannel, 0);
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
        FMOD_Channel_Stop(currentChannel);
        currentChannel = nullptr;
        state = PlaybackState::STOPPED;
        songFinished = false;  // Manual stop, not natural finish
        std::cout << "Stopped" << std::endl;
    }
#else
    state = PlaybackState::STOPPED;
    songFinished = false;
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

bool AudioPlayer::hasFinished() const {
    return songFinished;
}

void AudioPlayer::setVolume(float vol) {
    volume = vol;
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    
#ifdef FMOD_AVAILABLE
    if (currentChannel) {
        FMOD_Channel_SetVolume(currentChannel, volume);
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
        FMOD_Channel_GetPosition(currentChannel, &position, FMOD_TIMEUNIT_MS);
        return position;
    }
#endif
    return 0;
}

unsigned int AudioPlayer::getLength() const {
#ifdef FMOD_AVAILABLE
    if (currentSound) {
        unsigned int length = 0;
        FMOD_Sound_GetLength(currentSound, &length, FMOD_TIMEUNIT_MS);
        return length;
    }
#endif
    return 0;
}

void AudioPlayer::setPosition(unsigned int positionMs) {
#ifdef FMOD_AVAILABLE
    if (currentChannel) {
        FMOD_Channel_SetPosition(currentChannel, positionMs, FMOD_TIMEUNIT_MS);
    }
#endif
}

std::string AudioPlayer::getCurrentSongName() const {
    return currentSong.getDisplayName();
}

void AudioPlayer::update() {
#ifdef FMOD_AVAILABLE
    if (fmodSystem) {
        FMOD_System_Update(fmodSystem);
        
        // Check if song finished playing
        if (currentChannel && state == PlaybackState::PLAYING) {
            FMOD_BOOL isPlaying = 0;
            FMOD_Channel_IsPlaying(currentChannel, &isPlaying);
            if (!isPlaying) {
                // Song finished naturally
                state = PlaybackState::STOPPED;
                songFinished = true;  // Mark as finished for auto-advance
                currentChannel = nullptr;
                std::cout << "Song finished naturally" << std::endl;
            }
        }
    }
#else
    // Simulate song finishing after 10 seconds for testing
    static auto lastPlayTime = std::chrono::steady_clock::now();
    if (state == PlaybackState::PLAYING) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastPlayTime).count();
        if (elapsed >= 10) {  // Simulate 10-second songs for testing
            state = PlaybackState::STOPPED;
            songFinished = true;
            std::cout << "Simulated song finished" << std::endl;
        }
    } else {
        lastPlayTime = std::chrono::steady_clock::now();
    }
#endif
}

#ifdef FMOD_AVAILABLE
bool AudioPlayer::initializeFMOD() {
    FMOD_RESULT result = FMOD_System_Create(&fmodSystem, FMOD_VERSION);
    if (result != FMOD_OK) {
        std::cout << "Failed to create FMOD system!" << std::endl;
        return false;
    }
    
    result = FMOD_System_Init(fmodSystem, 32, FMOD_INIT_NORMAL, 0);
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