#include "user.hpp"

namespace fs = std::filesystem;

User::User(string path)
{
    setPlaylists(path);
}

User::~User()
{
    selectedPlaylist = nullptr;
}

void User::setPlaylists(string path){
    Playlist* ret = new Playlist(getPlaylistName(path));
    try {
        if (fs::exists(path) && fs::is_directory(path)) {
            for (const auto& entry : fs::recursive_directory_iterator(path)) {
                if (entry.is_regular_file() && entry.path().extension() == ".mp3") {
                    ret->addSong(entry.path().string());
                }
            }
        } else {
            std::cerr << "Invalid directory path: " << path << std::endl;
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        }

    selectedPlaylist = ret;
}

void User::startListening(){
    songLoader music;
    size_t index = 0;
    music.listenSong(selectedPlaylist->getSong(index));

    std::string command;
    while (true) {
        cout << "Enter command (next, pause, resume, quit): " << endl;
        cout << "Current Volume : " << "\033[32m" << music.getVolume() << "\033[0m" << endl;
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