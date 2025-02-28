#include "songLoader.hpp"

namespace fs = std::filesystem;

songLoader::songLoader(string filepath)
:filepath(filepath){
    updateSongs();
}

songLoader::~songLoader(){
    songs.clear();
}

void songLoader::updateSongs(){
    // We looking for all the beatmaps. Windows path ONLY
    try {
            if (fs::exists(filepath) && fs::is_directory(filepath)) {
                for (const auto& entry : fs::recursive_directory_iterator(filepath)) {
                    if (entry.is_regular_file() && entry.path().extension() == ".mp3") {
                        songs.push_back(entry.path().string());
                    }
                }
            } else {
                std::cerr << "Invalid directory path: " << filepath << std::endl;
            }
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
    }

}

void songLoader::debugSongs(){
    cout << "Liste des musiques actuelles : " << endl;
    for (string str : songs){
        cout << str << endl;
    }
}

void songLoader::listenSong(size_t index){
    if (index < 0 || index >= songs.size()) {
        std::cerr << "Invalid song index." << std::endl;
        return;
    }

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }
    
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! Mix_Error: " << Mix_GetError() << std::endl;
        SDL_Quit();
        return;
    }
    
    Mix_Music* music = Mix_LoadMUS(songs[index].c_str());
    if (!music) {
        std::cerr << "Failed to load music: " << Mix_GetError() << std::endl;
        Mix_CloseAudio();
        SDL_Quit();
        return;
    }
    
    Mix_PlayMusic(music, -1);
    std::cout << "Playing: " << songs[index] << std::endl;
    
    std::string command;
    while (true) {
        std::cout << "Enter command (next, pause, resume, quit): ";
        std::cin >> command;
        
        if (command == "pause") {
            Mix_PauseMusic();
        } else if (command == "resume") {
            Mix_ResumeMusic();
        } else if (command == "next") {
            index = (index + 1) % songs.size();
            Mix_FreeMusic(music);
            if (index >= songs.size()){
                break;
            }
            music = Mix_LoadMUS(songs[index].c_str());
            if (!music) {
                std::cerr << "Failed to load next song: " << Mix_GetError() << std::endl;
                break;
            }
            Mix_PlayMusic(music, -1);
            std::cout << "Now playing: " << songs[index] << std::endl;
        } else if (command == "quit") {
            break;
        } else {
            std::cout << "Invalid command." << std::endl;
        }
    }
    
    Mix_FreeMusic(music);
    Mix_CloseAudio();
    SDL_Quit();
}
