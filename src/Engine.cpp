#include "Engine.h"

bool Engine::setupFEN(State &state, const char *FENstring) {
  int i = 0; // Loopinng through string
  int j = 0; // Looping through boardState

  while (FENstring[i] != ' ') {

    if (FENstring[i] == '/') {
      i++;
      continue;
    }
    if (FENstring[i] > ASCII_OFFSET + 0 && FENstring[i] < ASCII_OFFSET + 9) {
      j += FENstring[i] - ASCII_OFFSET;
      i++;
      continue;
    }
    switch (FENstring[i]) {
    case 'K':
      state.boardState[j] = Wking;
      break;
    case 'k':
      state.boardState[j] = Bking;
      break;
    case 'Q':
      state.boardState[j] = Wqueen;
      break;
    case 'q':
      state.boardState[j] = Bqueen;
      break;
    case 'N':
      state.boardState[j] = Wknight;
      break;
    case 'n':
      state.boardState[j] = Bknight;
      break;
    case 'B':
      state.boardState[j] = Wbishop;
      break;
    case 'b':
      state.boardState[j] = Bbishop;
      break;
    case 'R':
      state.boardState[j] = Wrook;
      break;
    case 'r':
      state.boardState[j] = Brook;
      break;
    case 'P':
      state.boardState[j] = Wpawn;
      break;
    case 'p':
      state.boardState[j] = Bpawn;
      break;
    default:
      break;
    }

    i++;
    j++;
  }
  i++;
  state.isWhiteTurn = (FENstring[i] == 'w') ? true : false;
  i += 2;

  for (int i = 0; i < 4; i++) {
    state.castleAvailability[i] = false;
  }
  while (FENstring[i] != ' ') {
    if (FENstring[i] == '-') {
      i++;
      break;
    }
    switch (FENstring[i]) {
    case 'K':
      state.castleAvailability[0] = true;
      break;
    case 'Q':
      state.castleAvailability[1] = true;
      break;
    case 'q':
      state.castleAvailability[3] = true;
      break;
    case 'k':
      state.castleAvailability[2] = true;
    }
    i++;
  }
  i++;

  if (FENstring[i] == '-') {
    state.enPassantAvailable = false;
    state.enPassant = 0;
    i++;
  } else {
    int right = FENstring[i] - 97;
    int down = 8 - (FENstring[i + 1] - ASCII_OFFSET);
    state.enPassantAvailable = true;
    state.enPassant = right + 8 * down;
    i += 2;
  }
  i++;

  // handle the half move clock and full move number.
  return true;
}

void Engine::handlePieceSelection(Coordinate &location, State &state,
                                  std::vector<Move> &moves) {

  std::cout << (check(state) ? "Check" : "No Check") << std::endl;

  int index = location.i * 8 + location.j;
  pieceIndex indexPiece = state.boardState[index];
  if (state.isWhiteTurn) {
    if (indexPiece > empty && indexPiece <= Wpawn) {
      // Drag square for the highlight
      state.dragSquare = index;
      state.dragSquareValue = indexPiece;
      Engine::generateLegalMoves(state, moves);

    } else
      state.dragSquareValue = empty;
  } else {
    if (indexPiece >= Bking) {
      // Drag square for the highlight
      state.dragSquare = index;
      state.dragSquareValue = indexPiece;
      Engine::generateLegalMoves(state, moves);
    } else
      state.dragSquareValue = empty;
  }
}

void Engine::handlePiecePlacement(Coordinate &location, State &state,
                                  std::vector<Move> &moves, Move &lastMove) {
  int index = location.i * 8 + location.j;
  pieceIndex temp =
      state.boardState[state.dragSquare]; // Mouse button down bha ko square

  // Check if it is a valid move
  bool valid = false;
  for (Move move : moves) {
    if (move.end == index) {
      valid = true;
    }
  }
  if (!valid) {
    state.dragSquareValue = empty;
    moves.clear();
    return;
  }

  // Previous place is empty now
  state.boardState[state.dragSquare] = empty;

  // We store some info about the last move
  lastMove.made = true;
  lastMove.start = state.dragSquare;
  lastMove.end = index;

  // If either the start or end of last move touches rook, we disable castling

  // Play sound

  // Make enPassant work
  if (temp == Wpawn || temp == Bpawn) {
    int offset = (Wpawn == temp) ? 16 : -16;
    if (state.enPassantAvailable && (index) == state.enPassant) {
      int killLocation = index + offset / 2;
      state.boardState[killLocation] = empty;
      state.enPassantAvailable = false;
    } else {
      state.enPassantAvailable = false;
    }
    if ((state.dragSquare - (index)) == offset) {
      state.enPassantAvailable = true;
      state.enPassant = state.dragSquare - offset / 2;
      std::cout << "En passant Available at " << state.enPassant << std::endl;
    }
  } else {
    state.enPassantAvailable = false;
  }

  // If it was a castle
  if (temp == Wking || temp == Bking) {
    // We have a castle move if
    if ((lastMove.end - lastMove.start) == 2) {
      // We move the rook too
      if (state.boardState[lastMove.end + 1] == Wrook ||
          state.boardState[lastMove.end + 1] == Brook) {
        // Last check to see if there is a rook there

        state.boardState[lastMove.end + 1] = empty;
        state.boardState[lastMove.end - 1] = (temp == Wking) ? Wrook : Brook;
      }
    }
    if ((lastMove.end - lastMove.start) == -2) {
      // We move the rook too
      //
      if (state.boardState[lastMove.end - 2] == Wrook ||
          state.boardState[lastMove.end - 2] == Brook) {
        // Last check to see if there is a rook there

        state.boardState[lastMove.end - 2] = empty;
        state.boardState[lastMove.end + 1] = (temp == Wking) ? Wrook : Brook;
      }
    }
    if (temp == Wking) {
      state.castleAvailability[0] = false;
      state.castleAvailability[1] = false;
    }
    if (temp == Bking) {
      state.castleAvailability[2] = false;
      state.castleAvailability[3] = false;
    }
  }

  // it a pawn reaches promotion square
  if (temp == Wpawn || temp == Bpawn) {
    if (location.i == 0 || location.i == 7) {
      temp = (temp == Wpawn) ? Wqueen : Bqueen;
    }
  }

  // We change the location to new
  std::cout << "Pos: (" << location.i << ", " << location.j << ")" << std::endl;
  state.dragSquare = index; // Now we change the drag square value
  state.boardState[state.dragSquare] = temp; // Put the piece is that square
  state.dragSquareValue = empty;             // Make the start square empty

  state.isWhiteTurn = !state.isWhiteTurn;
}

bool Engine::generateLegalMoves(const State &state, std::vector<Move> &moves) {
  std::vector<Move> newMoves;
  generatePseudoLegalMoves(state, newMoves);

  for (Move move : newMoves) {
    Move lastMove = {true, 0, 0};
    State newState = state;
    Coordinate location = {move.end / 8, move.end % 8};

    Engine::handlePiecePlacement(location, newState, newMoves, lastMove);
    // We check if our king is still in danger, if no bad move.
    if (!check(newState)) {
      moves.push_back(move);
    }
  }
  return true;
}

bool Engine::generatePseudoLegalMoves(const State &state,
                                      std::vector<Move> &moves) {
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

bool Engine::check(const State &state) {
  pieceIndex startPiece = state.isWhiteTurn ? Wking : Bking;
  pieceIndex endPiece = state.isWhiteTurn ? Wpawn : Bpawn;

  // Find the square of the king
  pieceIndex searchPiece = state.isWhiteTurn ? Bking : Wking;
  int kingIndex = -1;
  for (int i = 0; i < 64; i++) {
    if (state.boardState[i] == searchPiece) {
      kingIndex = i;
      break;
    }
  }
  if (kingIndex == -1) {
    return false;
  }

  for (int i = 0; i < 64; i++) {
    // For each of the opponent's piece
    if (state.boardState[i] >= startPiece && state.boardState[i] <= endPiece) {
      // We make a new state with tthat piece and generate moves
      // If any of those moves attacks the king we retrun true

      State newState = state;
      newState.dragSquare = i;
      newState.dragSquareValue = newState.boardState[i];

      // newState.isWhiteTurn = !newState.isWhiteTurn;
      std::vector<Move> moves;

      Engine::generatePseudoLegalMoves(newState, moves);

      for (Move move : moves) {
        if (move.end == kingIndex) {
          return true;
        }
      }
    }
  }

  return false;
}
