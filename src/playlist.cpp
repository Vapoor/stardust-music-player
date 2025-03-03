#include "playlist.hpp"

Playlist::Playlist(string user_name,string filepath)
:name(user_name),filepath(filepath),playlistsongs(vector<string>())
{}


Playlist::~Playlist()
{
    playlistsongs.clear();
}