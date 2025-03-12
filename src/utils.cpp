#include "utils.hpp"

string getSongName(string filepath) {
    std::regex osuPattern(R"(.*?/Songs[\\/]\d+\s+([^\\/]+)[\\/])");
    std::regex mp3Pattern(R"((.+[\\/])?([^\\/]+)\.mp3$)");
    std::smatch match;
    
    if (std::regex_search(filepath, match, osuPattern)) {
        return "\033[38;5;200m" + match[1].str() + "\033[0m";
    } else if (std::regex_search(filepath, match, mp3Pattern)) {
        return "\033[38;5;200m" + match[2].str() + "\033[0m";
    }
    else{
        return filepath;
    }
}

string getPlaylistName(string filepath){
    // Check if the filepath contains "bin/playlist"
    if (filepath.find("bin/playlist") != std::string::npos) {
        // Find the position of the last '/'
        size_t lastSlash = filepath.find_last_of("/");
        
        // If a slash is found and it's not the last character, extract the name
        if (lastSlash != std::string::npos && lastSlash + 1 < filepath.length()) {
            return filepath.substr(lastSlash + 1);
        }
    }
    
    return "Untitled Playlist";
}


