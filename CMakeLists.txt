cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
# "Trim" the build. Include the minimal set of components, main, and anything it depends on.
idf_build_set_property(MINIMAL_BUILD ON)


# --- Build Variant Selectie ---
# Standaard staat hij op 'matrix'
set(BUILD_VARIANT "matrix" CACHE STRING "Kies build variant: matrix of timer")

if(BUILD_VARIANT STREQUAL "timer")
    add_definitions(-DDEVICE_COOKING_TIMER)
    message(STATUS ">>> Building for: COOKING TIMER <<<")
    # Hier kun je later specifieke timer-bestanden toevoegen
    set(SOURCE_FILES src/main.cpp src/TimerLogic.cpp)
else()
    add_definitions(-DDEVICE_IPIXEL_MATRIX)
    message(STATUS ">>> Building for: IPIXEL MATRIX <<<")
    set(SOURCE_FILES src/main.cpp src/MatrixLogic.cpp)
endif()

# Gebruik de gekozen source files
add_executable(${PROJECT_NAME}.elf ${SOURCE_FILES})

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
        src/png/MyFont.cpp
        src/png/MyFont.h
        src/functions/MatrixContext.cpp
        src/functions/MatrixContext.h
)
