#ifndef AUDIOPLAYER_HPP
#define AUDIOPLAYER_HPP

#include <string>
#include <memory>
#include "Song.hpp"

// Forward declaration for FMOD types
namespace FMOD {
    class System;
    class Sound;
    class Channel;
}

enum class PlaybackState {
    STOPPED,
    PLAYING,
    PAUSED
};

class AudioPlayer {
public:
    AudioPlayer();
    ~AudioPlayer();
    
    bool initialize();
    void cleanup();
    
    bool loadSong(const Song& song);
    void play();
    void pause();
    void resume();
    void stop();
    
    PlaybackState getState() const;
    bool isPlaying() const;
    bool isPaused() const;
    
    void setVolume(float volume); // 0.0 to 1.0
    float getVolume() const;
    
    // Position in milliseconds
    unsigned int getPosition() const;
    unsigned int getLength() const;
    void setPosition(unsigned int positionMs);
    
    std::string getCurrentSongName() const;
    
    void update(); // Call this regularly to update FMOD
    
private:
    FMOD::System* fmodSystem;
    FMOD::Sound* currentSound;
    FMOD::Channel* currentChannel;
    
    Song currentSong;
    PlaybackState state;
    float volume;
    
    bool initializeFMOD();
};

#endif