#include "../headers/discordPresence.hpp"
#include <iostream>
#include <cstring>
#include <thread>

// Discord Game SDK includes (only if available)
#ifdef DISCORD_AVAILABLE
#include "discord.h"
#endif

// Your Discord Application ID
const int64_t APPLICATION_ID = 0; //1389957827065548892;

RichPresence::RichPresence() 
    : core(nullptr), activities(nullptr), isInitialized(false), isConnected(false),
      shouldRunLoop(false), refreshInterval(7) { // 7 second refresh interval
}

RichPresence::~RichPresence() {
    stopRefreshLoop();
    cleanup();
}

bool RichPresence::initialize() {
#ifdef DISCORD_AVAILABLE
    std::cout << "Attempting to initialize Discord Rich Presence..." << std::endl;
    
    // Give Discord some time to be ready
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    try {
        DiscordCreateParams params{};
        params.client_id = APPLICATION_ID;
        params.flags = DiscordCreateFlags_Default;
        
        EDiscordResult result = DiscordCreate(DISCORD_VERSION, &params, &core);
        
        if (result != DiscordResult_Ok) {
            std::cout << "Failed to initialize Discord SDK. Error code: " << static_cast<int>(result) << std::endl;
            std::cout << "Make sure:" << std::endl;
            std::cout << "1. Discord desktop app is running" << std::endl;
            std::cout << "2. Your Discord app is properly configured" << std::endl;
            return false;
        }
        
        activities = core->get_activity_manager(core);
        if (!activities) {
            std::cout << "Failed to get Discord activity manager" << std::endl;
            cleanup();
            return false;
        }
        
        isInitialized = true;
        std::cout << "Discord Rich Presence initialized successfully!" << std::endl;
        
        // Set initial idle state
        setIdleState();
        
        // Start the refresh loop
        startRefreshLoop();
        
        return true;
    } catch (const std::exception& e) {
        std::cout << "Discord initialization error: " << e.what() << std::endl;
        return false;
    }
#else
    std::cout << "Discord Rich Presence not available (Discord SDK not included)" << std::endl;
    std::cout << "Status updates will be shown in console instead." << std::endl;
    isInitialized = true; // Mark as "initialized" for console simulation
    setIdleState();
    startRefreshLoop();
    return true;
#endif
}

void RichPresence::cleanup() {
#ifdef DISCORD_AVAILABLE
    if (core) {
        // Clear activity before cleanup
        if (activities) {
            activities->clear_activity(activities, nullptr, [](void* data, EDiscordResult result) {
                (void)data; // Suppress unused parameter warning
            });
        }
        
        core->destroy(core);
        core = nullptr;
        activities = nullptr;
    }
#endif
    isInitialized = false;
}

void RichPresence::setSongPlaying(const std::string& songName, const std::string& artist, bool isInPlaylist, const std::string& playlistName) {
    std::lock_guard<std::mutex> lock(stateMutex);
    currentState.state = PresenceState::PLAYING;
    currentState.songName = songName;
    currentState.artist = artist;
    currentState.isInPlaylist = isInPlaylist;
    currentState.playlistName = playlistName;
    
    //std::cout << "State updated: Playing - " << artist << " - " << songName << std::endl;
}

void RichPresence::setSongPaused(const std::string& songName, const std::string& artist, bool isInPlaylist, const std::string& playlistName) {
    std::lock_guard<std::mutex> lock(stateMutex);
    currentState.state = PresenceState::PAUSED;
    currentState.songName = songName;
    currentState.artist = artist;
    currentState.isInPlaylist = isInPlaylist;
    currentState.playlistName = playlistName;
    
    //std::cout << "State updated: Paused - " << artist << " - " << songName << std::endl;
}

void RichPresence::setBrowsingState(int totalSongs) {
    std::lock_guard<std::mutex> lock(stateMutex);
    currentState.state = PresenceState::BROWSING;
    currentState.totalSongs = totalSongs;
    currentState.songName.clear();
    currentState.artist.clear();
    currentState.playlistName.clear();
    currentState.isInPlaylist = false;
    
    //std::cout << "State updated: Browsing " << totalSongs << " songs" << std::endl;
}

void RichPresence::setIdleState() {
    std::lock_guard<std::mutex> lock(stateMutex);
    currentState.state = PresenceState::IDLE;
    currentState.songName.clear();
    currentState.artist.clear();
    currentState.playlistName.clear();
    currentState.isInPlaylist = false;
    currentState.totalSongs = 0;
    
    //std::cout << "State updated: Idle" << std::endl;
}

void RichPresence::startRefreshLoop() {
    if (shouldRunLoop.load()) {
        return; // Already running
    }
    
    shouldRunLoop = true;
    refreshThread = std::thread(&RichPresence::refreshLoop, this);
    std::cout << "Discord refresh loop started (every " << refreshInterval.count() << " seconds)" << std::endl;
}

void RichPresence::stopRefreshLoop() {
    if (shouldRunLoop.load()) {
        shouldRunLoop = false;
        if (refreshThread.joinable()) {
            refreshThread.join();
        }
        std::cout << "Discord refresh loop stopped" << std::endl;
    }
}

void RichPresence::refreshLoop() {
    while (shouldRunLoop.load()) {
        try {
            // Run Discord callbacks
#ifdef DISCORD_AVAILABLE
            if (isInitialized && core) {
                core->run_callbacks(core);
                checkConnection();
            }
#endif
            
            // Apply current state to Discord
            applyCurrentState();
            
            // Wait for next refresh, but check shouldRunLoop every 100ms for quick exit
            int totalWaitMs = refreshInterval.count() * 1000;
            while (totalWaitMs > 0 && shouldRunLoop.load()) {
                int sleepMs = (totalWaitMs > 100) ? 100 : totalWaitMs;
                std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
                totalWaitMs -= sleepMs;
            }
        } catch (const std::exception& e) {
            std::cout << "Error in refresh loop: " << e.what() << std::endl;
            
            // Even during error recovery, allow quick exit
            int totalWaitMs = 1000;
            while (totalWaitMs > 0 && shouldRunLoop.load()) {
                int sleepMs = (totalWaitMs > 100) ? 100 : totalWaitMs;
                std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
                totalWaitMs -= sleepMs;
            }
        }
    }
}

void RichPresence::applyCurrentState() {
    if (!isInitialized) {
        return;
    }
    
    std::string details, state, smallImageKey, smallImageText;
    
    // Get current state (thread-safe)
    {
        std::lock_guard<std::mutex> lock(stateMutex);
        getDiscordStrings(currentState, details, state, smallImageKey, smallImageText);
    }
    
    // Check if anything changed
    if (details == lastAppliedDetails && state == lastAppliedState && smallImageKey == lastAppliedSmallImage) {
        return; // No changes, skip update
    }
    
    // Update Discord (or console)
#ifdef DISCORD_AVAILABLE
    if (isConnected) {
        setActivity(details, state, "osu_logo", "osuListener by Vapor", smallImageKey, smallImageText);
        
        // Update our cache
        lastAppliedDetails = details;
        lastAppliedState = state;
        lastAppliedSmallImage = smallImageKey;
    }
#else
    // Console fallback
    std::cout << "\n[Discord Status] " << details << " | " << state << "\n";
    
    // Update our cache
    lastAppliedDetails = details;
    lastAppliedState = state;
    lastAppliedSmallImage = smallImageKey;
#endif
}

void RichPresence::getDiscordStrings(const PresenceData& data, std::string& details, std::string& state, 
                                    std::string& smallImageKey, std::string& smallImageText) {
    switch (data.state) {
        case PresenceState::PLAYING:
            details = "♪ " + data.artist + " - " + data.songName;
            if (data.isInPlaylist && !data.playlistName.empty()) {
                state = "Playlist: " + data.playlistName + " • Made by Vapor";
            } else {
                state = "Browsing osu! collection • Made by Vapor";
            }
            smallImageKey = "play_icon";
            smallImageText = "Playing";
            break;
            
        case PresenceState::PAUSED:
            details = "⏸ " + data.artist + " - " + data.songName;
            if (data.isInPlaylist && !data.playlistName.empty()) {
                state = "Playlist: " + data.playlistName + " • Made by Vapor";
            } else {
                state = "Browsing osu! collection • Made by Vapor";
            }
            smallImageKey = "pause_icon";
            smallImageText = "Paused";
            break;
            
        case PresenceState::BROWSING:
            details = "Exploring music library";
            state = "Browsing " + std::to_string(data.totalSongs) + " songs • Made by Vapor";
            smallImageKey = "";
            smallImageText = "";
            break;
            
        case PresenceState::IDLE:
        default:
            details = "Idle";
            state = "Ready to listen to osu! beats • Made by Vapor";
            smallImageKey = "";
            smallImageText = "";
            break;
    }
}

void RichPresence::setActivity(const std::string& details, const std::string& state,
                              const std::string& largeImageKey, const std::string& largeImageText,
                              const std::string& smallImageKey, const std::string& smallImageText) {
#ifdef DISCORD_AVAILABLE
    if (!isInitialized || !activities) {
        return;
    }
    
    DiscordActivity activity{};
    memset(&activity, 0, sizeof(activity)); // Clear all fields
    
    // Copy strings safely with bounds checking
    if (!details.empty()) {
        strncpy(activity.details, details.c_str(), sizeof(activity.details) - 1);
        activity.details[sizeof(activity.details) - 1] = '\0';
    }
    
    if (!state.empty()) {
        strncpy(activity.state, state.c_str(), sizeof(activity.state) - 1);
        activity.state[sizeof(activity.state) - 1] = '\0';
    }
    
    // Set images
    if (!largeImageKey.empty()) {
        strncpy(activity.assets.large_image, largeImageKey.c_str(), sizeof(activity.assets.large_image) - 1);
        activity.assets.large_image[sizeof(activity.assets.large_image) - 1] = '\0';
    }
    
    if (!largeImageText.empty()) {
        strncpy(activity.assets.large_text, largeImageText.c_str(), sizeof(activity.assets.large_text) - 1);
        activity.assets.large_text[sizeof(activity.assets.large_text) - 1] = '\0';
    }
    
    if (!smallImageKey.empty()) {
        strncpy(activity.assets.small_image, smallImageKey.c_str(), sizeof(activity.assets.small_image) - 1);
        activity.assets.small_image[sizeof(activity.assets.small_image) - 1] = '\0';
    }
    
    if (!smallImageText.empty()) {
        strncpy(activity.assets.small_text, smallImageText.c_str(), sizeof(activity.assets.small_text) - 1);
        activity.assets.small_text[sizeof(activity.assets.small_text) - 1] = '\0';
    }
    
    // Set activity type
    activity.type = DiscordActivityType_Listening;
    
    // Note: Buttons are not supported in this Discord SDK version
    // The presence will show without clickable buttons
    
    // Update activity
    activities->update_activity(activities, &activity, nullptr, [](void* data, EDiscordResult result) {
        (void)data; // Suppress unused parameter warning
        if (result != DiscordResult_Ok) {
            std::cout << "Failed to update Discord activity. Error: " << static_cast<int>(result);
            if (result == static_cast<EDiscordResult>(43)) {
                std::cout << " (Image not found - check asset names in Discord Developer Portal)";
            }
            std::cout << std::endl;
        }
    });
#endif
}

void RichPresence::checkConnection() {
#ifdef DISCORD_AVAILABLE
    if (!core) return;
    
    static auto lastCheck = std::chrono::system_clock::now();
    auto now = std::chrono::system_clock::now();
    auto timeSinceCheck = std::chrono::duration_cast<std::chrono::seconds>(now - lastCheck).count();
    
    if (timeSinceCheck >= 5) { // Check every 5 seconds
        bool wasConnected = isConnected;
        
        // Assume connected if we can get the activity manager
        isConnected = (activities != nullptr);
        
        if (!wasConnected && isConnected) {
        } else if (wasConnected && !isConnected) {
            std::cout << "Discord Rich Presence disconnected!" << std::endl;
        }
        
        lastCheck = now;
    }
#else
    // Always "connected" for console mode
    isConnected = true;
#endif
}