# JaDraw

My C++ software graphics library for drawing to a bitmap. Good for embedded devices with enough to hold a full 24-bit framebuffer.

This gives functions that let you draw to a given bitmap. How to display the resulting image is up to your specific application.

It's inspired by [Olive.c](https://tsoding.github.io/olive.c/) and [Adafruit GFX Library](https://learn.adafruit.com/adafruit-gfx-graphics-library/overview). They each have certain quirks I don't like and lack some drawing functions I want, so I made my own.

## Development setup

After cloning the repo, you need to put SDL in here.

1. First, download [SDL2-2.30.11.zip](https://github.com/libsdl-org/SDL/releases/download/release-2.30.11/SDL2-2.30.11.zip) from [SDL's 2.30.11 release page on GitHub](https://github.com/libsdl-org/SDL/releases/tag/release-2.30.11) and extract it.
1. Go in SDL2's x86_64-w64-mingw32 directory and copy the "include" and "lib" folders into this project under the SDL2 folder.
1. Go in x86_64-w64-mingw32's bin folder and copy SDL2.dll and paste it next to main.cpp in this project.
1. Now you can start debugging in VSCode (e.g. by pressing F5).
