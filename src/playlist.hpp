#ifndef PLAYLIST_HPP
#define PLAYLIST_HPP

#include <string>
#include <vector>
using namespace std;

class Playlist{
    private:
        string name;
        vector<string> playlistsongs;

    public:
        Playlist(string user_name);
        ~Playlist();

};









#endif