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
        Mix_Music* currentmusic = nullptr;
        void playNextSong();
        int volume = 50; // 50%
    public:
        songLoader();
        ~songLoader();
        void listenSong(string song);
        inline int getVolume(){return volume;};
        inline void setVolume(int vol){volume = vol;};

};


#endif