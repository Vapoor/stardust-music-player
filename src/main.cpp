#include "songLoader.hpp"

int main(int argc, char* argv[]){
    songLoader test = songLoader("C:/Users/Utilisateur/AppData/Local/osu!/Songs"); // Put Whatever folder with mp3 you want
    // test.debugSongs();
    test.listenSong();
    return 0;
}