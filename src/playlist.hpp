#ifndef PLAYLIST_HPP
#define PLAYLIST_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#include "utils.hpp"
using namespace std;

class Playlist{
    private:
        string name;
        vector<string> playlistsongs;

    public:
        Playlist(string user_name);
        ~Playlist();
        void addSong(string new_song);
        inline size_t getSize(){return playlistsongs.size();};
        string getSong(size_t index);

};









#endif