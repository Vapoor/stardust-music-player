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

:: Check if FMOD library exists
if not exist "%FMOD_LIB%\fmod_vc.lib" (
    echo ERROR: fmod_vc.lib not found at %FMOD_LIB%
    echo Available files:
    dir "%FMOD_LIB%"
    pause
    exit /b 1
)

:: Compile all source files for 64-bit
echo Compiling source files (64-bit)...
cl /std:c++17 /EHsc /O2 /nologo /DFMOD_AVAILABLE ^
   /I"headers" /I"%FMOD_INC%" ^
   /c src\*.cpp /Fo:build\ ^
   /favor:AMD64

if errorlevel 1 (
    echo Compilation failed!
    pause
    exit /b 1
)

:: Link using 64-bit linker
echo Linking (64-bit)...
link /nologo /MACHINE:X64 build\*.obj ^
     /LIBPATH:"%FMOD_LIB%" fmod_vc.lib ^
     /OUT:bin\osuListener.exe

if errorlevel 1 (
    echo Linking failed!
    pause
    exit /b 1
)

:: Copy FMOD DLL
echo Copying FMOD DLL...
if exist "%FMOD_LIB%\fmod.dll" (
    copy "%FMOD_LIB%\fmod.dll" "bin\" >nul
    echo fmod.dll copied successfully
) else (
    echo Warning: fmod.dll not found at %FMOD_LIB%
)

echo.
echo Build successful! 
echo Run: bin\osuListener.exe
echo.
pause