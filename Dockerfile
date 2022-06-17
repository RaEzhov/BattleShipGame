# Dockerfile builds server image

FROM gcc:latest AS build

WORKDIR BattleShip
COPY ./global ./global
COPY ./server ./server

RUN apt-get update && \
    apt-get install -y libpqxx-dev libsfml-dev cmake ninja-build  && \
    cd ./server && \
    cmake -DCMAKE_BUILD_TYPE=Release -G Ninja -S ./ -B ./build && \
    cd ./build && \
    cmake --build ./ --target ./BattleShipServer

FROM ubuntu:22.04

ENV BATTLESHIP_SERVER_PORT=$BATTLESHIP_SERVER_PORT
ENV BATTLESHIP_SERVER_DBIP=$BATTLESHIP_SERVER_DBIP
ENV BATTLESHIP_SERVER_DBPORT=$BATTLESHIP_SERVER_DBPORT
ENV BATTLESHIP_SERVER_DBUSER=$BATTLESHIP_SERVER_DBUSER
ENV BATTLESHIP_SERVER_DBPASSWORD=$BATTLESHIP_SERVER_DBPASSWORD

RUN apt-get update && \
    apt-get install -y libpqxx-dev libsfml-dev && \
	groupadd -r sample && \
    useradd -r -g sample sample
USER sample
COPY --from=build BattleShip/server/build/BattleShipServer .
COPY --from=build BattleShip/server/config* ./
CMD ["./BattleShipServer"]
