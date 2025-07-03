#ifndef AUDIOPLAYER_HPP
#define AUDIOPLAYER_HPP

#include <string>
#include <memory>
#include "song.hpp"

// Forward declaration for FMOD types
#ifdef FMOD_AVAILABLE
struct FMOD_SYSTEM;
struct FMOD_SOUND;
struct FMOD_CHANNEL;
typedef struct FMOD_SYSTEM FMOD_SYSTEM;
typedef struct FMOD_SOUND FMOD_SOUND;
typedef struct FMOD_CHANNEL FMOD_CHANNEL;
#endif

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
    bool hasFinished() const;  // New method to check if song finished
    
    void setVolume(float volume); // 0.0 to 1.0
    float getVolume() const;
    
    // Position in milliseconds
    unsigned int getPosition() const;
    unsigned int getLength() const;
    void setPosition(unsigned int positionMs);
    
    std::string getCurrentSongName() const;
    
    void update(); // Call this regularly to update FMOD
    
private:
#ifdef FMOD_AVAILABLE
    FMOD_SYSTEM* fmodSystem;
    FMOD_SOUND* currentSound;
    FMOD_CHANNEL* currentChannel;
#else
    void* fmodSystem;
    void* currentSound;
    void* currentChannel;
#endif
    
    Song currentSong;
    PlaybackState state;
    float volume;
    bool songFinished;  // Track if current song finished naturally
    
    bool initializeFMOD();
};

#endif