cmake_minimum_required(VERSION 3.16)
project(BattleShipGame)

add_subdirectory(client)

IF("${CMAKE_SYSTEM}" MATCHES "Linux")
    add_subdirectory(server)
ENDIF()

add_subdirectory(tests)
