cmake_minimum_required(VERSION 4.0)
project(myipixel)

set(CMAKE_CXX_STANDARD 14)

add_executable(myipixel
        src/main.cpp
        src/web/device/raw/setLED.h
        src/iPixelDeviceRegistry.cpp
        .pio/libdeps/esp32s3dev/AsyncTCP/src/AsyncTCP.cpp
        src/web/Webserver.cpp
        src/web/device/raw/clear.h
        src/main.h
        src/iPixelDevice.cpp)
