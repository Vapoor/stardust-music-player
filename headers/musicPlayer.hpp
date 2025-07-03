#ifndef MUSICPLAYER_HPP
#define MUSICPLAYER_HPP

#include <vector>
#include <string>
#include "song.hpp"
#include "audioPlayer.hpp"
#include "playlist.hpp"
#include "discordPresence.hpp"

enum class QueueMode {
    ALL_SONGS,      // Playing all songs
    PLAYLIST,       // Playing from a specific playlist
    RANDOM          // Random mode
};

class MusicPlayer {
public:
    MusicPlayer();
    ~MusicPlayer();
    
    bool initialize();
    void run(); // Main application loop
    
private:
    AudioPlayer audioPlayer;
    RichPresence richPresence;
    std::vector<Song> allSongs;
    std::vector<Song> currentQueue;
    std::vector<int> randomIndices;  // For random mode
    int currentSongIndex;
    int randomPosition;              // Current position in random indices
    
    QueueMode queueMode;
    std::string currentPlaylistName;
    float savedVolume;               // Persistent volume
    
    void displayMenu();
    void processCommand(const std::string& command);
    
    // Song management
    void scanSongs();
    void displayAllSongs();
    void searchSongs(const std::string& query);
    
    // Playback controls
    void playCurrentSong();
    void playNext();
    void playPrevious();
    void pauseResume();
    void stopPlayback();
    void setVolume(float volume);
    void showCurrentSong();
    
    // Queue management
    void playFromQueue(int index);
    void setQueueFromAllSongs();
    void setQueueFromPlaylist(const std::string& playlistName);
    void setRandomMode();
    void clearQueue();
    void displayQueue();
    void updateDiscordPresence();
    
    // Playlist commands
    void createPlaylistCommand(const std::string& name);
    void deletePlaylistCommand(const std::string& name);
    void showPlaylists();
    void showPlaylistContents(const std::string& name);
    void addToPlaylistCommand(const std::string& playlistName, int songIndex);
    void removeFromPlaylistCommand(const std::string& playlistName, int songIndex);
    void playPlaylist(const std::string& name);
    
    // Random mode
    void generateRandomIndices();
    void playRandomSong();
    
    // Utility functions
    void displaySongList(const std::vector<Song>& songs, bool showGlobalIndex = false);
    int parseIntCommand(const std::string& input, int defaultValue = -1);
    std::vector<std::string> splitCommand(const std::string& command);
    void showHelp();
    void saveSettings();
    void loadSettings();
    
    void update(); // Called regularly to update audio and check for song end
};

#endif