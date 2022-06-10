# Dockerfile builds server image

FROM gcc:latest AS build

WORKDIR BattleShip
COPY ./message_status.h .
COPY ./server ./server

RUN apt-get update && \
    apt-get install -y \
      libpqxx-dev libsfml-dev\
      cmake ninja-build\
    && \
    cd ./server &&\
    cmake -DCMAKE_BUILD_TYPE=Release -G Ninja -S ./ -B ./build &&\
    cd ./build &&\
    cmake --build ./ --target ./BattleShipServer

FROM ubuntu:22.04
RUN apt-get update && \
    apt-get install -y \
      libpqxx-dev libsfml-dev\
	&& groupadd -r sample && useradd -r -g sample sample
USER sample
COPY --from=build BattleShip/server/build/BattleShipServer .
COPY --from=build BattleShip/server/config .
CMD ["./BattleShipServer"]
