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
