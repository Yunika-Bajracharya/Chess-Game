#pragma once

#include "Engine.h"
#include "Game.h"
#include "Player.h"
#include "Structures.h"
#include "TextureManager.h"
#include <vector>

#define BLOCK_WIDTH 80
#define PIECE_VARIETY 12

#define ASCII_OFFSET 48

class Grid {
public:
  Grid();
  ~Grid();

  bool setupFEN(const char *FENstring);

  void render();
  void update();

  void handleMouseButtonDown(SDL_Event &button);
  void handlePieceSelection(Coordinate &location);

  void handleMouseButtonUp(SDL_Event &button);
  void handlePiecePlacement(Coordinate &location);

  // Looks at the current dragSquare and dragSquareValue and generates moves.
  bool generateMoves();

  bool isValidPieceLocation(const Coordinate &location);
  void filterLegalMoves();

private:
  SDL_Texture *pieceTexture;
  Mix_Chunk *moveSound;
  int gridStartX;
  int gridStartY;

  std::vector<Move> moves;
  State state;

  Move lastMove;
  Player players[2];
};
