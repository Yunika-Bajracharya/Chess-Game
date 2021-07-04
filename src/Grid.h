#pragma once

#include "Game.h"
#include "TextureManager.h"
#include <SDL2/SDL.h>

#define BLOCK_WIDTH 80
#define PIECE_VARIETY 12

enum pieceIndex {
  Wking = 1,
  Wqueen,
  Wbishop,
  Wknight,
  Wrook,
  Wpawn,
  Bking,
  Bqueen,
  Bbishop,
  Bknight,
  Brook,
  Bpawn,
};

class Grid {
public:
  Grid();
  ~Grid();

  void render();
  void update();

private:
  SDL_Texture *pieceTexture;
  int boardState[64];
};
