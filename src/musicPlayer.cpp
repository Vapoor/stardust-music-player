// Enhanced musicPlayer.cpp with song index display and auto-progression
#include "../headers/musicPlayer.hpp"
#include "../headers/songScanner.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <thread>
#include <chrono>
#include <random>
#include <fstream>

MusicPlayer::MusicPlayer() : currentSongIndex(-1), randomPosition(-1), queueMode(QueueMode::ALL_SONGS), 
                            savedVolume(1.0f), showProgressTimer(false) {}

MusicPlayer::~MusicPlayer() {
    saveSettings();
}

bool MusicPlayer::initialize() {
    std::cout << "Initializing Stardust Music Player..." << std::endl;
    
    // Initialize audio player
    if (!audioPlayer.initialize()) {
        std::cout << "Failed to initialize audio player!" << std::endl;
        return false;
    }
    
    // Initialize Discord Rich Presence
    richPresence.initialize();
    
    // Load settings
    loadSettings();
    audioPlayer.setVolume(savedVolume);
    
    // Load playlists
    PlaylistManager::getInstance().loadPlaylistsFromFile("playlists.txt");
    
    // Scan for songs
    scanSongs();
    
    std::cout << "Initialization complete!" << std::endl;
    return true;
}

void MusicPlayer::run() {
    std::cout << "\n=== Stardust Music Player ===" << std::endl;
    std::cout << "Type 'help' for available commands" << std::endl;
    richPresence.setBrowsingState(static_cast<int>(allSongs.size()));
    
    std::string input;
    while (true) {
        update(); // Update audio system and check for auto-progression
        
        // Show progress if a song is playing
        if (showProgressTimer && audioPlayer.isPlaying()) {
            displayCurrentProgress();
        }
        
        // Check for input without blocking
        if (hasInput()) {
            std::cout << "\n> ";
            std::getline(std::cin, input);
            
            if (input == "quit" || input == "exit") {
                break;
            }
            
            processCommand(input);
        } else {
            // Small delay to prevent excessive CPU usage
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    // Save playlists and settings before exiting
    PlaylistManager::getInstance().savePlaylistsToFile("playlists.txt");
    saveSettings();
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
    std::cout << "  all - Switch back to all songs mode" << std::endl;
    std::cout << "  timer - Toggle progress timer display" << std::endl;
    std::cout << "\nPlayback:" << std::endl;
    std::cout << "  play <number> - Play song by index" << std::endl;
    std::cout << "  play - Resume/play current song" << std::endl;
    std::cout << "  pause - Pause playback" << std::endl;
    std::cout << "  stop - Stop playback" << std::endl;
    std::cout << "  next - Next song" << std::endl;
    std::cout << "  prev - Previous song" << std::endl;
    std::cout << "  vol <0-100> - Set volume (persistent)" << std::endl;
    std::cout << "  current - Show current song info" << std::endl;
    std::cout << "  random - Enable random mode" << std::endl;
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
    else if (cmd == "all") {
        setQueueFromAllSongs();
    }
    else if (cmd == "timer") {
        showProgressTimer = !showProgressTimer;
        std::cout << "Progress timer " << (showProgressTimer ? "enabled" : "disabled") << std::endl;
    }
    else if (cmd == "random") {
        setRandomMode();
    }
    else if (cmd == "loop") {
        toggleLoop();
    }
    else if (cmd == "check" && parts.size() > 1) {
        std::string playlistName = parts[1];
        checkCurrentSongInPlaylist(playlistName);
    }
    else if (cmd == "play") {
        if (parts.size() > 1) {
            int index = parseIntCommand(parts[1]) - 1; // Convert to 0-based index
            playFromQueue(index);
        } else {
            if (audioPlayer.isPaused()) {
                audioPlayer.resume();
                updateDiscordPresence();
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
    std::cout << "Scanning music library..." << std::endl;
    allSongs = SongScanner::scanOsuSongs();
    
    // Assign IDs to songs for easier reference
    for (size_t i = 0; i < allSongs.size(); ++i) {
        allSongs[i].id = static_cast<int>(i + 1);
    }
    
    if (queueMode == QueueMode::ALL_SONGS) {
        setQueueFromAllSongs();
    }
    
    richPresence.setBrowsingState(static_cast<int>(allSongs.size()));
}

void MusicPlayer::displayAllSongs() {
    if (allSongs.empty()) {
        std::cout << "No songs found. Try scanning first with 'scan' command." << std::endl;
        return;
    }
    
    displaySongList(allSongs, true);
}

void MusicPlayer::searchSongs(const std::string& query) {
    std::vector<Song> results;
    std::vector<int> globalIndices;
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);
    
    for (size_t i = 0; i < allSongs.size(); ++i) {
        std::string songName = allSongs[i].getDisplayName();
        std::transform(songName.begin(), songName.end(), songName.begin(), ::tolower);
        
        if (songName.find(lowerQuery) != std::string::npos) {
            results.push_back(allSongs[i]);
            globalIndices.push_back(static_cast<int>(i + 1));
        }
    }
    
    if (results.empty()) {
        std::cout << "No songs found matching: " << query << std::endl;
    } else {
        std::cout << "Search results for '" << query << "':" << std::endl;
        for (size_t i = 0; i < results.size(); ++i) {
            std::cout << globalIndices[i] << ". " << results[i].getDisplayName() << std::endl;
        }
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
        displayPlayingMessage();
        updateDiscordPresence();
    }
}

void MusicPlayer::displayPlayingMessage() {
    if (currentSongIndex >= 0 && currentSongIndex < static_cast<int>(currentQueue.size())) {
        const Song& song = currentQueue[currentSongIndex];
        
        // Get the song index in the original queue for display
        int displayIndex = getSongDisplayIndex(song);
        
        std::cout << "Playing: ";
        if (displayIndex > 0) {
            std::cout << displayIndex << ". ";
        }
        std::cout << song.getDisplayName() << " | Length: " << audioPlayer.formatTime(audioPlayer.getLength()) << std::endl;
    }
}

void MusicPlayer::displayCurrentProgress() {
    if (currentSongIndex >= 0 && currentSongIndex < static_cast<int>(currentQueue.size())) {
        const Song& song = currentQueue[currentSongIndex];
        int displayIndex = getSongDisplayIndex(song);
        
        std::cout << "\r";
        if (displayIndex > 0) {
            std::cout << displayIndex << ". ";
        }
        std::cout << song.getDisplayName() << " | " << audioPlayer.getProgressString();
        
        if (audioPlayer.getRemainingTime() <= 5000) { // Show countdown in last 5 seconds
            std::cout << " [AUTO-NEXT IN " << (audioPlayer.getRemainingTime() / 1000) << "s]";
        }
        
        std::cout << std::flush;
    }
}

int MusicPlayer::getSongDisplayIndex(const Song& song) {
    // If we're in playlist mode (including random playlist), show playlist position
    if (queueMode == QueueMode::PLAYLIST || 
        (queueMode == QueueMode::RANDOM && !currentPlaylistName.empty())) {
        
        for (size_t i = 0; i < currentQueue.size(); ++i) {
            if (currentQueue[i] == song) {
                return static_cast<int>(i + 1); // Playlist position (1-based)
            }
        }
    }
    
    // For all songs mode (including random all songs), show global ID
    return song.id;
}

void MusicPlayer::playNext() {
    if (currentQueue.empty()) {
        std::cout << "No songs in queue!" << std::endl;
        return;
    }
    
    if (queueMode == QueueMode::RANDOM) {
        randomPosition++;
        if (randomPosition >= static_cast<int>(randomIndices.size())) {
            // Generate new random order and continue
            generateRandomIndices();
            randomPosition = 0;
            std::cout << "Generated new random order." << std::endl;
        }
        currentSongIndex = randomIndices[randomPosition];
    } else {
        currentSongIndex++;
        if (currentSongIndex >= static_cast<int>(currentQueue.size())) {
            currentSongIndex = 0; // Loop back to beginning
            std::cout << "Reached end of queue, looping to beginning." << std::endl;
        }
    }
    
    playCurrentSong();
}

void MusicPlayer::playPrevious() {
    if (currentQueue.empty()) {
        std::cout << "No songs in queue!" << std::endl;
        return;
    }
    
    if (queueMode == QueueMode::RANDOM) {
        randomPosition--;
        if (randomPosition < 0) {
            randomPosition = static_cast<int>(randomIndices.size()) - 1;
            std::cout << "Reached beginning of random queue, looping to end." << std::endl;
        }
        currentSongIndex = randomIndices[randomPosition];
    } else {
        currentSongIndex--;
        if (currentSongIndex < 0) {
            currentSongIndex = static_cast<int>(currentQueue.size()) - 1; // Loop to end
            std::cout << "Reached beginning of queue, looping to end." << std::endl;
        }
    }
    
    playCurrentSong();
}

void MusicPlayer::pauseResume() {
    if (audioPlayer.isPlaying()) {
        audioPlayer.pause();
        std::cout << "Paused" << std::endl;
        updateDiscordPresence();
    } else if (audioPlayer.isPaused()) {
        audioPlayer.resume();
        std::cout << "Resumed" << std::endl;
        updateDiscordPresence();
    } else {
        std::cout << "No song is currently playing." << std::endl;
    }
}

void MusicPlayer::stopPlayback() {
    audioPlayer.stop();
    std::cout << "Stopped" << std::endl;
    richPresence.setIdleState();
}

void MusicPlayer::setVolume(float volume) {
    savedVolume = volume;
    audioPlayer.setVolume(volume);
    std::cout << "Volume set to " << static_cast<int>(volume * 100) << "% (saved)" << std::endl;
}

void MusicPlayer::showCurrentSong() {
    if (currentSongIndex >= 0 && currentSongIndex < static_cast<int>(currentQueue.size())) {
        const Song& song = currentQueue[currentSongIndex];
        int displayIndex = getSongDisplayIndex(song);
        
        std::cout << "Current song: ";
        if (displayIndex > 0) {
            std::cout << displayIndex << ". ";
        }
        std::cout << song.getDisplayName() << std::endl;
        
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
        
        if (audioPlayer.getState() != PlaybackState::STOPPED) {
            std::cout << "Progress: " << audioPlayer.getProgressString() << std::endl;
        }
        
        std::string modeStr;
        switch (queueMode) {
            case QueueMode::ALL_SONGS:
                modeStr = "All Songs";
                break;
            case QueueMode::PLAYLIST:
                modeStr = "Playlist: " + currentPlaylistName;
                break;
            case QueueMode::RANDOM:
                if (!currentPlaylistName.empty()) {
                    modeStr = "Random - Playlist: " + currentPlaylistName;
                } else {
                    modeStr = "Random - All Songs";
                }
                break;
        }
        std::cout << "Mode: " << modeStr << std::endl;
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
    
    // Update random position if in random mode
    if (queueMode == QueueMode::RANDOM) {
        for (size_t i = 0; i < randomIndices.size(); ++i) {
            if (randomIndices[i] == index) {
                randomPosition = static_cast<int>(i);
                break;
            }
        }
    }
    
    playCurrentSong();
}

void MusicPlayer::setQueueFromAllSongs() {
    currentQueue = allSongs;
    queueMode = QueueMode::ALL_SONGS;
    currentPlaylistName.clear();
    currentSongIndex = -1;
    std::cout << "Switched to all songs mode (" << currentQueue.size() << " songs)" << std::endl;
    richPresence.setBrowsingState(static_cast<int>(allSongs.size()));
}

void MusicPlayer::setQueueFromPlaylist(const std::string& playlistName) {
    Playlist* playlist = PlaylistManager::getInstance().getPlaylist(playlistName);
    if (playlist && !playlist->isEmpty()) {
        currentQueue = playlist->getSongs();
        queueMode = QueueMode::PLAYLIST;
        currentPlaylistName = playlistName;
        currentSongIndex = -1;
        std::cout << "Queue set to playlist '" << playlistName << "' (" << currentQueue.size() << " songs)" << std::endl;
    } else {
        std::cout << "Playlist '" << playlistName << "' not found or empty!" << std::endl;
    }
}

void MusicPlayer::setRandomMode() {
    if (currentQueue.empty()) {
        std::cout << "No songs available for random mode!" << std::endl;
        return;
    }
    
    // Keep current queue but switch to random mode
    QueueMode previousMode = queueMode;
    queueMode = QueueMode::RANDOM;
    
    generateRandomIndices();
    
    // If we have a current song, find its position in the random order
    if (currentSongIndex >= 0 && currentSongIndex < static_cast<int>(currentQueue.size())) {
        // Find the current song in the random indices
        for (size_t i = 0; i < randomIndices.size(); ++i) {
            if (randomIndices[i] == currentSongIndex) {
                randomPosition = static_cast<int>(i);
                break;
            }
        }
    } else {
        // No current song, start from beginning of random order
        currentSongIndex = randomIndices[0];
        randomPosition = 0;
    }
    
    // Show appropriate message based on what we're randomizing
    if (previousMode == QueueMode::PLAYLIST) {
        std::cout << "Random mode enabled for playlist '" << currentPlaylistName << "' (" << currentQueue.size() << " songs)" << std::endl;
    } else {
        std::cout << "Random mode enabled for all songs (" << currentQueue.size() << " songs)" << std::endl;
    }
    
    // Start playing if we weren't already playing
    if (audioPlayer.getState() == PlaybackState::STOPPED) {
        std::cout << "Playing random song..." << std::endl;
        playCurrentSong();
    }
}

void MusicPlayer::generateRandomIndices() {
    randomIndices.clear();
    for (size_t i = 0; i < currentQueue.size(); ++i) {
        randomIndices.push_back(static_cast<int>(i));
    }
    
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(randomIndices.begin(), randomIndices.end(), g);
}

void MusicPlayer::clearQueue() {
    currentQueue.clear();
    currentSongIndex = -1;
    queueMode = QueueMode::ALL_SONGS;
    currentPlaylistName.clear();
}

void MusicPlayer::displayQueue() {
    if (currentQueue.empty()) {
        std::cout << "Queue is empty." << std::endl;
        return;
    }
    
    std::cout << "\nCurrent Queue";
    switch (queueMode) {
        case QueueMode::ALL_SONGS:
            std::cout << " (All Songs)";
            break;
        case QueueMode::PLAYLIST:
            std::cout << " (Playlist: " << currentPlaylistName << ")";
            break;
        case QueueMode::RANDOM:
            if (!currentPlaylistName.empty()) {
                std::cout << " (Random - Playlist: " << currentPlaylistName << ")";
            } else {
                std::cout << " (Random - All Songs)";
            }
            break;
    }
    std::cout << ":" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    if (queueMode == QueueMode::RANDOM) {
        // Show next few songs in random order
        int showCount = (std::min)(10, static_cast<int>(randomIndices.size()));
        for (int i = 0; i < showCount; ++i) {
            int pos = (randomPosition + i) % static_cast<int>(randomIndices.size());
            int songIdx = randomIndices[pos];
            const Song& song = currentQueue[songIdx];
            std::string marker = (i == 0) ? " -> " : "    ";
            
            // Show appropriate index based on context
            if (!currentPlaylistName.empty()) {
                // In playlist random mode, show playlist position
                std::cout << marker << (songIdx + 1) << ". " << song.getDisplayName() << std::endl;
            } else {
                // In all songs random mode, show global ID
                std::cout << marker << song.id << ". " << song.getDisplayName() << std::endl;
            }
        }
        if (randomIndices.size() > 10) {
            std::cout << "    ... and " << (randomIndices.size() - 10) << " more songs in random order" << std::endl;
        }
    } else {
        for (size_t i = 0; i < currentQueue.size(); ++i) {
            std::string marker = (static_cast<int>(i) == currentSongIndex) ? " -> " : "    ";
            int displayIndex = getSongDisplayIndex(currentQueue[i]);
            std::cout << marker << displayIndex << ". " << currentQueue[i].getDisplayName() << std::endl;
        }
    }
}

void MusicPlayer::updateDiscordPresence() {
    if (currentSongIndex >= 0 && currentSongIndex < static_cast<int>(currentQueue.size())) {
        const Song& song = currentQueue[currentSongIndex];
        bool isPlaying = audioPlayer.isPlaying();
        
        // Determine if we're in playlist mode (including random playlist)
        bool inPlaylist = (queueMode == QueueMode::PLAYLIST || 
                          (queueMode == QueueMode::RANDOM && !currentPlaylistName.empty()));
        std::string playlistName = inPlaylist ? currentPlaylistName : "";
        
        if (isPlaying) {
            richPresence.setSongPlaying(song.title, song.artist, inPlaylist, playlistName);
        } else {
            richPresence.setSongPaused(song.title, song.artist, inPlaylist, playlistName);
        }
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

void MusicPlayer::displaySongList(const std::vector<Song>& songs, bool showGlobalIndex) {
    if (songs.empty()) {
        std::cout << "No songs to display." << std::endl;
        return;
    }
    
    std::cout << "\nSongs (" << songs.size() << " total):" << std::endl;
    std::cout << "================================" << std::endl;
    
    for (size_t i = 0; i < songs.size(); ++i) {
        if (showGlobalIndex && songs[i].id > 0) {
            std::cout << songs[i].id << ". " << songs[i].getDisplayName() << std::endl;
        } else {
            std::cout << (i + 1) << ". " << songs[i].getDisplayName() << std::endl;
        }
        
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

void MusicPlayer::saveSettings() {
    std::ofstream file("settings.txt");
    if (file.is_open()) {
        file << "volume=" << savedVolume << std::endl;
        file << "show_progress=" << (showProgressTimer ? "1" : "0") << std::endl;
        file << "loop_mode=" << (loopCurrentSong ? "1" : "0") << std::endl;
        file.close();
    }
}

void MusicPlayer::loadSettings() {
    std::ifstream file("settings.txt");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("volume=") == 0) {
                try {
                    savedVolume = std::stof(line.substr(7));
                    if (savedVolume < 0.0f) savedVolume = 0.0f;
                    if (savedVolume > 1.0f) savedVolume = 1.0f;
                } catch (...) {
                    savedVolume = 1.0f;
                }
            } else if (line.find("show_progress=") == 0) {
                try {
                    showProgressTimer = (std::stoi(line.substr(14)) == 1);
                } catch (...) {
                    showProgressTimer = false;
                }
            } else if (line.find("loop_mode=") == 0) {
                try {
                    loopCurrentSong = (std::stoi(line.substr(10)) == 1);
                } catch (...) {
                    loopCurrentSong = false;
                }
            }
        }
        file.close();
    }
}

void MusicPlayer::toggleLoop() {
    loopCurrentSong = !loopCurrentSong;
    std::cout << "Loop mode " << (loopCurrentSong ? "enabled" : "disabled") << std::endl;
    if (loopCurrentSong) {
        std::cout << "Current song will repeat when finished" << std::endl;
    }
}

void MusicPlayer::checkCurrentSongInPlaylist(const std::string& playlistName) {
    // Check if we have a current song
    if (currentSongIndex < 0 || currentSongIndex >= static_cast<int>(currentQueue.size())) {
        std::cout << "No song is currently selected." << std::endl;
        return;
    }
    
    const Song& currentSong = currentQueue[currentSongIndex];
    
    // Get the playlist
    Playlist* playlist = PlaylistManager::getInstance().getPlaylist(playlistName);
    if (!playlist) {
        std::cout << "Playlist '" << playlistName << "' not found!" << std::endl;
        return;
    }
    
    // Check if current song is in the playlist
    const std::vector<Song>& playlistSongs = playlist->getSongs();
    bool found = false;
    int position = -1;
    
    for (size_t i = 0; i < playlistSongs.size(); ++i) {
        if (playlistSongs[i] == currentSong) {
            found = true;
            position = static_cast<int>(i + 1); // 1-based position
            break;
        }
    }
    
    // Display result
    std::cout << "Current song: " << currentSong.getDisplayName() << std::endl;
    
    if (found) {
        std::cout << "This song IS in playlist '" << playlistName << "' (position " << position << ")" << std::endl;
    } else {
        std::cout << "This song is NOT in playlist '" << playlistName << "'" << std::endl;
        std::cout << "  Use 'add " << playlistName << " " << currentSong.id << "' to add it" << std::endl;
    }
}

void MusicPlayer::update() {
    audioPlayer.update();
    
    // Check if song finished naturally (not manually stopped)
    if (audioPlayer.hasFinished() && currentSongIndex >= 0 && !currentQueue.empty()) {
        if (loopCurrentSong) {
            std::cout << "\n\nSong finished, looping current song..." << std::endl;
            playCurrentSong(); // Replay the same song
        } else {
            std::cout << "\n\nSong finished, auto-advancing to next..." << std::endl;
            playNext();
        }
        return; // Exit early after starting next song
    }
    
    // Small delay to prevent excessive CPU usage
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

bool MusicPlayer::hasInput() {
    // Cross-platform input checking
#ifdef _WIN32
    return _kbhit();
#else
    // Unix/Linux version
    fd_set readfds;
    struct timeval timeout;
    
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    
    return select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout) > 0;
#endif
}