#include "../headers/songScanner.hpp"
#include <filesystem>
#include <iostream>
#include <regex>
#include <cstdlib>

namespace fs = std::filesystem;

std::vector<Song> SongScanner::scanOsuSongs() {
    std::vector<Song> songs;
    std::string songsPath = getOsuSongsPath();
    
    if (!fs::exists(songsPath)) {
        std::cout << "osu! Songs folder not found at: " << songsPath << std::endl;
        return songs;
    }
    
    std::cout << "Scanning osu! songs directory..." << std::endl;
    int scannedCount = 0;
    
    try {
        for (const auto& entry : fs::directory_iterator(songsPath)) {
            if (entry.is_directory()) {
                std::string folderName = entry.path().filename().string();
                Song song = parseFolderName(folderName, entry.path().string());
                
                if (!song.filePath.empty() && !songExists(songs, song)) {
                    songs.push_back(song);
                    scannedCount++;
                    if (scannedCount % 50 == 0) {
                        std::cout << "Scanned " << scannedCount << " songs..." << std::endl;
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Error scanning directory: " << e.what() << std::endl;
    }
    
    std::cout << "Found " << songs.size() << " unique songs." << std::endl;
    return songs;
}

std::string SongScanner::getOsuSongsPath() {
    std::string localAppData;
    
#ifdef _WIN32
    const char* appData = std::getenv("LOCALAPPDATA");
    if (appData) {
        localAppData = std::string(appData);
    } else {
        // Fallback to APPDATA if LOCALAPPDATA is not available
        const char* appDataFallback = std::getenv("APPDATA");
        if (appDataFallback) {
            localAppData = std::string(appDataFallback) + "\\..\\Local";
        }
    }
#else
    // For non-Windows systems, try common paths
    const char* home = std::getenv("HOME");
    if (home) {
        localAppData = std::string(home) + "/.local/share";
    }
#endif
    
    return localAppData + "/osu!/Songs";
}

Song SongScanner::parseFolderName(const std::string& folderName, const std::string& folderPath) {
    // Pattern: "ID Artist - Title" or similar variations
    std::regex pattern(R"(^\d+\s*(.+))");
    std::smatch matches;
    
    if (std::regex_match(folderName, matches, pattern)) {
        std::string artistTitle = matches[1].str();
        
        // Try to split artist and title by " - "
        size_t dashPos = artistTitle.find(" - ");
        std::string artist, title;
        
        if (dashPos != std::string::npos) {
            artist = artistTitle.substr(0, dashPos);
            title = artistTitle.substr(dashPos + 3);
        } else {
            // If no dash found, use the whole string as title
            artist = "Unknown Artist";
            title = artistTitle;
        }
        
        // Find MP3 file in the folder
        std::string mp3Path = findMp3File(folderPath);
        
        if (!mp3Path.empty()) {
            return Song(artist, title, mp3Path, 0);
        }
    }
    
    return Song(); // Return empty song if parsing failed
}

std::string SongScanner::findMp3File(const std::string& folderPath) {
    try {
        for (const auto& entry : fs::directory_iterator(folderPath)) {
            if (entry.is_regular_file()) {
                std::string extension = entry.path().extension().string();
                std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
                
                if (extension == ".mp3") {
                    return entry.path().string();
                }
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Error reading folder " << folderPath << ": " << e.what() << std::endl;
    }
    
    return "";
}

bool SongScanner::songExists(const std::vector<Song>& songs, const Song& newSong) {
    for (const auto& song : songs) {
        if (song == newSong) {
            return true;
        }
    }
    return false;
}