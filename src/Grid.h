#pragma once

#include "Game.h"
#include "Player.h"
#include "TextureManager.h"
#include <vector>

#define BLOCK_WIDTH 80
#define PIECE_VARIETY 12

#define ASCII_OFFSET 48

struct Move {
  bool made;
  int start;
  int end;
};

struct Coordinate {
  int i, j;
};

enum pieceIndex {
  empty = 0,
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

  // Looks at the current dragSquare and dragSquareValue and generates moves.
  bool generateMoves();

  static bool isValidPieceLocation(Coordinate &location);

private:
  SDL_Texture *pieceTexture;
  Mix_Chunk *moveSound;
  int gridStartX;
  int gridStartY;

  pieceIndex boardState[64];
  std::vector<Move> moves;
  Player players[2];

  int dragSquare;
  pieceIndex dragSquareValue;
  Move lastMove;

  bool whiteTurn;
  bool castleAvailability[4]; // KQkq
  bool enPassantAvailable;
  int enPassant;
};
