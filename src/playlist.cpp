#include "playlist.hpp"

Playlist::Playlist(string user_name)
:name(user_name),playlistsongs(vector<string>())
{}


Playlist::~Playlist()
{
    playlistsongs.clear();
}

void Playlist::addSong(string new_song){
    if (find(playlistsongs.begin(),playlistsongs.end(), new_song)!= playlistsongs.end()){
        cout << "The song : " << getSongName(new_song) << " is already in the playlist ! " << endl;
    }
    else{
    playlistsongs.push_back(new_song);
    }
}

string Playlist::getSong(size_t index){
    return playlistsongs[index];
}