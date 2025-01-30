#include "songLoader.hpp"

songLoader::songLoader(string filepath)
:filepath(filepath){
    updateSongs();
}

songLoader::~songLoader(){
    songs.clear();
}

void songLoader::updateSongs(){
    // We looking for all the beatmaps. Windows path ONLY

}

void songLoader::debugSongs(){
    cout << "Liste des musiques actuelles : " << endl;
    for (string str : songs){
        cout << str << endl;
    }
}