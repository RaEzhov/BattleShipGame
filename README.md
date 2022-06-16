<h1 align="center">BattleShip C++ Game</h1>

<h2 align="center">Tech Stack</h2>
<div align="center">  

<img src="https://img.shields.io/badge/Client-C++%20SFML-blue">
<img src="https://img.shields.io/badge/Server-C++%20SFML%20libpqxx-orange">
<img src="https://img.shields.io/badge/Database-PostgeSQL-purple">
<img src="https://img.shields.io/badge/Tests-GoogleTest-249">
</div>

<h2 align="center"> Status </h2>
<div align="center">

[![CMakeBuild](https://github.com/RaEzhov/BattleShipGame/actions/workflows/build.yml/badge.svg?branch=master)](https://github.com/RaEzhov/BattleShipGame/actions/workflows/build.yml)
[![GoogleTests](https://github.com/RaEzhov/BattleShipGame/actions/workflows/gtests.yml/badge.svg?branch=master)](https://github.com/RaEzhov/BattleShipGame/actions/workflows/gtests.yml)
[![CppLint](https://github.com/RaEzhov/BattleShipGame/actions/workflows/linter.yml/badge.svg?branch=master)](https://github.com/RaEzhov/BattleShipGame/actions/workflows/linter.yml)
[![DockerBuild](https://github.com/RaEzhov/BattleShipGame/actions/workflows/docker.yml/badge.svg?branch=master)](https://github.com/RaEzhov/BattleShipGame/actions/workflows/docker.yml)
<br>
[![PullRequests](https://img.shields.io/github/issues-pr/RaEzhov/BattleShipGame)](https://github.com/RaEzhov/BattleShipGame/pulls)
[![Issues](https://img.shields.io/github/issues/RaEzhov/BattleShipGame)](https://github.com/RaEzhov/BattleShipGame/issues)
</div>

<h2 align="center"> About </h2>

This is client-server application. Server must listen clients and do requests to database. With client you can play Battleship.  
   

<h2 align="center"> How to use </h2>

**If you want to play with some people**
- Just download [installer](https://disk.yandex.ru/d/9wMulFnfzx02rg)
- Install it on your PC
- Enjoy game!
  
  
**If you want to start server on your machine, you must**

- Install [Docker](https://www.docker.com/)
- Start and set up the [PostgeSQL](https://www.postgresql.org/) database*
- Download files [docker-compose.yml](https://raw.githubusercontent.com/RaEzhov/BattleShipGame/master/docker-compose.yml)
and [.env](https://raw.githubusercontent.com/RaEzhov/BattleShipGame/master/.env)
- Edit `.env` file with your database settings 
- Run `docker-compose up -d` in directory with this files
- In client app folder you need to open `config` file and type your server address 
- And now the server is running!
  
  
  
  
*Your database must contains this tables and table `users` must contains first row with id == 0
<br>
<img src="https://user-images.githubusercontent.com/79147155/173154220-e436e4d6-b2f6-4a09-8324-8cc7b1d9f109.png" alt="battleship_db" width="300px"/>

<h2 align="center"> Textures </h2>

**These are ships textures:**  

![Ship4_a](https://user-images.githubusercontent.com/79147155/173153700-873f148d-2f3e-4406-b27b-a36c9aad99dc.png)
![Ship3_a](https://user-images.githubusercontent.com/79147155/173153720-c6050b40-b89f-4156-a33c-315bd15d7312.png)
![Ship2_a](https://user-images.githubusercontent.com/79147155/173153738-d29ec788-f848-4eed-834c-96267c781927.png)
![Ship1_a](https://user-images.githubusercontent.com/79147155/173153750-bef10fba-52cc-4e79-8855-e471a79bb9c3.png)
![Ship4_i](https://user-images.githubusercontent.com/79147155/173153796-4675d8ab-e787-4be5-a54b-670409ea61ac.png)
![Ship3_i](https://user-images.githubusercontent.com/79147155/173153881-85a10888-b0df-4ab3-868c-0e26946c28d2.png)
![Ship2_i](https://user-images.githubusercontent.com/79147155/173153895-2f383129-d386-4b57-9696-108b9383299e.png)
![Ship1_i](https://user-images.githubusercontent.com/79147155/173153973-df97df20-5087-45bb-9216-92c762b3fb88.png)
![Ship4_d](https://user-images.githubusercontent.com/79147155/173153987-f84ba4db-fa03-440c-9e6b-545b1c654183.png)
![Ship3_d](https://user-images.githubusercontent.com/79147155/173153810-e5188a9d-2993-4210-b052-f3456ebe2cc2.png)
![Ship2_d](https://user-images.githubusercontent.com/79147155/173153999-2fe4a8cd-8491-4da9-983a-2c6456ebbc1c.png)
![Ship1_d](https://user-images.githubusercontent.com/79147155/173154008-e1786633-7dd1-43f3-b933-772a0da37b45.png)



<h2 align="center"> Screenshots </h2>

![Screenshot from 2022-06-11 00-15-36](https://user-images.githubusercontent.com/79147155/173152304-77f80193-9a72-4567-b1d3-d409617281b3.png)

![Screenshot from 2022-06-11 00-11-10](https://user-images.githubusercontent.com/79147155/173151875-b0ec0c52-b6fd-489a-a174-0b3140b0291f.png)

![Screenshot from 2022-06-11 00-14-53](https://user-images.githubusercontent.com/79147155/173152227-53f530a1-6903-4859-9895-29dab72dce91.png)
