#include <iostream>
#include "../headers/musicPlayer.hpp"

int main() {
    try {
        MusicPlayer player;
        
        if (!player.initialize()) {
            std::cout << "Failed to initialize music player!" << std::endl;
            return 1;
        }
        
        player.run();
        
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "Unknown error occurred!" << std::endl;
        return 1;
    }
    
    return 0;
}