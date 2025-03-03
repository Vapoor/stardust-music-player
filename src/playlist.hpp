#ifndef PLAYLIST_HPP
#define PLAYLIST_HPP

#include <string>
#include <vector>
using namespace std;

class Playlist{
    private:
        string name;
        string filepath;
        vector<string> playlistsongs;

    public:
        Playlist(string user_name, string filepath);
        ~Playlist();

};









#endif