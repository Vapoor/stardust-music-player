// Enhanced audioPlayer.cpp with timer support
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
      state(PlaybackState::STOPPED), volume(1.0f), songFinished(false),
      songStartTime(std::chrono::steady_clock::now()), songLengthMs(0) {
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
    songFinished = false;
    songLengthMs = 0;
    
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
    
    // Get song length
    unsigned int length = 0;
    FMOD_Sound_GetLength(currentSound, &length, FMOD_TIMEUNIT_MS);
    songLengthMs = length;
    
    std::cout << "Loaded: " << song.getDisplayName() << " (Length: " << formatTime(songLengthMs) << ")" << std::endl;
    return true;
#else
    // Simulate different song lengths for testing
    songLengthMs = 30000 + (song.id % 5) * 15000; // 30-90 seconds based on song ID
    std::cout << "Simulated loading: " << song.getDisplayName() << " (Length: " << formatTime(songLengthMs) << ")" << std::endl;
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
        songFinished = false;
        songStartTime = std::chrono::steady_clock::now();
        pausedDuration = std::chrono::milliseconds(0);
        FMOD_Channel_SetVolume(currentChannel, volume);
        std::cout << "Playing: " << currentSong.getDisplayName() << " | Length: " << formatTime(songLengthMs) << std::endl;
    } else {
        std::cout << "Failed to play song!" << std::endl;
    }
#else
    state = PlaybackState::PLAYING;
    songFinished = false;
    songStartTime = std::chrono::steady_clock::now();
    pausedDuration = std::chrono::milliseconds(0);
    std::cout << "Simulated playing: " << currentSong.getDisplayName() << " | Length: " << formatTime(songLengthMs) << std::endl;
#endif
}

void AudioPlayer::pause() {
#ifdef FMOD_AVAILABLE
    if (currentChannel && state == PlaybackState::PLAYING) {
        FMOD_Channel_SetPaused(currentChannel, 1);
        state = PlaybackState::PAUSED;
        pauseStartTime = std::chrono::steady_clock::now();
        std::cout << "Paused" << std::endl;
    }
#else
    if (state == PlaybackState::PLAYING) {
        state = PlaybackState::PAUSED;
        pauseStartTime = std::chrono::steady_clock::now();
        std::cout << "Simulated pause" << std::endl;
    }
#endif
}

void AudioPlayer::resume() {
#ifdef FMOD_AVAILABLE
    if (currentChannel && state == PlaybackState::PAUSED) {
        FMOD_Channel_SetPaused(currentChannel, 0);
        state = PlaybackState::PLAYING;
        
        // Add the paused duration to total paused time
        auto pauseDuration = std::chrono::steady_clock::now() - pauseStartTime;
        pausedDuration += std::chrono::duration_cast<std::chrono::milliseconds>(pauseDuration);
        
        std::cout << "Resumed" << std::endl;
    }
#else
    if (state == PlaybackState::PAUSED) {
        state = PlaybackState::PLAYING;
        
        // Add the paused duration to total paused time
        auto pauseDuration = std::chrono::steady_clock::now() - pauseStartTime;
        pausedDuration += std::chrono::duration_cast<std::chrono::milliseconds>(pauseDuration);
        
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
        songFinished = false;
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
    return getCurrentPlaybackPosition();
}

unsigned int AudioPlayer::getLength() const {
    return songLengthMs;
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

unsigned int AudioPlayer::getCurrentPlaybackPosition() const {
    if (state == PlaybackState::STOPPED) {
        return 0;
    }
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - songStartTime);
    
    // Subtract paused duration
    auto actualElapsed = elapsed - pausedDuration;
    
    // If currently paused, don't add current pause time
    if (state == PlaybackState::PAUSED) {
        auto currentPauseDuration = std::chrono::duration_cast<std::chrono::milliseconds>(now - pauseStartTime);
        actualElapsed -= currentPauseDuration;
    }
    
    return static_cast<unsigned int>(std::max(0LL, actualElapsed.count()));
}

unsigned int AudioPlayer::getRemainingTime() const {
    if (songLengthMs == 0) return 0;
    
    unsigned int currentPos = getCurrentPlaybackPosition();
    if (currentPos >= songLengthMs) return 0;
    
    return songLengthMs - currentPos;
}

std::string AudioPlayer::formatTime(unsigned int milliseconds) const {
    unsigned int totalSeconds = milliseconds / 1000;
    unsigned int minutes = totalSeconds / 60;
    unsigned int seconds = totalSeconds % 60;
    
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%u:%02u", minutes, seconds);
    return std::string(buffer);
}

std::string AudioPlayer::getProgressString() const {
    if (state == PlaybackState::STOPPED) {
        return "Stopped";
    }
    
    unsigned int currentPos = getCurrentPlaybackPosition();
    unsigned int remaining = getRemainingTime();
    
    return formatTime(currentPos) + " / " + formatTime(songLengthMs) + " (Remaining: " + formatTime(remaining) + ")";
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
                state = PlaybackState::STOPPED;
                songFinished = true;
                currentChannel = nullptr;
                std::cout << "Song finished naturally" << std::endl;
            }
        }
    }
#else
    // Simulate song finishing based on our timer
    if (state == PlaybackState::PLAYING && songLengthMs > 0) {
        unsigned int currentPos = getCurrentPlaybackPosition();
        
        // Add 1 second buffer as requested
        if (currentPos >= songLengthMs + 1000) {
            state = PlaybackState::STOPPED;
            songFinished = true;
            std::cout << "Simulated song finished naturally" << std::endl;
        }
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