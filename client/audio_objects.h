// Copyright 2022 Roman Ezhov. Github: RaEzhov

#ifndef CLIENT_AUDIO_OBJECTS_H_
#define CLIENT_AUDIO_OBJECTS_H_

#include <string>

#include <SFML/Audio.hpp>


class Sound {
 public:
  explicit Sound(const std::string &file);

  void play();

 private:
  sf::SoundBuffer buffer;
  sf::Sound sound;
};


#endif  // CLIENT_AUDIO_OBJECTS_H_
