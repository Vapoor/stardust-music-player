#include "../headers/songScanner.hpp"
#include <filesystem>
#include <iostream>
#include <regex>
#include <cstdlib>

// For metadata extraction
#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#include <propkey.h>
#include <propvarutil.h>
#include <comdef.h>
#pragma comment(lib, "propsys.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "shell32.lib")
#endif

namespace fs = std::filesystem;

std::vector<Song> SongScanner::scanOsuSongs() {
    std::vector<Song> songs;
    
    // Scan osu! songs
    std::vector<Song> osuSongs = scanOsuDirectory();
    songs.insert(songs.end(), osuSongs.begin(), osuSongs.end());
    
    // Scan Geometry Dash songs
    std::vector<Song> gdSongs = scanGeometryDashDirectory();
    songs.insert(songs.end(), gdSongs.begin(), gdSongs.end());
    
    std::cout << "Found " << songs.size() << " total songs (" << osuSongs.size() << " osu!, " << gdSongs.size() << " Geometry Dash)." << std::endl;
    return songs;
}

std::vector<Song> SongScanner::scanOsuDirectory() {
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
                        std::cout << "Scanned " << scannedCount << " osu! songs..." << std::endl;
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Error scanning osu! directory: " << e.what() << std::endl;
    }
    
    return songs;
}

std::vector<Song> SongScanner::scanGeometryDashDirectory() {
    std::vector<Song> songs;
    std::string gdPath = getGeometryDashPath();
    
    if (!fs::exists(gdPath)) {
        std::cout << "Geometry Dash folder not found at: " << gdPath << std::endl;
        return songs;
    }
    
    std::cout << "Scanning Geometry Dash songs directory..." << std::endl;
    int scannedCount = 0;
    int skippedCount = 0;
    
    try {
        for (const auto& entry : fs::directory_iterator(gdPath)) {
            if (entry.is_regular_file()) {
                std::string extension = entry.path().extension().string();
                std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
                
                if (extension == ".mp3") {
                    Song song = extractMetadataFromMp3(entry.path().string());
                    
                    if (!song.artist.empty() && !song.title.empty() && !songExists(songs, song)) {
                        songs.push_back(song);
                        scannedCount++;
                        if (scannedCount % 25 == 0) {
                            std::cout << "Scanned " << scannedCount << " Geometry Dash songs..." << std::endl;
                        }
                    } else {
                        skippedCount++;
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Error scanning Geometry Dash directory: " << e.what() << std::endl;
    }
    
    std::cout << "Geometry Dash scan complete: " << scannedCount << " songs added, " << skippedCount << " skipped (missing metadata)" << std::endl;
    return songs;
}

std::string SongScanner::getGeometryDashPath() {
    return "C:\\Users\\Utilisateur\\AppData\\Local\\GeometryDash";
}

Song SongScanner::extractMetadataFromMp3(const std::string& filePath) {
    Song song;
    song.filePath = filePath;
    
#ifdef _WIN32
    // Initialize COM
    HRESULT hr = CoInitialize(nullptr);
    if (FAILED(hr)) {
        return song; // Return empty song if COM initialization fails
    }
    
    try {
        // Convert to wide string
        std::wstring wFilePath(filePath.begin(), filePath.end());
        
        // Create property store
        IPropertyStore* pProps = nullptr;
        hr = SHGetPropertyStoreFromParsingName(wFilePath.c_str(), nullptr, GPS_READWRITE, IID_PPV_ARGS(&pProps));
        
        if (SUCCEEDED(hr)) {
            PROPVARIANT propVar;
            
            // Get Title
            PropVariantInit(&propVar);
            hr = pProps->GetValue(PKEY_Title, &propVar);
            if (SUCCEEDED(hr) && propVar.vt == VT_LPWSTR && propVar.pwszVal != nullptr) {
                song.title = wideStringToUtf8(propVar.pwszVal);
            }
            PropVariantClear(&propVar);
            
            // Get Contributing Artists (this is the correct property for "Contributing artists" field)
            PropVariantInit(&propVar);
            hr = pProps->GetValue(PKEY_Music_AlbumArtist, &propVar);
            if (SUCCEEDED(hr) && propVar.vt == VT_LPWSTR && propVar.pwszVal != nullptr) {
                song.artist = wideStringToUtf8(propVar.pwszVal);
            } else {
                // If Contributing Artists is empty, try regular Artist as fallback
                PropVariantClear(&propVar);
                PropVariantInit(&propVar);
                hr = pProps->GetValue(PKEY_Music_Artist, &propVar);
                if (SUCCEEDED(hr) && propVar.vt == VT_LPWSTR && propVar.pwszVal != nullptr) {
                    song.artist = wideStringToUtf8(propVar.pwszVal);
                }
            }
            PropVariantClear(&propVar);
            
            // Debug output to see what we found
            if (!song.title.empty() || !song.artist.empty()) {
                std::cout << "Found metadata for " << fs::path(filePath).filename().string() 
                         << " - Title: '" << song.title << "', Artist: '" << song.artist << "'" << std::endl;
            } else {
                std::cout << "No metadata found for: " << fs::path(filePath).filename().string() << std::endl;
            }
            
            pProps->Release();
        } else {
            std::cout << "Failed to get property store for: " << fs::path(filePath).filename().string() << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "Error extracting metadata from " << filePath << ": " << e.what() << std::endl;
    }
    
    CoUninitialize();
#else
    // For non-Windows systems, you would need a different metadata extraction library
    // such as TagLib (https://taglib.org/)
    std::cout << "Metadata extraction not implemented for this platform" << std::endl;
#endif
    
    // Clean up title and artist strings
    if (!song.title.empty()) {
        song.title = cleanMetadataString(song.title);
    }
    if (!song.artist.empty()) {
        song.artist = cleanMetadataString(song.artist);
    }
    
    return song;
}

std::string SongScanner::wideStringToUtf8(const std::wstring& wstr) {
#ifdef _WIN32
    if (wstr.empty()) return std::string();
    
    // Get the required buffer size
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
    if (sizeNeeded <= 0) return std::string();
    
    // Convert to UTF-8
    std::string result(sizeNeeded, 0);
    int bytesConverted = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), &result[0], sizeNeeded, nullptr, nullptr);
    
    if (bytesConverted <= 0) return std::string();
    
    return result;
#else
    // For non-Windows systems, this is a simplified conversion
    return std::string(wstr.begin(), wstr.end());
#endif
}

std::string SongScanner::cleanMetadataString(const std::string& input) {
    std::string result = input;
    
    // Remove leading/trailing whitespace
    result.erase(0, result.find_first_not_of(" \t\r\n"));
    result.erase(result.find_last_not_of(" \t\r\n") + 1);
    
    // Replace any problematic characters if needed
    // You can add more cleaning rules here if necessary
    
    return result;
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