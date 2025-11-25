cmake_minimum_required(VERSION 4.0)
project(myipixel)

set(CMAKE_CXX_STANDARD 14)

add_executable(myipixel
        src/main.cpp
        src/iPixelDeviceRegistry.cpp
        .pio/libdeps/esp32s3dev/AsyncTCP/src/AsyncTCP.cpp
        src/web/Webserver.cpp
        src/main.h
        src/iPixelDevice.cpp
        src/global.h
        src/png/pngmaker.h
        src/png/pngmaker.cpp
        lib/PNGenc/PNGenc.cpp
        src/png/cBitmap.cpp
        src/png/cBitmap.h
        src/cgif/cgif.c)
