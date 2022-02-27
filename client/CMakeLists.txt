cmake_minimum_required(VERSION 3.21)
project(BattleShipClient)

set(CMAKE_CXX_STANDARD 20)

add_executable(BattleShipClient main.cpp)

find_package(SFML COMPONENTS system window graphics audio network REQUIRED)

target_link_libraries(BattleShipClient sfml-system sfml-window sfml-graphics sfml-audio sfml-network)