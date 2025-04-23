#include "songLoader.hpp"

namespace fs = std::filesystem;

songLoader::songLoader()

{}

songLoader::~songLoader()
{}



void songLoader::playNextSong() {
  
}



void songLoader::listenSong(string song){
    
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }
    
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! Mix_Error: " << Mix_GetError() << std::endl;
        SDL_Quit();
        return;
    }
    
    currentmusic = Mix_LoadMUS(song.c_str());
    if (!currentmusic) {
        std::cerr << "Failed to load music: " << Mix_GetError() << std::endl;
        Mix_CloseAudio();
        SDL_Quit();
        return;
    }
    
    Mix_PlayMusic(currentmusic, 0);
    Mix_VolumeMusic((volume*128)/100);
    std::cout << "Playing: " << getSongName(song) << std::endl;
    
   

}


