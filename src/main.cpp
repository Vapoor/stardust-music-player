#include <csignal>
#include "songLoader.hpp"

void handleSignal(int signal) {
    std::cout << "\nCtrl+C detected! Stopping music and exiting..." << std::endl;
    Mix_HaltMusic(); // Stop music
    Mix_CloseAudio(); // Close SDL_Mixer properly
    SDL_Quit(); // Quit SDL
    exit(0); // Ensure clean exit
}

int main(int argc, char* argv[]){
    signal(SIGINT, handleSignal);
    songLoader test = songLoader("./test"); // Put Whatever folder with mp3 you want
    // test.debugSongs();
    test.listenSong();
    return 0;
}