#ifndef MUSICPLAYER_HPP
#define MUSICPLAYER_HPP

#include <vector>
#include <string>
#include "Song.hpp"
#include "AudioPlayer.hpp"
#include "Playlist.hpp"

class MusicPlayer {
public:
    MusicPlayer();
    ~MusicPlayer();
    
    bool initialize();
    void run(); // Main application loop
    
private:
    AudioPlayer audioPlayer;
    std::vector<Song> allSongs;
    std::vector<Song> currentQueue;
    int currentSongIndex;
    
    bool isUsingPlaylist;
    std::string currentPlaylistName;
    
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
    void clearQueue();
    void displayQueue();
    
    // Playlist commands
    void createPlaylistCommand(const std::string& name);
    void deletePlaylistCommand(const std::string& name);
    void showPlaylists();
    void showPlaylistContents(const std::string& name);
    void addToPlaylistCommand(const std::string& playlistName, int songIndex);
    void removeFromPlaylistCommand(const std::string& playlistName, int songIndex);
    void playPlaylist(const std::string& name);
    
    // Utility functions
    void displaySongList(const std::vector<Song>& songs);
    int parseIntCommand(const std::string& input, int defaultValue = -1);
    std::vector<std::string> splitCommand(const std::string& command);
    void showHelp();
    
    void update(); // Called regularly to update audio and check for song end
};

#endif