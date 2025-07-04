# osu!Listener


A console-based music player specifically designed for osu! beatmap collections. Automatically scans your osu! Songs folder and allows you to create custom playlists and control playback with simple commands.

## Features

- **Automatic Song Discovery**: Scans `%LOCALAPPDATA%/osu!/Songs` for all beatmaps
- **Duplicate Handling**: Automatically filters out duplicate songs with different IDs
- **Custom Playlists**: Create, manage, and play custom playlists
- **Playback Controls**: Play, pause, stop, next, previous, volume control
- **Search Functionality**: Search through your song collection
- **Queue Management**: View and manage the current playback queue

   ### Can now handle Discord Rich Presence !

## Requirements

- Windows (99% sure you got this requirement)
- x64 Native Tools for VS 2022 | [Download Here](https://visualstudio.microsoft.com/visual-cpp-build-tools/)
- FMOD Engine Library | [Download Here](https://www.fmod.com/download)

## Installation for Users

1. **Download the latest [release](https://github.com/Vapoor/osulistener/releases)**

2. **Uncompress the .rar and simply start the .exe !**



## Installation for Developpers

### FMOD Lib

1. **Uncompress the folder into lib folder**
   - You **have** to create this lib folder. The path expected should be ```root/lib/FMOD Studio API Windows```.

### Using .bat file

1. **Open x64 Native Tools Command Prompt for VS 2022**

2. **Build the project**:
   ```bash
   build_msvc.bat
   ```
## Usage

1. **Run the program**:
   ```
   ./bin/osuListener.exe
   ```
2. **Basic Commands**:
   - `help` - Show all available commands
   - `scan` - Scan osu! songs directory
   - `list` - Show all discovered songs
   - `play <number>` - Play song by index
   - `pause` - Pause/resume playback
   - `next` - Next song
   - `prev` - Previous song
   - `stop` - Stop playback

3. **Playlist Management**:
   - `playlists` - Show all playlists
   - `create <name>` - Create new playlist
   - `add <playlist> <song_index>` - Add song to playlist
   - `playlist <name>` - Play entire playlist
   - `show <name>` - Show playlist contents

4. **Other Commands**:
   - `search <query>` - Search for songs
   - `vol <0-100>` - Set volume
   - `current` - Show current song info
   - `all` - Switch back to all songs mode
   - `random` - Enable random mode
   - `timer` - Toggle progress timer display
   - `queue` - Show current playback queue
   - `quit` - Exit program

## Example Usage Session

```
> scan
Scanning osu! songs directory...
Found 150 unique songs.

> list
Songs (150 total):
1. Camellia - Ghost
2. DragonForce - Through the Fire and Flames
3. Hatsune Miku - Senbonzakura
...

> create favorites
Created playlist: favorites

> add favorites 1
Added 'Camellia - Ghost' to playlist 'favorites'


> playlist favorites
Queue set to playlist 'favorites' (1 songs)
Playing: Camellia - Ghost

> pause
Paused

> next
Reached end of queue, looping to beginning.
Playing: Camellia - Ghost
```

- **Without FMOD**: The program will still work but will only simulate audio playback (no actual sound)
- **Playlist Persistence**: Playlists are automatically saved to `playlists.txt` and loaded on startup
- **osu! Path Detection**: Automatically detects the standard osu! installation path
- **Memory Usage**: Designed to handle large song collections efficiently

## Setup Discord Rich Presence

If you want to show to everyone that you are using my application (which is kinda cool right ?), here is all the steps.

1. Install to official [Discord SDK Game library](https://discord.com/developers/docs/developer-tools/game-sdk). (This app was tested using the 3.2.1 version only).

2. Extraction the library into the project into lib folder. **You need to create the lib folder if not already done**. The path should be ```root/lib/discord_sdk_game```


Now rebuild the app, the .bat should inform you if he found or not the SDK. If yes, you should now see your profile changing based on what your listening !

## Troubleshooting

### "osu! Songs folder not found"
- Ensure osu! is installed in the standard location (`%LOCALAPPDATA%/osu!/`)
- Check that the Songs folder exists and contains beatmap folders
- **osu!Lazer is not supported for now**

### "FMOD not available" or no audio
- Download and properly install FMOD as described in the installation steps
- Ensure `fmod.dll` is in the same directory as the executable
- Rebuild the project after adding FMOD

### Build errors
- Ensure you're using a C++17 compatible compiler
- Check that all source files are in the same directory
- For Visual Studio: use "x64" configuration, not "x86"


## License

This project is for educational and personal use. FMOD has its own licensing terms for commercial use.

## Authors

This project got maded by Vaporr for educational purpose.