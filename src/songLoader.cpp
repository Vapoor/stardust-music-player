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
    bool running = true;
    size_t currentSongIndex = index;
    if (SDL_Init(SDL_INIT_AUDIO) < 0){
        throw SDL_GetError();
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0){
        throw Mix_GetError();
    }
    cout << "Now Playing : " << songs[currentSongIndex] << endl;

    // Loading the mp3 file
    Mix_Music* music = Mix_LoadMUS(songs[currentSongIndex].c_str());

    while(running){
        if(!music){
            cerr << "Failed to load : " << songs[currentSongIndex] << endl;
        }
       
        Mix_PlayMusic(music,1);

    

    SDL_Event event;
    while (SDL_PollEvent(&event)){
        if (event.type == SDL_QUIT){
            running = false;
            break;
        }
        else if (event.type == SDL_KEYDOWN){
            switch (event.key.keysym.sym){
                case SDLK_p: //Pause and Resume
                    if (Mix_PausedMusic()){
                        Mix_ResumeMusic();
                    }
                    else{
                        Mix_PauseMusic();
                    }
                case SDLK_s: // Stop
                    Mix_HaltMusic();
                    Mix_FreeMusic(music);
                    music = nullptr;
                    break;

                case SDLK_n: // Next Song
                    Mix_HaltMusic();
                    Mix_FreeMusic(music);
                    music = nullptr;
                    currentSongIndex = (currentSongIndex+1)% songs.size();
                    break;

                case SDLK_ESCAPE: // Quit
                    running = false;
                    break;

                default:
                    break;
            }
        }
    }

    if (!Mix_PlayingMusic() && music) {
        Mix_FreeMusic(music);
        music = nullptr;
        currentSongIndex = (currentSongIndex+1)% songs.size();
    }

    SDL_Delay(100);

    }

    if (music) {
        Mix_FreeMusic(music);
    }
}