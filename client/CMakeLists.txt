cmake_minimum_required(VERSION 3.16)
project(BattleShipClient)

set(CMAKE_CXX_STANDARD 17)

IF("${CMAKE_SYSTEM}" MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
ELSE()
    set(SFML_STATIC_LIBRARIES TRUE)
    set(SFML_ROOT D:/Projects/SFML_test/SFML-2.5.1/)
ENDIF()

include_directories(../)

add_executable(BattleShipClient main.cpp screen_objects.cpp battle_ship_game.cpp game_field.cpp game_field_cell.cpp user.cpp config.cpp audio_objects.cpp)

find_package(SFML 2.5.1 COMPONENTS system window graphics audio network REQUIRED)

target_link_libraries(BattleShipClient sfml-system sfml-window sfml-graphics sfml-audio sfml-network)
