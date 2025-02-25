#include "songLoader.hpp"

int main(int argc, char* argv[]){
    songLoader test = songLoader("C:/Users/Utilisateur/AppData/Local/osu!/Songs");
    test.debugSongs();
    // test.listenSong(0);
    return 0;
}