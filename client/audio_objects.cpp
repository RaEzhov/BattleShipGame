// Copyright 2022 Roman Ezhov. Github: RaEzhov

#include "client/config.h"
#include "client/audio_objects.h"


Sound::Sound(const std::string &file) {
  if (!buffer.loadFromFile(Config::instance().resources + file)) {
    throw std::runtime_error("Load sound error!\n");
  }
  sound.setBuffer(buffer);
  sound.setVolume(70);
}

void Sound::play() {
  sound.play();
}
