cmake_minimum_required(VERSION 3.21)
project(BattleShipServer)

set(CMAKE_CXX_STANDARD 20)

add_executable(BattleShipServer main.cpp db_connection.cpp logger.cpp config.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpqxx -lpq -pthread")

set(PQXX /usr/local/include/pqxx)

find_library(PQXX_LIB pqxx)
find_library(PQ_LIB pq)

find_package(SFML COMPONENTS system network REQUIRED)

TARGET_LINK_LIBRARIES(BattleShipServer  ${PQXX_LIB} ${PQ_LIB}  sfml-system sfml-network)
