cmake_minimum_required(VERSION 3.21)
project(BattleShipClient)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(BattleShipClient main.cpp screen_objects.cpp battle_ship_game.cpp game_field.cpp game_field_cell.cpp user.cpp config.cpp config.h)

find_package(SFML COMPONENTS system window graphics audio network REQUIRED)

target_link_libraries(BattleShipClient sfml-system sfml-window sfml-graphics sfml-audio sfml-network)
