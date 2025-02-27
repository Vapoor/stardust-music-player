# Osu!Listener

This C++ app will allow you to listen to all the musics from yours beatmaps

## Example

## Setup

To run **osuListener**, you need the following libraries installed on your system:

### **Required Libraries:**

1. **SDL2**  
   SDL2 is used for handling graphics, events, and audio.

   - On **Linux** (Ubuntu/Debian):
     ```sh
     sudo apt-get install libsdl2-dev
     ```

   - On **Windows** (MSYS2 with UCRT64):
     You can install SDL2 through **MSYS2** using the following command:
     ```sh
     pacman -S mingw-w64-ucrt-x86_64-SDL2
     ```

2. **SDL2_mixer**  
   SDL2_mixer is used for handling audio formats like MP3 and WAV.

   - On **Linux** (Ubuntu/Debian):
     ```sh
     sudo apt-get install libsdl2-mixer-dev
     ```

   - On **Windows** (MSYS2 with UCRT64):
     Install it using:
     ```sh
     pacman -S mingw-w64-ucrt-x86_64-SDL2_mixer
     ```


## Start the application

1. **Build the application using makefile.**
    ```build
      make win # For Windows
      make lin # For Linux
    ```
2. **Start the application**
    ```start
    ./bin/osuListener.exe
    ```

## FAQ

**Which type of osu is supported ?**  
For now, the app is just looking for mp3 into a folder given into main, meaning you can do with osu! without any problems or with Lazer if you give TempFiles Folder, but **I would not recommand for now**.

**Can the app be used on Linux and on Windows ?**   
This app was built for Windows and Linux, simply download the [depedencies](#Setup) then use make.


## Author

This application has been only developped by myself (Vapoor)

