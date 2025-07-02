#ifndef SONG_HPP
#define SONG_HPP

#include <string>

struct Song {
    std::string artist;
    std::string title;
    std::string filePath;
    int id;
    
    Song() : id(0) {}
    
    Song(const std::string& artist, const std::string& title, const std::string& path, int songId)
        : artist(artist), title(title), filePath(path), id(songId) {}
    
    std::string getDisplayName() const {
        return artist + " - " + title;
    }
    
    bool operator==(const Song& other) const {
        return artist == other.artist && title == other.title;
    }
};

#endif