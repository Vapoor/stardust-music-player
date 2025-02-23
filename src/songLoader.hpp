#ifndef SONGLOADERHPP
#define SONGLOADERHPP

//#include "../lib/SFML/include/SFML/Audio.hpp"
#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include "utils.hpp"

using namespace std;

class songLoader{
    private:
        vector<string> songs;
        string filepath;
        //sf::Music currentSong;
    public:
        songLoader(string filepath);
        ~songLoader();
        void updateSongs();
        void debugSongs();
        void listenSong(size_t index);

};


#endif