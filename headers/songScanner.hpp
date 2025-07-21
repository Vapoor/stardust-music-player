#ifndef SONGSCANNER_HPP
#define SONGSCANNER_HPP

#include <vector>
#include <string>
#include "Song.hpp"

class SongScanner {
public:
    static std::vector<Song> scanOsuSongs();
    
private:
    // Main scanning functions
    static std::vector<Song> scanOsuDirectory();
    static std::vector<Song> scanGeometryDashDirectory();
    
    // Path functions
    static std::string getOsuSongsPath();
    static std::string getGeometryDashPath();
    
    // osu! specific functions
    static Song parseFolderName(const std::string& folderName, const std::string& folderPath);
    static std::string findMp3File(const std::string& folderPath);
    
    // Geometry Dash specific functions
    static Song extractMetadataFromMp3(const std::string& filePath);
    static std::string cleanMetadataString(const std::string& input);
    static std::string wideStringToUtf8(const std::wstring& wstr);
    
    // Utility functions
    static bool songExists(const std::vector<Song>& songs, const Song& newSong);
};

#endif