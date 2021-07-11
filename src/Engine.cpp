#include "Engine.h"

bool Engine::generateMoves(const State &state, std::vector<Move> &moves) {
  moves.clear();
  if (state.boardState[state.dragSquare] != state.dragSquareValue ||
      state.dragSquareValue == empty) {
    return false;
  }
  Coordinate knightOffset[8] = {{-1, 2}, {1, 2},  {1, -2},  {-1, -2},
                                {2, 1},  {-2, 1}, {-2, -1}, {2, -1}};
  Coordinate slideOffset[8] = {{1, 0}, {-1, 0},  {0, -1}, {0, 1},
                               {1, 1}, {-1, -1}, {1, -1}, {-1, 1}};

  if (state.dragSquareValue == Wrook || state.dragSquareValue == Brook ||
      state.dragSquareValue == Wbishop || state.dragSquareValue == Bbishop ||
      state.dragSquareValue == Wqueen || state.dragSquareValue == Bqueen) {

    int start =
        (state.dragSquareValue == Wrook || state.dragSquareValue == Brook ||
         state.dragSquareValue == Wqueen || state.dragSquareValue == Bqueen)
            ? 0
            : 4;
    int width =
        (state.dragSquareValue == Wqueen || state.dragSquareValue == Bqueen)
            ? 8
            : 4;

    for (int i = start; i < start + width; i++) {
      Coordinate pieceLocation = {state.dragSquare / 8, state.dragSquare % 8};

      while (true) {
        // .i, .j refering to object ko variable, i refering to loop index
        pieceLocation.i += slideOffset[i].i;
        pieceLocation.j += slideOffset[i].j;
        if (!isValidPieceLocation(pieceLocation, state)) {
          break;
        }

        moves.push_back({true, state.dragSquare, pieceLocation});
        if (state.boardState[pieceLocation.i * 8 + pieceLocation.j] != empty) {
          break;
        }
      }
    }
    return true;

  } else if (state.dragSquareValue == Wpawn || state.dragSquareValue == Bpawn) {

    Coordinate pieceLocation = {state.dragSquare / 8, state.dragSquare % 8};

    Coordinate offset[2] = {{-1, 0}, {-2, 0}};
    Coordinate enPassantoffset[2] = {{-1, -1}, {-1, 1}};
    int initialRow;
    if (state.dragSquareValue == Wpawn) {
      offset[0] = {-1, 0};
      offset[1] = {-2, 0};
      enPassantoffset[0] = {-1, -1};
      enPassantoffset[1] = {-1, 1};
      initialRow = 6;
    } else {
      offset[0] = {1, 0};
      offset[1] = {2, 0};
      enPassantoffset[0] = {1, -1};
      enPassantoffset[1] = {1, 1};
      initialRow = 1;
    }

    pieceLocation.i += offset[0].i;
    pieceLocation.j += offset[0].j;
    if (isValidPieceLocation(pieceLocation, state) &&
        state.boardState[pieceLocation.i * 8 + pieceLocation.j] == empty) {
      moves.push_back({true, state.dragSquare, pieceLocation});

      // We check for 2 square move
      pieceLocation = {state.dragSquare / 8, state.dragSquare % 8};
      if (pieceLocation.i == initialRow) {
        pieceLocation.i += offset[1].i;
        pieceLocation.j += offset[1].j;
        if (isValidPieceLocation(pieceLocation, state) &&
            state.boardState[pieceLocation.i * 8 + pieceLocation.j] == empty) {
          moves.push_back({true, state.dragSquare, pieceLocation});
        }
      }
    }

    for (int i = 0; i < 2; i++) {
      Coordinate pieceLocation = {state.dragSquare / 8, state.dragSquare % 8};
      pieceLocation.i += enPassantoffset[i].i;
      pieceLocation.j += enPassantoffset[i].j;
      if (isValidPieceLocation(pieceLocation, state)) {
        moves.push_back({true, state.dragSquare, pieceLocation});
      }
    }
    return true;

  } else if (state.dragSquareValue == Wking || state.dragSquareValue == Bking) {
    Coordinate castleOffset[2] = {{0, 2}, {0, -2}};

    for (int i = 0; i < 8; i++) {
      Coordinate pieceLocation = {state.dragSquare / 8, state.dragSquare % 8};
      // .i, .j refering to object ko variable, i refering to loop index
      pieceLocation.i += slideOffset[i].i;
      pieceLocation.j += slideOffset[i].j;
      if (!isValidPieceLocation(pieceLocation, state)) {
        continue;
      }
      moves.push_back({true, state.dragSquare, pieceLocation});
    }

    // Checks for castle
    if ((state.dragSquareValue == Wking &&
         (state.castleAvailability[0] || state.castleAvailability[1])) ||
        (state.dragSquareValue == Bking &&
         (state.castleAvailability[2] || state.castleAvailability[3]))) {

      int start = state.dragSquareValue == Wking ? 0 : 2;
      for (int i = start; i < start + 2; i++) {
        if (!state.castleAvailability[i])
          continue;
        Coordinate pieceLocation = {state.dragSquare / 8, state.dragSquare % 8};
        bool castleable = true;

        int shiftTillFactorof8 = (i - start) ? -1 : 2;

        int j = state.dragSquare + castleOffset[i - start].j / 2;
        while ((j + shiftTillFactorof8) % 8 != 0) {
          if (state.boardState[j] != empty) {
            castleable = false;
            break;
          }
          j += castleOffset[i - start].j / 2;
        }
        pieceLocation.i += castleOffset[i - start].i;
        pieceLocation.j += castleOffset[i - start].j;

        if (isValidPieceLocation(pieceLocation, state) && castleable) {
          moves.push_back({true, state.dragSquare, pieceLocation});
        }
      }
    }

  } else if (state.dragSquareValue == Wknight ||
             state.dragSquareValue == Bknight) {
    for (int i = 0; i < 8; i++) {
      Coordinate pieceLocation = {state.dragSquare / 8, state.dragSquare % 8};
      // .i, .j refering to object ko variable, i refering to loop index
      pieceLocation.i += knightOffset[i].i;
      pieceLocation.j += knightOffset[i].j;
      if (!isValidPieceLocation(pieceLocation, state)) {
        continue;
      }
      moves.push_back({true, state.dragSquare, pieceLocation});
    }
    return true;
  }

  else {
    return false;
  }

  return true;
}

bool Engine::isValidPieceLocation(const Coordinate &location,
                                  const State &state) {

  /*
   * Location is the Coordiante of the ending place.
   * The value dragSquare and dragSquareValue are the boardState location(piece
   * location) currently selected piece value
   * Next we need some other game values
   */
  if (location.i >= 0 && location.j >= 0 && location.i <= 7 &&
      location.j <= 7) {
    int index = location.i * 8 + location.j;
    pieceIndex indexPiece = state.boardState[index];

    if (indexPiece == empty) {
      if (state.dragSquareValue == Wpawn || state.dragSquareValue == Bpawn) {

        if (state.enPassantAvailable) {
          if (state.enPassant == index) {
            return true;
          }
        }

        // If it is a pawn making a diagonal move on an empty square
        if ((state.dragSquare - index) % 8 != 0) {
          return false;
        }
      }
      return true;
    }

    if (state.isWhiteTurn) {
      if (indexPiece >= Bking) {
        return true;
      } else
        return false;

    } else {
      // If black's turn
      if (indexPiece < Bking) {
        return true;
      } else
        return false;
    }

    return true;
  } else
    return false;
}
