#include "songLoader.hpp"

int main(int argc, char* argv[]){
    songLoader test = songLoader("./test");
    // test.debugSongs();
    test.listenSong();
    return 0;
}