# OpenGLGUI
This is an object-oriented GUI library that I'm currently working on.
The drawing backend uses an OpenGL batch renderer.
Cross-platform system events and windowing is done using SDL2.
FreeType2 is used for making bitmaps for drawing text.

**Note that the library is very work in progress!**

![screenshot](images/screenshot_mhwi_db.png)

## Build
The library is tested and working on Windows 10 and Linux (Mint 20.1).
You will need:
* C++14 compatible version of g++
* CMake at least `3.13.4`
* SDL2
* FreeType2
* Graphics card that supports OpenGL Core 3.3 at least (Tested on RX 5700XT and Intel HD 4000)

On debian based system you should be able to install the dependencies like so:
```
apt install cmake
apt install libfreetype6-dev
apt install libsdl2-dev
```

### Instructions
**Note that the project builds in release mode and as a static lib by default!**
1. First make sure you are in the root directory of the project.
2. Run `cmake -S . -B build` in the terminal to build the required CMake files.
3. Run `cmake --build build` in the terminal to build the actual project.

You can verify that the library has built correctly by running one of the example files.
For example: in the terminal from the root directory run the `gui/bin/example_treeview` executable.
For the moment this is required because we don't embed any fonts yet.

### Installing
On Unix platforms you can run `cmake --install build` to install the library.
From then on you can link against the library by specifying its name `gui` (Subject to change).

**The instructions for Windows are incomplete for the moment!**
