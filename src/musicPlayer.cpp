#include "../headers/musicPlayer.hpp"
#include "../headers/songScanner.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <thread>
#include <chrono>

MusicPlayer::MusicPlayer() : currentSongIndex(-1), isUsingPlaylist(false) {}

MusicPlayer::~MusicPlayer() {}

bool MusicPlayer::initialize() {
    std::cout << "Initializing osu! Music Player..." << std::endl;
    
    // Initialize audio player
    if (!audioPlayer.initialize()) {
        std::cout << "Failed to initialize audio player!" << std::endl;
        return false;
    }
    
    // Load playlists
    PlaylistManager::getInstance().loadPlaylistsFromFile("playlists.txt");
    
    // Scan for songs
    scanSongs();
    
    std::cout << "Initialization complete!" << std::endl;
    return true;
}

void MusicPlayer::run() {
    std::cout << "\n=== osu! Music Player ===" << std::endl;
    std::cout << "Type 'help' for available commands" << std::endl;
    
    std::string input;
    while (true) {
        update(); // Update audio system
        
        std::cout << "\n> ";
        std::getline(std::cin, input);
        
        if (input == "quit" || input == "exit") {
            break;
        }
        
        processCommand(input);
    }
    
    // Save playlists before exiting
    PlaylistManager::getInstance().savePlaylistsToFile("playlists.txt");
    std::cout << "Goodbye!" << std::endl;
}

void MusicPlayer::displayMenu() {
    std::cout << "\n=== Available Commands ===" << std::endl;
    std::cout << "General:" << std::endl;
    std::cout << "  help - Show this menu" << std::endl;
    std::cout << "  scan - Rescan osu! songs directory" << std::endl;
    std::cout << "  list - Show all songs" << std::endl;
    std::cout << "  search <query> - Search for songs" << std::endl;
    std::cout << "  queue - Show current queue" << std::endl;
    std::cout << "\nPlayback:" << std::endl;
    std::cout << "  play <number> - Play song by index" << std::endl;
    std::cout << "  play - Resume/play current song" << std::endl;
    std::cout << "  pause - Pause playback" << std::endl;
    std::cout << "  stop - Stop playback" << std::endl;
    std::cout << "  next - Next song" << std::endl;
    std::cout << "  prev - Previous song" << std::endl;
    std::cout << "  vol <0-100> - Set volume" << std::endl;
    std::cout << "  current - Show current song info" << std::endl;
    std::cout << "\nPlaylists:" << std::endl;
    std::cout << "  playlists - Show all playlists" << std::endl;
    std::cout << "  create <name> - Create new playlist" << std::endl;
    std::cout << "  delete <name> - Delete playlist" << std::endl;
    std::cout << "  show <name> - Show playlist contents" << std::endl;
    std::cout << "  add <playlist> <song_index> - Add song to playlist" << std::endl;
    std::cout << "  remove <playlist> <song_index> - Remove song from playlist" << std::endl;
    std::cout << "  playlist <name> - Play entire playlist" << std::endl;
    std::cout << "\nOther:" << std::endl;
    std::cout << "  quit/exit - Exit program" << std::endl;
}

void MusicPlayer::processCommand(const std::string& command) {
    if (command.empty()) return;
    
    std::vector<std::string> parts = splitCommand(command);
    std::string cmd = parts[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
    
    if (cmd == "help") {
        displayMenu();
    }
    else if (cmd == "scan") {
        scanSongs();
    }
    else if (cmd == "list") {
        displayAllSongs();
    }
    else if (cmd == "search" && parts.size() > 1) {
        std::string query = command.substr(command.find(' ') + 1);
        searchSongs(query);
    }
    else if (cmd == "play") {
        if (parts.size() > 1) {
            int index = parseIntCommand(parts[1]) - 1; // Convert to 0-based index
            playFromQueue(index);
        } else {
            if (audioPlayer.isPaused()) {
                audioPlayer.resume();
            } else {
                playCurrentSong();
            }
        }
    }
    else if (cmd == "pause") {
        pauseResume();
    }
    else if (cmd == "stop") {
        stopPlayback();
    }
    else if (cmd == "next") {
        playNext();
    }
    else if (cmd == "prev" || cmd == "previous") {
        playPrevious();
    }
    else if (cmd == "vol" || cmd == "volume") {
        if (parts.size() > 1) {
            float vol = parseIntCommand(parts[1]) / 100.0f;
            setVolume(vol);
        }
    }
    else if (cmd == "current") {
        showCurrentSong();
    }
    else if (cmd == "queue") {
        displayQueue();
    }
    else if (cmd == "playlists") {
        showPlaylists();
    }
    else if (cmd == "create" && parts.size() > 1) {
        std::string name = command.substr(command.find(' ') + 1);
        createPlaylistCommand(name);
    }
    else if (cmd == "delete" && parts.size() > 1) {
        std::string name = command.substr(command.find(' ') + 1);
        deletePlaylistCommand(name);
    }
    else if (cmd == "show" && parts.size() > 1) {
        std::string name = command.substr(command.find(' ') + 1);
        showPlaylistContents(name);
    }
    else if (cmd == "add" && parts.size() > 2) {
        std::string playlistName = parts[1];
        int songIndex = parseIntCommand(parts[2]) - 1;
        addToPlaylistCommand(playlistName, songIndex);
    }
    else if (cmd == "remove" && parts.size() > 2) {
        std::string playlistName = parts[1];
        int songIndex = parseIntCommand(parts[2]) - 1;
        removeFromPlaylistCommand(playlistName, songIndex);
    }
    else if (cmd == "playlist" && parts.size() > 1) {
        std::string name = command.substr(command.find(' ') + 1);
        playPlaylist(name);
    }
    else {
        std::cout << "Unknown command. Type 'help' for available commands." << std::endl;
    }
}

void MusicPlayer::scanSongs() {
    std::cout << "Scanning osu! songs..." << std::endl;
    allSongs = SongScanner::scanOsuSongs();
    setQueueFromAllSongs();
}

void MusicPlayer::displayAllSongs() {
    if (allSongs.empty()) {
        std::cout << "No songs found. Try scanning first with 'scan' command." << std::endl;
        return;
    }
    
    displaySongList(allSongs);
}

void MusicPlayer::searchSongs(const std::string& query) {
    std::vector<Song> results;
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
    
    for (const auto& song : allSongs) {
        std::string songName = song.getDisplayName();
        std::transform(songName.begin(), songName.end(), songName.begin(), ::tolower);
        
        if (songName.find(lowerQuery) != std::string::npos) {
            results.push_back(song);
        }
    }
    
    if (results.empty()) {
        std::cout << "No songs found matching: " << query << std::endl;
    } else {
        std::cout << "Search results for '" << query << "':" << std::endl;
        displaySongList(results);
    }
}

void MusicPlayer::playCurrentSong() {
    if (currentQueue.empty()) {
        std::cout << "No songs in queue. Add some songs first!" << std::endl;
        return;
    }
    
    if (currentSongIndex < 0 || currentSongIndex >= static_cast<int>(currentQueue.size())) {
        currentSongIndex = 0;
    }
    
    const Song& song = currentQueue[currentSongIndex];
    if (audioPlayer.loadSong(song)) {
        audioPlayer.play();
    }
}

void MusicPlayer::playNext() {
    if (currentQueue.empty()) {
        std::cout << "No songs in queue!" << std::endl;
        return;
    }
    
    currentSongIndex++;
    if (currentSongIndex >= static_cast<int>(currentQueue.size())) {
        currentSongIndex = 0; // Loop back to beginning
        std::cout << "Reached end of queue, looping to beginning." << std::endl;
    }
    
    playCurrentSong();
}

void MusicPlayer::playPrevious() {
    if (currentQueue.empty()) {
        std::cout << "No songs in queue!" << std::endl;
        return;
    }
    
    currentSongIndex--;
    if (currentSongIndex < 0) {
        currentSongIndex = static_cast<int>(currentQueue.size()) - 1; // Loop to end
        std::cout << "Reached beginning of queue, looping to end." << std::endl;
    }
    
    playCurrentSong();
}

void MusicPlayer::pauseResume() {
    if (audioPlayer.isPlaying()) {
        audioPlayer.pause();
    } else if (audioPlayer.isPaused()) {
        audioPlayer.resume();
    } else {
        std::cout << "No song is currently playing." << std::endl;
    }
}

void MusicPlayer::stopPlayback() {
    audioPlayer.stop();
}

void MusicPlayer::setVolume(float volume) {
    audioPlayer.setVolume(volume);
    std::cout << "Volume set to " << static_cast<int>(volume * 100) << "%" << std::endl;
}

void MusicPlayer::showCurrentSong() {
    if (currentSongIndex >= 0 && currentSongIndex < static_cast<int>(currentQueue.size())) {
        const Song& song = currentQueue[currentSongIndex];
        std::cout << "Current song: " << song.getDisplayName() << std::endl;
        std::cout << "State: ";
        
        switch (audioPlayer.getState()) {
            case PlaybackState::PLAYING:
                std::cout << "Playing";
                break;
            case PlaybackState::PAUSED:
                std::cout << "Paused";
                break;
            case PlaybackState::STOPPED:
                std::cout << "Stopped";
                break;
        }
        
        std::cout << " | Volume: " << static_cast<int>(audioPlayer.getVolume() * 100) << "%" << std::endl;
        
        if (isUsingPlaylist) {
            std::cout << "Playing from playlist: " << currentPlaylistName << std::endl;
        }
    } else {
        std::cout << "No song selected." << std::endl;
    }
}

void MusicPlayer::playFromQueue(int index) {
    if (index < 0 || index >= static_cast<int>(currentQueue.size())) {
        std::cout << "Invalid song index!" << std::endl;
        return;
    }
    
    currentSongIndex = index;
    playCurrentSong();
}

void MusicPlayer::setQueueFromAllSongs() {
    currentQueue = allSongs;
    isUsingPlaylist = false;
    currentPlaylistName.clear();
    std::cout << "Queue set to all songs (" << currentQueue.size() << " songs)" << std::endl;
}

void MusicPlayer::setQueueFromPlaylist(const std::string& playlistName) {
    Playlist* playlist = PlaylistManager::getInstance().getPlaylist(playlistName);
    if (playlist && !playlist->isEmpty()) {
        currentQueue = playlist->getSongs();
        isUsingPlaylist = true;
        currentPlaylistName = playlistName;
        currentSongIndex = -1;
        std::cout << "Queue set to playlist '" << playlistName << "' (" << currentQueue.size() << " songs)" << std::endl;
    } else {
        std::cout << "Playlist '" << playlistName << "' not found or empty!" << std::endl;
    }
}

void MusicPlayer::clearQueue() {
    currentQueue.clear();
    currentSongIndex = -1;
    isUsingPlaylist = false;
    currentPlaylistName.clear();
}

void MusicPlayer::displayQueue() {
    if (currentQueue.empty()) {
        std::cout << "Queue is empty." << std::endl;
        return;
    }
    
    std::cout << "\nCurrent Queue";
    if (isUsingPlaylist) {
        std::cout << " (Playlist: " << currentPlaylistName << ")";
    }
    std::cout << ":" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    for (size_t i = 0; i < currentQueue.size(); ++i) {
        std::string marker = (static_cast<int>(i) == currentSongIndex) ? " -> " : "    ";
        std::cout << marker << (i + 1) << ". " << currentQueue[i].getDisplayName() << std::endl;
    }
}

void MusicPlayer::createPlaylistCommand(const std::string& name) {
    PlaylistManager::getInstance().createPlaylist(name);
}

void MusicPlayer::deletePlaylistCommand(const std::string& name) {
    PlaylistManager::getInstance().deletePlaylist(name);
}

void MusicPlayer::showPlaylists() {
    PlaylistManager::getInstance().displayAllPlaylists();
}

void MusicPlayer::showPlaylistContents(const std::string& name) {
    PlaylistManager::getInstance().displayPlaylist(name);
}

void MusicPlayer::addToPlaylistCommand(const std::string& playlistName, int songIndex) {
    if (songIndex < 0 || songIndex >= static_cast<int>(allSongs.size())) {
        std::cout << "Invalid song index! Use 'list' to see available songs." << std::endl;
        return;
    }
    
    PlaylistManager::getInstance().addSongToPlaylist(playlistName, allSongs[songIndex]);
}

void MusicPlayer::removeFromPlaylistCommand(const std::string& playlistName, int songIndex) {
    PlaylistManager::getInstance().removeSongFromPlaylist(playlistName, songIndex);
}

void MusicPlayer::playPlaylist(const std::string& name) {
    setQueueFromPlaylist(name);
    if (!currentQueue.empty()) {
        currentSongIndex = 0;
        playCurrentSong();
    }
}

void MusicPlayer::displaySongList(const std::vector<Song>& songs) {
    if (songs.empty()) {
        std::cout << "No songs to display." << std::endl;
        return;
    }
    
    std::cout << "\nSongs (" << songs.size() << " total):" << std::endl;
    std::cout << "================================" << std::endl;
    
    for (size_t i = 0; i < songs.size(); ++i) {
        std::cout << (i + 1) << ". " << songs[i].getDisplayName() << std::endl;
        
        // Show in batches of 20 to avoid overwhelming output
        if ((i + 1) % 20 == 0 && i + 1 < songs.size()) {
            std::cout << "\nPress Enter to continue or type 'q' to quit listing: ";
            std::string input;
            std::getline(std::cin, input);
            if (input == "q") break;
            std::cout << std::endl;
        }
    }
}

int MusicPlayer::parseIntCommand(const std::string& input, int defaultValue) {
    try {
        return std::stoi(input);
    } catch (const std::exception&) {
        return defaultValue;
    }
}

std::vector<std::string> MusicPlayer::splitCommand(const std::string& command) {
    std::vector<std::string> parts;
    std::stringstream ss(command);
    std::string part;
    
    while (ss >> part) {
        parts.push_back(part);
    }
    
    return parts;
}

void MusicPlayer::showHelp() {
    displayMenu();
}

void MusicPlayer::update() {
    audioPlayer.update();
    
    // Auto-advance to next song if current song finished
    if (audioPlayer.getState() == PlaybackState::STOPPED && 
        currentSongIndex >= 0 && !currentQueue.empty()) {
        
        // Check if we were actually playing (not manually stopped)
        static bool wasPlaying = false;
        if (wasPlaying) {
            std::cout << "\nSong finished, playing next..." << std::endl;
            playNext();
        }
        wasPlaying = false;
    } else if (audioPlayer.getState() == PlaybackState::PLAYING) {
        static bool wasPlaying = true;
    }
    
    // Small delay to prevent excessive CPU usage
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}