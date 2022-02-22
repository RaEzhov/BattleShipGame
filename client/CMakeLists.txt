cmake_minimum_required(VERSION 3.17)
project(BattleShipClient)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-static")

add_executable(BattleShipClient main.cpp)

set(SFML_STATIC_LIBRARIES TRUE)

set(SFML_DIR C:/SFML_MINGW/SFML-2.5.1/lib/cmake/SFML)

find_package(SFML COMPONENTS system window graphics audio network REQUIRED)

include_directories(C:/SFML_MINGW/SFML-2.5.1/include)

target_link_libraries(BattleShipClient sfml-system sfml-window sfml-graphics sfml-audio sfml-network)