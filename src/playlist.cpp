#include "playlist.hpp"

Playlist::Playlist(string user_name)
:name(user_name),playlistsongs(vector<string>())
{}


Playlist::~Playlist()
{
    playlistsongs.clear();
}