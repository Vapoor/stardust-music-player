#ifndef USER_HPP
#define USER_HPP


#include <filesystem>
#include <algorithm>
#include "songLoader.hpp"
#include "playlist.hpp"
#include "utils.hpp"


class User{
    private:
        vector<const string> musicFiles;
        vector<Playlist*> allPlaylists;
        void setmusicFiles(string path);
        void readExistingPlaylist();

    public:
    User(string path);
    ~User();
    void startListening(Playlist* current_playlist);

};




#endif