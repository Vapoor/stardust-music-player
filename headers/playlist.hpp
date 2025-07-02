#ifndef PLAYLIST_HPP
#define PLAYLIST_HPP

#include <vector>
#include <string>
#include <map>
#include "Song.hpp"

class Playlist {
public:
    Playlist(const std::string& name = "Default");
    
    void addSong(const Song& song);
    void removeSong(int index);
    void clear();
    
    const std::vector<Song>& getSongs() const;
    std::string getName() const;
    void setName(const std::string& name);
    
    bool isEmpty() const;
    size_t size() const;
    
    Song getSong(int index) const;
    
    void displaySongs() const;
    
private:
    std::string playlistName;
    std::vector<Song> songs;
};

class PlaylistManager {
public:
    static PlaylistManager& getInstance();
    
    void createPlaylist(const std::string& name);
    void deletePlaylist(const std::string& name);
    void addSongToPlaylist(const std::string& playlistName, const Song& song);
    void removeSongFromPlaylist(const std::string& playlistName, int songIndex);
    
    std::vector<std::string> getPlaylistNames() const;
    Playlist* getPlaylist(const std::string& name);
    
    void displayAllPlaylists() const;
    void displayPlaylist(const std::string& name) const;
    
    void savePlaylistsToFile(const std::string& filename) const;
    void loadPlaylistsFromFile(const std::string& filename);
    
private:
    PlaylistManager() = default;
    std::map<std::string, Playlist> playlists;
};

#endif