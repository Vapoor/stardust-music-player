#ifndef SONGSCANNER_HPP
#define SONGSCANNER_HPP

#include <vector>
#include <string>
#include "Song.hpp"

class SongScanner {
public:
    static std::vector<Song> scanOsuSongs();
    
private:
    static std::string getOsuSongsPath();
    static Song parseFolderName(const std::string& folderName, const std::string& folderPath);
    static std::string findMp3File(const std::string& folderPath);
    static bool songExists(const std::vector<Song>& songs, const Song& newSong);
};

#endif