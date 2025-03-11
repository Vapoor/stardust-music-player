#include "playlist.hpp"

Playlist::Playlist(string user_name)
:name(user_name),playlistsongs(vector<string>())
{}


Playlist::~Playlist()
{
    playlistsongs.clear();
}

void Playlist::addSong(string new_song){
    playlistsongs.push_back(new_song);
}