#include <csignal>
#include "user.hpp"

void handleSignal(int signal) {
    std::cout << "\nCtrl+C detected! Stopping music and exiting..." << std::endl;
    Mix_HaltMusic(); // Stop music
    Mix_CloseAudio(); // Close SDL_Mixer properly
    SDL_Quit(); // Quit SDL
    exit(0); // Ensure clean exit
}

int main(int argc, char* argv[]){
    signal(SIGINT, handleSignal);
    string select  = " ";
    if (argc > 1){
        select = argv[1];
    }
    else{
        select = "./test";
    }

    User mainuser(select);
    mainuser.startListening();
    return 0;
}