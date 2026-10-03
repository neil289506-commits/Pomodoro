cmake_minimum_required(VERSION 3.21)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "macOS 通用版：Apple Silicon + Intel")
project(pomodoro_macos LANGUAGES CXX OBJCXX)  # .mm 需要啟用 OBJCXX
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_OBJCXX_STANDARD 20)
set(CMAKE_OBJCXX_STANDARD_REQUIRED ON)
add_subdirectory(../qt-core qt-core)
add_executable(pomodoro src/main.cpp src/PlatformVolume.cpp src/PlatformForegroundWatcher.mm)
target_link_libraries(pomodoro PRIVATE pomo_core)
target_link_libraries(pomodoro PRIVATE "-framework AppKit")  # NSWorkspace 前景 App 偵測
