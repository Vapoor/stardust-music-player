#include "user.hpp"

namespace fs = std::filesystem;

/**
 * Debugging Purpose
 */

 void User::debugmusicFiles(){
    for (size_t i = 0; i < musicFiles.size(); i++){
        cout << musicFiles[i] << endl;
    }
 }





 /**
  * Functions of the 
  * User Class
  */


User::User(string path)
{
    setmusicFiles(path);
    readExistingPlaylist();
}

User::~User()
{
    musicFiles.clear();
}

void User::setmusicFiles(string path){
    try {
        if (fs::exists(path) && fs::is_directory(path)) {
            for (const auto& entry : fs::recursive_directory_iterator(path)) {
                if (entry.is_regular_file() && entry.path().extension() == ".mp3") {
                    if (find(musicFiles.begin(),musicFiles.end(), entry.path().string()) == musicFiles.end()){
                        // Avoiding putting 2 times the same music, would be dumb no :) ?
                        musicFiles.push_back(entry.path().string());
                    }
                }
            }
        } else {
            std::cerr << "Invalid directory path: " << path << std::endl;
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        }

}


void User::readExistingPlaylist(){

}

void User::startListening(Playlist* current_playlist){
    songLoader music;
    size_t index = 0;
    if(current_playlist == nullptr){
        cout << "No playlist has been selected" << endl;
        if(musicFiles.size() != 0){
            music.listenSong(musicFiles[0]);
        }
    }
    else if (current_playlist->getSize() <= 0){
        std::cout << "This playlist don't have any musics in it" << endl;
        if(musicFiles.size() != 0){
            music.listenSong(musicFiles[0]);
        }
   
    }

    else{
        music.listenSong(current_playlist->getSong(index));
    }
    std::string command;

    // Menu while Listening 
    while (true) {
        cout << "Enter command (next, pause, resume, quit): " << endl;
        cout << "Current Volume : " << "\033[32m" << music.getVolume() << "\033[0m" << " | You can set Volume by doing setv" << endl;
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
            music.setVolume(vol);
        }
        
        else if (command == "pause" || command == "p") {
            Mix_PauseMusic();
        } else if (command == "resume" || command == "r") {
            Mix_ResumeMusic();
        } 
            else if (command == "next" || command =="n" ) {
                /*
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
            */
        } else if (command == "quit" || command == "q") {
            break; 
        }
        else  {
            std::cout << "Invalid command." << std::endl;
        }
    }
}
