#ifndef USER_HPP
#define USER_HPP

#include "songLoader.hpp"
#include "playlist.hpp"

class User{
    private:
        vector<Playlist> playlists;
        Playlist selectedPlaylist;

    public:
    User();
    ~User();

};




#endif