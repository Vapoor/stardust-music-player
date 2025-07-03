@echo off
echo Building osu! Music Listener with MSVC (64-bit)...

:: Create directories
if not exist build mkdir build
if not exist bin mkdir bin

:: Check FMOD installation and set paths
if exist "lib\FMOD Studio\api\core\inc\fmod.h" (
    set FMOD_INC=lib\FMOD Studio\api\core\inc
    set FMOD_LIB=lib\FMOD Studio\api\core\lib\x64
    echo Found FMOD at: lib\FMOD Studio
) else if exist "lib\FMOD Studio API Windows\api\core\inc\fmod.h" (
    set FMOD_INC=lib\FMOD Studio API Windows\api\core\inc
    set FMOD_LIB=lib\FMOD Studio API Windows\api\core\lib\x64
    echo Found FMOD at: lib\FMOD Studio API Windows
) else (
    echo ERROR: FMOD not found!
    echo Please ensure FMOD is installed in one of these locations:
    echo   - lib\FMOD Studio\api\core\inc\fmod.h
    echo   - lib\FMOD Studio API Windows\api\core\inc\fmod.h
    dir lib
    pause
    exit /b 1
)

:: Check Discord SDK (optional)
set DISCORD_FLAGS=
set DISCORD_LIBS=
if exist "lib\discord_game_sdk\cpp\discord.h" (
    set DISCORD_INC=lib\discord_game_sdk\cpp
    set DISCORD_LIB=lib\discord_game_sdk\lib\x86_64
    set DISCORD_FLAGS=/DDISCORD_AVAILABLE /I"%DISCORD_INC%"
    set DISCORD_LIBS=discord_game_sdk.dll.lib
    echo Found Discord SDK - Rich Presence enabled
) else (
    echo Discord SDK not found - Rich Presence will be simulated
)

:: Check if FMOD library exists
if not exist "%FMOD_LIB%\fmod_vc.lib" (
    echo ERROR: fmod_vc.lib not found at %FMOD_LIB%
    echo Available files:
    dir "%FMOD_LIB%"
    pause
    exit /b 1
)

:: List all source files explicitly (including new Discord file)
echo Compiling source files (64-bit)...
cl /std:c++17 /EHsc /O2 /nologo /DFMOD_AVAILABLE %DISCORD_FLAGS% ^
   /I"headers" /I"%FMOD_INC%" ^
   /c src\audioPlayer.cpp src\main.cpp src\musicPlayer.cpp src\playlist.cpp src\songScanner.cpp src\discordPresence.cpp ^
   /Fo:build\ ^
   /favor:AMD64

if errorlevel 1 (
    echo Compilation failed!
    pause
    exit /b 1
)

:: Link using 64-bit linker with all object files
echo Linking (64-bit)...
if defined DISCORD_LIBS (
    link /nologo /MACHINE:X64 ^
         build\audioPlayer.obj build\main.obj build\musicPlayer.obj build\playlist.obj build\songScanner.obj build\discordPresence.obj ^
         /LIBPATH:"%FMOD_LIB%" fmod_vc.lib ^
         /LIBPATH:"%DISCORD_LIB%" %DISCORD_LIBS% ^
         /OUT:bin\osuListener.exe
) else (
    link /nologo /MACHINE:X64 ^
         build\audioPlayer.obj build\main.obj build\musicPlayer.obj build\playlist.obj build\songScanner.obj build\discordPresence.obj ^
         /LIBPATH:"%FMOD_LIB%" fmod_vc.lib ^
         /OUT:bin\osuListener.exe
)

if errorlevel 1 (
    echo Linking failed!
    pause
    exit /b 1
)

:: Copy required DLLs
echo Copying required DLLs...
if exist "%FMOD_LIB%\fmod.dll" (
    copy "%FMOD_LIB%\fmod.dll" "bin\" >nul
    echo fmod.dll copied successfully
) else (
    echo Warning: fmod.dll not found at %FMOD_LIB%
)

if exist "%DISCORD_LIB%\discord_game_sdk.dll" (
    copy "%DISCORD_LIB%\discord_game_sdk.dll" "bin\" >nul
    echo discord_game_sdk.dll copied successfully
) else (
    echo Discord SDK DLL not found - Rich Presence will be simulated
)

echo.
echo Build successful! 
echo.
echo Features enabled:
echo - FMOD Audio: YES
if defined DISCORD_LIBS (
    echo - Discord Rich Presence: YES
) else (
    echo - Discord Rich Presence: Simulated ^(Discord SDK not found^)
)
echo.
echo Run: bin\osuListener.exe
echo.
pause