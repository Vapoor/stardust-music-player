#include "utils.hpp"

string getSongName(string filepath) {
    std::regex pattern(R"(.*?/Songs[\\/]\d+\s+([^\\/]+)[\\/])");
    std::smatch match;

    if (std::regex_search(filepath, match, pattern)) {
        return "\033[38;5;200m" + match[1].str() + "\033[0m";
    }
    return "";
}
