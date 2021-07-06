#pragma once

#include "Game.h"
#include "TextureManager.h"

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
  void handleMouseButtonDown(SDL_Event &button);
  void handleMouseButtonUp(SDL_Event &button);

private:
  SDL_Texture *pieceTexture;
  int gridStartX;
  int gridStartY;

  int boardState[64];
  int dragSquare;
  int dragSquareValue;

  bool whiteTurn;
  bool castleAvailability[4]; // KQkq
  bool enPassantAvailable;
  int enPassant;
};
