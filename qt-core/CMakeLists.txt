cmake_minimum_required(VERSION 3.21)
project(pomo_core LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
# 編譯器：Windows = MSVC 2022、Linux = g++、macOS = Clang
if(WIN32 AND (NOT MSVC OR MSVC_VERSION LESS 1930))
  message(FATAL_ERROR "Windows 請使用 MSVC 2022")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  message(FATAL_ERROR "Linux 請使用 g++")
elseif(APPLE AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  message(FATAL_ERROR "macOS 請使用 Clang")
endif()
find_package(Qt6 6.8.3 REQUIRED COMPONENTS Widgets)
# CI 會設定 POMO_REQUIRE_TTS=ON：缺少 Qt Speech 模組就直接失敗，避免悄悄編出沒有語音的版本
option(POMO_REQUIRE_TTS "Qt Speech 模組為必要" OFF)
if(POMO_REQUIRE_TTS)
  find_package(Qt6 6.8.3 REQUIRED COMPONENTS TextToSpeech)
else()
  find_package(Qt6 6.8.3 QUIET COMPONENTS TextToSpeech)
endif()
# 標頭檔也要列入來源，AUTOMOC 才會處理放在 include/ 的 Q_OBJECT 類別
file(GLOB POMO_HEADERS CONFIGURE_DEPENDS include/pomo/*.h)
add_library(pomo_core STATIC ${POMO_HEADERS}
  src/AppInfo.cpp src/Settings.cpp src/Session.cpp src/Guardian.cpp src/History.cpp src/Stats.cpp
  src/Announcer.cpp src/Theme.cpp src/TimerRing.cpp src/StatsPage.cpp src/SettingsPage.cpp src/MainWindow.cpp src/AppEntry.cpp src/LauncherDialog.cpp src/I18n.cpp)
# 資源：多語系字串與視窗圖示（靜態函式庫的資源由 Qt 自動連進執行檔）
qt_add_resources(pomo_core pomo_resources PREFIX "/" BASE "${CMAKE_CURRENT_SOURCE_DIR}/.."
  FILES ../i18n/strings.json ../assets/tomato.png)
target_include_directories(pomo_core PUBLIC include)
target_link_libraries(pomo_core PUBLIC Qt6::Widgets)
if(TARGET Qt6::TextToSpeech)
  target_link_libraries(pomo_core PUBLIC Qt6::TextToSpeech)
  target_compile_definitions(pomo_core PUBLIC POMO_HAS_TTS=1)
endif()
if(MSVC)
  target_compile_options(pomo_core PUBLIC /utf-8 /permissive-)
endif()
