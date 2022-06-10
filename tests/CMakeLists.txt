cmake_minimum_required(VERSION 3.16)
project(BattleShipTests)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/e2239ee6043f73722e7aa812a459f54a28552929.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


enable_testing()

IF("${CMAKE_SYSTEM}" MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
ELSE()
    set(SFML_STATIC_LIBRARIES TRUE)
    set(SFML_ROOT D:/Projects/SFML_test/SFML-2.5.1/)
ENDIF()

find_package(SFML 2.5.1 COMPONENTS system window graphics audio network REQUIRED)


add_executable(BattleShipTests main.cpp ../client/user.cpp ../client/screen_objects.cpp ../client/config.cpp)
target_link_libraries(BattleShipTests gtest_main)

include_directories(../)
include_directories(../../)

include(GoogleTest)
gtest_discover_tests(BattleShipTests)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread")

target_link_libraries(BattleShipTests sfml-system sfml-window sfml-graphics sfml-audio sfml-network)
