#pragma once

#include "Game.h"
#include "Structures.h"
#include <vector>

class Engine {
public:
  static bool setupFEN(State &state, const char *FENstring);

  static void handlePieceSelection(Coordinate &location, State &state,
                                   std::vector<Move> &moves);

  static void handlePiecePlacement(Coordinate &location, State &state,
                                   std::vector<Move> &moves, Move &lastMove);

  static bool generateLegalMoves(const State &state, std::vector<Move> &moves);
  static bool generatePseudoLegalMoves(const State &state,
                                       std::vector<Move> &moves);

  static bool isValidPieceLocation(const Coordinate &location,
                                   const State &state);

  static bool check(const State &state);
};
