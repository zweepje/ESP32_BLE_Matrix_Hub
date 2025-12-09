cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
idf_build_set_property(MINIMAL_BUILD ON)

project(myipixel)

set(CMAKE_CXX_STANDARD 17)

add_executable(myipixel
        src/main.cpp
        src/iPixelDeviceRegistry.cpp
        .pio/libdeps/esp32s3dev/AsyncTCP/src/AsyncTCP.cpp
        src/web/Webserver.cpp
        src/main.h
        src/iPixelDevice.cpp
        src/global.h
        src/cgif/cgif.c
        src/initfs.c
        src/cgif/cgif_raw.c
        src/png/IndexedBitmap.cpp
        src/png/IndexedBitmap.h
        src/functions/temperature.cpp
         src/functions/temperature.h
        src/png/GifMaker.cpp
        src/png/GifMaker.h
        src/clock/timefunctions.cpp
        src/clock/timefunctions.h
        src/functions/Animation.cpp
        src/functions/Animation.h
        src/png/LetterDraw.cpp
        src/png/LetterDraw.h
        src/png/letterbitmap.h
)
