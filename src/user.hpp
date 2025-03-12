#ifndef USER_HPP
#define USER_HPP


#include <filesystem>
#include "songLoader.hpp"
#include "playlist.hpp"
#include "utils.hpp"


class User{
    private:
        Playlist* selectedPlaylist;
        void setPlaylists(string path);

    public:
    User(string path);
    ~User();
    void startListening();

};




#endif