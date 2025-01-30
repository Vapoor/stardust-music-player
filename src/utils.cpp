#include "utils.hpp"

string getSongName(string filepath){
    size_t pos = filepath.find(' ');
    if (pos != string::npos){
        return filepath.substr(pos+1);
    }
    return filepath;
}
