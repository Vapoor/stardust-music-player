#include "songLoader.hpp"

int main(int argc, char* argv[]){
    if (argc != 2){
        std::cout << "Usage: " << argv[0] << " <path to music folder>" << std::endl;
        return 1;
    }
    songLoader test = songLoader(argv[1]);
    // test.debugSongs();
    test.listenSong();
    return 0;
}