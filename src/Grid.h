#pragma once

#include "Game.h"
#include <SDL2/SDL.h>

#define BLOCK_WIDTH 80

class Grid {
public:
  Grid();
  ~Grid();

  void render();
  void update();
};
