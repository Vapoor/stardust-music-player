#ifndef SONGLOADERHPP
#define SONGLOADERHPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
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
    public:
        songLoader(string filepath);
        ~songLoader();
        void updateSongs();
        void debugSongs();
        void listenSong(size_t index);

};


#endif