#include "../headers/playlist.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

// Playlist Implementation
Playlist::Playlist(const std::string& name) : playlistName(name) {}

void Playlist::addSong(const Song& song) {
    songs.push_back(song);
}

void Playlist::removeSong(int index) {
    if (index >= 0 && index < static_cast<int>(songs.size())) {
        songs.erase(songs.begin() + index);
    }
}

void Playlist::clear() {
    songs.clear();
}

const std::vector<Song>& Playlist::getSongs() const {
    return songs;
}

std::string Playlist::getName() const {
    return playlistName;
}

void Playlist::setName(const std::string& name) {
    playlistName = name;
}

bool Playlist::isEmpty() const {
    return songs.empty();
}

size_t Playlist::size() const {
    return songs.size();
}

Song Playlist::getSong(int index) const {
    if (index >= 0 && index < static_cast<int>(songs.size())) {
        return songs[index];
    }
    return Song();
}

void Playlist::displaySongs() const {
    if (songs.empty()) {
        std::cout << "Playlist '" << playlistName << "' is empty." << std::endl;
        return;
    }
    
    std::cout << "\nPlaylist: " << playlistName << " (" << songs.size() << " songs)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    for (size_t i = 0; i < songs.size(); ++i) {
        std::cout << i + 1 << ". " << songs[i].getDisplayName() << std::endl;
    }
}

// PlaylistManager Implementation
PlaylistManager& PlaylistManager::getInstance() {
    static PlaylistManager instance;
    return instance;
}

void PlaylistManager::createPlaylist(const std::string& name) {
    if (playlists.find(name) == playlists.end()) {
        playlists[name] = Playlist(name);
        std::cout << "Created playlist: " << name << std::endl;
    } else {
        std::cout << "Playlist '" << name << "' already exists!" << std::endl;
    }
}

void PlaylistManager::deletePlaylist(const std::string& name) {
    auto it = playlists.find(name);
    if (it != playlists.end()) {
        playlists.erase(it);
        std::cout << "Deleted playlist: " << name << std::endl;
    } else {
        std::cout << "Playlist '" << name << "' not found!" << std::endl;
    }
}

void PlaylistManager::addSongToPlaylist(const std::string& playlistName, const Song& song) {
    auto it = playlists.find(playlistName);
    if (it != playlists.end()) {
        it->second.addSong(song);
    } else {
        std::cout << "Playlist '" << playlistName << "' not found!" << std::endl;
    }
}

void PlaylistManager::removeSongFromPlaylist(const std::string& playlistName, int songIndex) {
    auto it = playlists.find(playlistName);
    if (it != playlists.end()) {
        if (songIndex >= 0 && songIndex < static_cast<int>(it->second.size())) {
            Song song = it->second.getSong(songIndex);
            it->second.removeSong(songIndex);
            std::cout << "Removed '" << song.getDisplayName() << "' from playlist '" << playlistName << "'" << std::endl;
        } else {
            std::cout << "Invalid song index!" << std::endl;
        }
    } else {
        std::cout << "Playlist '" << playlistName << "' not found!" << std::endl;
    }
}

std::vector<std::string> PlaylistManager::getPlaylistNames() const {
    std::vector<std::string> names;
    for (const auto& pair : playlists) {
        names.push_back(pair.first);
    }
    return names;
}

Playlist* PlaylistManager::getPlaylist(const std::string& name) {
    auto it = playlists.find(name);
    if (it != playlists.end()) {
        return &it->second;
    }
    return nullptr;
}

void PlaylistManager::displayAllPlaylists() const {
    if (playlists.empty()) {
        std::cout << "No playlists created yet." << std::endl;
        return;
    }
    
    std::cout << "\nAvailable Playlists:" << std::endl;
    std::cout << "===================" << std::endl;
    
    for (const auto& pair : playlists) {
        std::cout << "- " << pair.first << " (" << pair.second.size() << " songs)" << std::endl;
    }
}

void PlaylistManager::displayPlaylist(const std::string& name) const {
    auto it = playlists.find(name);
    if (it != playlists.end()) {
        it->second.displaySongs();
    } else {
        std::cout << "Playlist '" << name << "' not found!" << std::endl;
    }
}

void PlaylistManager::savePlaylistsToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Could not save playlists to file." << std::endl;
        return;
    }
    
    for (const auto& pair : playlists) {
        const Playlist& playlist = pair.second;
        file << "[PLAYLIST]" << playlist.getName() << std::endl;
        
        for (const auto& song : playlist.getSongs()) {
            file << song.artist << "|" << song.title << "|" << song.filePath << std::endl;
        }
        
        file << "[END]" << std::endl;
    }
    
    file.close();
    std::cout << "Playlists saved to " << filename << std::endl;
}

void PlaylistManager::loadPlaylistsFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Could not load playlists from file (file may not exist yet)." << std::endl;
        return;
    }
    
    std::string line;
    std::string currentPlaylistName;
    
    while (std::getline(file, line)) {
        if (line.substr(0, 10) == "[PLAYLIST]") {
            currentPlaylistName = line.substr(10);
            createPlaylist(currentPlaylistName);
        } else if (line == "[END]") {
            currentPlaylistName.clear();
        } else if (!currentPlaylistName.empty() && !line.empty()) {
            std::stringstream ss(line);
            std::string artist, title, filePath;
            
            if (std::getline(ss, artist, '|') && 
                std::getline(ss, title, '|') && 
                std::getline(ss, filePath)) {
                
                Song song(artist, title, filePath, 0);
                addSongToPlaylist(currentPlaylistName, song);
            }
        }
    }
    
    file.close();
    std::cout << "Playlists loaded from " << filename << std::endl;
}