#ifndef DISCORDPRESENCE_HPP
#define DISCORDPRESENCE_HPP

#include <string>
#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>

// Forward declaration
struct IDiscordCore;
struct IDiscordActivityManager;

enum class PresenceState {
    IDLE,
    BROWSING,
    PLAYING,
    PAUSED
};

struct PresenceData {
    PresenceState state;
    std::string songName;
    std::string artist;
    std::string playlistName;
    int totalSongs;
    bool isInPlaylist;
    
    PresenceData() : state(PresenceState::IDLE), totalSongs(0), isInPlaylist(false) {}
};

class RichPresence {
public:
    RichPresence();
    ~RichPresence();
    
    bool initialize();
    void cleanup();
    
    // State setters - these just update the current state
    void setSongPlaying(const std::string& songName, const std::string& artist, bool isInPlaylist = false, const std::string& playlistName = "");
    void setSongPaused(const std::string& songName, const std::string& artist, bool isInPlaylist = false, const std::string& playlistName = "");
    void setBrowsingState(int totalSongs);
    void setIdleState();
    
    // Start/stop the refresh loop
    void startRefreshLoop();
    void stopRefreshLoop();
    
private:
    IDiscordCore* core;
    IDiscordActivityManager* activities;
    bool isInitialized;
    bool isConnected;
    
    // State management
    PresenceData currentState;
    std::mutex stateMutex;
    
    // Refresh loop
    std::atomic<bool> shouldRunLoop;
    std::thread refreshThread;
    std::chrono::seconds refreshInterval;
    
    // Last applied state (to avoid redundant updates)
    std::string lastAppliedDetails;
    std::string lastAppliedState;
    std::string lastAppliedSmallImage;
    
    // Core Discord functions
    void setActivity(const std::string& details, const std::string& state,
                    const std::string& largeImageKey, const std::string& largeImageText,
                    const std::string& smallImageKey = "", const std::string& smallImageText = "");
    
    void refreshLoop();
    void applyCurrentState();
    void checkConnection();
    
    // State to Discord conversion
    void getDiscordStrings(const PresenceData& data, std::string& details, std::string& state, 
                          std::string& smallImageKey, std::string& smallImageText);
};

#endif