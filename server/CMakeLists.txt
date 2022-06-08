cmake_minimum_required(VERSION 3.16)
project(BattleShipServer)

set(CMAKE_CXX_STANDARD 20)

add_executable(BattleShipServer main.cpp db_connection.cpp logger.cpp config.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpqxx -lpq -pthread")

set(PQXX /usr/local/include/pqxx)

find_library(PQXX_LIB pqxx)
find_library(PQ_LIB pq)

find_package(SFML COMPONENTS system network REQUIRED)

target_link_libraries(BattleShipServer  ${PQXX_LIB} ${PQ_LIB}  sfml-system sfml-network)
