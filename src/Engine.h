#pragma once

#include "Game.h"
#include "Structures.h"
#include <vector>

class Engine {
public:
  static bool generateMoves(const State &state, std::vector<Move> &moves);
  static bool isValidPieceLocation(const Coordinate &location,
                                   const State &state);
};
