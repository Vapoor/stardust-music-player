#include "songLoader.hpp"

namespace fs = std::filesystem;

songLoader::songLoader(string filepath)
:filepath(filepath)
{}

songLoader::~songLoader()
{}



void songLoader::playNextSong() {
  
}



void songLoader::listenSong(){
    
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }
    
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! Mix_Error: " << Mix_GetError() << std::endl;
        SDL_Quit();
        return;
    }
    
    currentmusic = Mix_LoadMUS(songs[index].c_str());
    if (!currentmusic) {
        std::cerr << "Failed to load music: " << Mix_GetError() << std::endl;
        Mix_CloseAudio();
        SDL_Quit();
        return;
    }
    
    Mix_PlayMusic(currentmusic, 0);
    Mix_VolumeMusic((volume*128)/100);
    std::cout << "Playing: " << getSongName(songs[index]) << std::endl;
    
    std::string command;
    while (true) {
        cout << "Enter command (next, pause, resume, quit): " << endl;
        cout << "Current Volume : " << "\033[32m" << volume << "\033[0m" << endl;
        cout << "You can set Volume by doing setv" << endl;
        cin >> command;
        SDL_Delay(100);


        if (command == "setv"){
            string volum;
            cout << "Choose a volume (between 0% and 100%)" << endl;
            cin >> volum;
            int vol = stoi(volum);
            if (vol < 0 || vol > 100){
                cerr << "You cannot put volume under 0 or above 100%" << endl;
            }
            Mix_VolumeMusic((vol*128)/100);
            volume = vol;
        }
        
        else if (command == "pause" || command == "p") {
            Mix_PauseMusic();
        } else if (command == "resume" || command == "r") {
            Mix_ResumeMusic();
        } 
            else if (command == "next" || command =="n" ) {
            index = (index + 1) % songs.size();
            Mix_FreeMusic(currentmusic);
            if (index >= songs.size()){
                playNextSong();
            }
            else{
            currentmusic = Mix_LoadMUS(songs[index].c_str());
            if (!currentmusic) {
                std::cerr << "Failed to load next song: " << Mix_GetError() << std::endl;
                break;
            }
            Mix_PlayMusic(currentmusic, 0);
            std::cout << "Now playing: " << getSongName(songs[index]) << std::endl;
            }
        } else if (command == "quit" || command == "q") {
            break; 
        }
        else  {
            std::cout << "Invalid command." << std::endl;
        }
    }
    
    Mix_FreeMusic(currentmusic);
    Mix_CloseAudio();
    SDL_Quit();
}

void musicFinished(){
    
}
