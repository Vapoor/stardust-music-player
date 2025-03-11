#ifndef SONGLOADERHPP
#define SONGLOADERHPP

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include "utils.hpp"
#include "playlist.hpp"

using namespace std;

class songLoader{
    private:
        string filepath;
        Mix_Music* currentmusic = nullptr;
        void playNextSong();
        int volume = 50; // 50%
    public:
        songLoader(string filepath);
        ~songLoader();
        void listenSong();

};


#endif