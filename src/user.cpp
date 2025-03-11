#include "user.hpp"

namespace fs = std::filesystem;

User::User(string path)
{
    setPlaylists(path);
}

User::~User()
{
    selectedPlaylist = nullptr;
}

void User::setPlaylists(string path){
    // Reading file folder 
    try {
        if (fs::exists(path) && fs::is_directory(path)) {
            for (const auto& entry : fs::recursive_directory_iterator(path)) {
                if (entry.is_regular_file() && entry.path().extension() == ".mp3") {
                    songs.push_back(entry.path().string());
                }
            }
        } else {
            std::cerr << "Invalid directory path: " << filepath << std::endl;
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
}
}