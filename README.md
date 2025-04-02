# osu!Listener

osu!Listener is a project made by my own as a side project. It got mainly created for a personnal usage but feel free to give it a try.  
This application is allowing you to listen to your Osu! beatmaps. You can create playlist and retrieve them later at each restart.
**This project is probably far from being "clean", but we're all starting somewhere :)**

## Visuals

- Empty for now, mainly console based.

## Setup

To run **osu!Listener**, you need the following libraries installed on your system:

### **Required Libraries:**

1. **SDL2**  
   SDL2 is used for handling graphics, events, and audio.

   - On **Linux** (Ubuntu/Debian):
     ```sh
     sudo apt-get install libsdl2-dev
     ```

   - On **Windows** (using MSYS2 with UCRT64):
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

   - On **Windows** (using MSYS2 with UCRT64):
     Install it using:
     ```sh
     pacman -S mingw-w64-ucrt-x86_64-SDL2_mixer
     ```


## Start the application

1. **Build the application using makefile.**
    ```build
      make 
    ```
2. **Start the application**
    ```start
    ./bin/osuListener.exe </Path/To/Osu/Songs>
    ```

## FAQ

- **How it works ?**  
For now, the application looking at the path given, and run a program to look all the mp3 found , even in the folders, so if you give the root of your computer, except a long long time waiting.

- **Can the app be used on Linux and on Windows ?**   
This app was built for Linux only, you can still launch it into a Windows if you install MSYS2. I used UCRT64 to create my config.  
For the rest simply download the [depedencies](#Setup) then use make.

- **Can it supports others folder than Songs from Osu?**  
Yes it can theoretically, but I might not have fix every issues. So don't be suprised if you got an unwanted result.

- **Which osu is supported ?**  
For now, the application only supporting **stable version** of osu!, mainly because Lazer is still a WIP thing, and songs handling could change at any moment.

## Authors

This application has been only developped by myself (Vapoor)

