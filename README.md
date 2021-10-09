# Agro
This is an object-oriented GUI library that I'm currently working on.
The drawing backend uses an OpenGL batch renderer.
Cross-platform system events and windowing is done using SDL2.
FreeType2 is used for making bitmaps for drawing text.

**Note that the library is very work in progress!**

![screenshot](images/screenshot_widget_gallery.webp)

![screenshot](images/screenshot_mhwi_db.webp)

## Build
The library is tested and working on Windows 10, Linux (Mint 20.2) and MacOS Big Sur.
You will need:
* C++14 compatible compiler
* CMake at least `3.16.3`
* SDL2 (Only on Linux. It's used for installing X11 and GL dependencies but I would like to change that in the future)
* Graphics card that supports OpenGL Core 3.3 at least (Tested on RX 5700XT, Intel HD 3000/4000, Apple M1 and AMD Ryzen R4 4500U)

On debian based system you should be able to install the dependencies like so:
```
sudo apt install g++ cmake libsdl2-dev
```

### Instructions
1. First make sure you are in the root directory of the project.
2. Run `cmake -S . -B build  -DCMAKE_BUILD_TYPE=RELEASE` in the terminal to build the required CMake files.
3. Building is slightly different using a multi-configuration setup like MSVC or XCode vs G++.
    * When using single-configuration use `cmake --build build` in the terminal to build the actual project.
    * When using multi-configuration use `cmake --build build --config Release` in the terminal to build the actual project.
4. You can also build all test and example programs by passing these flags `-DBUILD_ALL_TESTS=ON -DBUILD_ALL_EXAMPLES=ON` to the build command.

You can verify that the library has built correctly by running one of the example files in the `Agro/bin` folder.

### Installing
On Unix platforms you can run `cmake --install build` to install the library.
From then on you can link against the library by specifying its name `Agro` and including it like so `#include <Agro/application.hpp>`

**The instructions for Windows are incomplete for the moment!**
