#pragma once

#include "Game.h"
#include "TextureManager.h"
#include <SDL2/SDL.h>

#define BLOCK_WIDTH 80
#define PIECE_VARIETY 12

#define ASCII_OFFSET 48

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

  bool setupFEN(const char *FENstring);

  void render();
  void update();
  void handleClick();

private:
  SDL_Texture *pieceTexture;

  int boardState[64];
  int dragSquare;

  bool whiteTurn;
  bool castleAvailability[4]; // KQkq
  bool enPassantAvailable;
  int enPassant;
};
