#include "Grid.h"

Grid::Grid() {
  pieceTexture = TextureManager::loadTexture("assets/pieces.png");
  for (int i = 0; i < 64; i++) {
    boardState[i] = empty;
  }
  gridStartX = WINDOW_WIDTH / 2 - BLOCK_WIDTH * 4;
  gridStartY = WINDOW_HEIGHT / 2 - BLOCK_WIDTH * 4;
  dragSquare = 0;
  dragSquareValue = empty;
  lastMove = {false, 0, 0};

  // Sound Stuff
  moveSound = Mix_LoadWAV("assets/move.wav");
  if (moveSound == NULL) {
    printf("Failed to load medium sound effect! SDL_mixer Error: %s\n",
           Mix_GetError());
  }

  setupFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  players[0] = Player("Suban", White);
  players[1] = Player("Orabin", Black);
}

Grid::~Grid() {
  SDL_DestroyTexture(pieceTexture);
  Mix_FreeChunk(moveSound);
}

bool Grid::setupFEN(const char *FENstring) {
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
      boardState[j] = Wking;
      break;
    case 'k':
      boardState[j] = Bking;
      break;
    case 'Q':
      boardState[j] = Wqueen;
      break;
    case 'q':
      boardState[j] = Bqueen;
      break;
    case 'N':
      boardState[j] = Wknight;
      break;
    case 'n':
      boardState[j] = Bknight;
      break;
    case 'B':
      boardState[j] = Wbishop;
      break;
    case 'b':
      boardState[j] = Bbishop;
      break;
    case 'R':
      boardState[j] = Wrook;
      break;
    case 'r':
      boardState[j] = Brook;
      break;
    case 'P':
      boardState[j] = Wpawn;
      break;
    case 'p':
      boardState[j] = Bpawn;
      break;
    default:
      break;
    }

    i++;
    j++;
  }
  i++;
  whiteTurn = (FENstring[i] == 'w') ? true : false;
  i += 2;

  for (int i = 0; i < 4; i++) {
    castleAvailability[i] = false;
  }
  while (FENstring[i] != ' ') {
    if (FENstring[i] == '-') {
      i++;
      break;
    }
    switch (FENstring[i]) {
    case 'K':
      castleAvailability[0] = true;
      break;
    case 'Q':
      castleAvailability[1] = true;
      break;
    case 'q':
      castleAvailability[3] = true;
      break;
    case 'k':
      castleAvailability[2] = true;
    }
    i++;
  }
  i++;

  if (FENstring[i] == '-') {
    enPassantAvailable = false;
    enPassant = 0;
    i++;
  } else {
    int right = FENstring[i] - 97;
    int down = 8 - (FENstring[i + 1] - ASCII_OFFSET);
    enPassantAvailable = true;
    enPassant = right + 8 * down;
    i += 2;
  }
  i++;

  // handle the half move clock and full move number.
  return true;
}

void Grid::handleMouseButtonDown(SDL_Event &event) {

  int x = event.button.x - gridStartX;
  int y = event.button.y - gridStartY;

  if (x > 0 && y > 0 && x < 8 * BLOCK_WIDTH && y < 8 * BLOCK_WIDTH) {
    int i = y / BLOCK_WIDTH;
    int j = x / BLOCK_WIDTH;
    std::cout << "Pos: (" << i << ", " << j << ")" << std::endl;

    // We check if the piece is of the correct color
    int index = i * 8 + j;
    pieceIndex indexPiece = boardState[index];
    if (whiteTurn) {
      if (indexPiece > empty && indexPiece <= Wpawn) {
        // Drag square for the highlight
        dragSquare = index;
        dragSquareValue = indexPiece;
        generateMoves();
      } else
        dragSquareValue = empty;
    } else {
      if (indexPiece >= Bking) {
        // Drag square for the highlight
        dragSquare = index;
        dragSquareValue = indexPiece;
        generateMoves();
      } else
        dragSquareValue = empty;
    }
  } else {
    return;
  }
}

void Grid::handleMouseButtonUp(SDL_Event &event) {
  int x = event.button.x - gridStartX;
  int y = event.button.y - gridStartY;

  if (dragSquareValue == empty) {
    return;
  }

  if (x > 0 && y > 0 && x < 8 * BLOCK_WIDTH && y < 8 * BLOCK_WIDTH) {
    int i = y / BLOCK_WIDTH;
    int j = x / BLOCK_WIDTH;
    pieceIndex temp = boardState[dragSquare]; // Mouse button down bha ko square

    // Check if it is a valid move
    bool valid = false;
    for (Move move : moves) {
      if (move.end == i * 8 + j) {
        valid = true;
      }
    }
    if (!valid) {
      dragSquareValue = empty;
      moves.clear();
      return;
    }

    // Previous place is empty now
    boardState[dragSquare] = empty;

    // We store some info about the last move
    lastMove.made = true;
    lastMove.start = dragSquare;
    lastMove.end = i * 8 + j;

    // If either the start or end of last move touches rook, we disable castling

    // Play sound
    Mix_PlayChannel(-1, moveSound, 0);

    // Make enPassant work
    if (temp == Wpawn || temp == Bpawn) {
      int offset = (Wpawn == temp) ? 16 : -16;
      if (enPassantAvailable && (i * 8 + j) == enPassant) {
        int killLocation = i * 8 + j + offset / 2;
        boardState[killLocation] = empty;
        enPassantAvailable = false;
      } else {
        enPassantAvailable = false;
      }
      if ((dragSquare - (i * 8 + j)) == offset) {
        enPassantAvailable = true;
        enPassant = dragSquare - offset / 2;
        std::cout << "En passant Available at " << enPassant << std::endl;
      }
    } else {
      enPassantAvailable = false;
    }

    // If it was a castle
    if (temp == Wking || temp == Bking) {
      // We have a castle move if
      if ((lastMove.end - lastMove.start) == 2) {
        // We move the rook too
        if (boardState[lastMove.end + 1] == Wrook ||
            boardState[lastMove.end + 1] == Brook) {
          // Last check to see if there is a rook there

          boardState[lastMove.end + 1] = empty;
          boardState[lastMove.end - 1] = (temp == Wking) ? Wrook : Brook;
        }
      }
      if ((lastMove.end - lastMove.start) == -2) {
        // We move the rook too
        //
        if (boardState[lastMove.end - 2] == Wrook ||
            boardState[lastMove.end - 2] == Brook) {
          // Last check to see if there is a rook there

          boardState[lastMove.end - 2] = empty;
          boardState[lastMove.end + 1] = (temp == Wking) ? Wrook : Brook;
        }
      }
      if (temp == Wking) {
        castleAvailability[0] = false;
        castleAvailability[1] = false;
      }
      if (temp == Bking) {
        castleAvailability[2] = false;
        castleAvailability[3] = false;
      }
    }

    // it a pawn reaches promotion square
    if (temp == Wpawn || temp == Bpawn) {
      if (i == 0 || i == 7) {
        temp = (temp == Wpawn) ? Wqueen : Bqueen;
      }
    }

    // We change the location to new
    std::cout << "Pos: (" << i << ", " << j << ")" << std::endl;
    dragSquare = i * 8 + j;        // Now we change the drag square value
    boardState[dragSquare] = temp; // Put the piece is that square
    dragSquareValue = empty;       // Make the start square empty

    whiteTurn = !whiteTurn;

  } else {
    return;
  }
}

void Grid::render() {
  SDL_Rect tempDest;
  SDL_Rect piecesSrcRect;

  tempDest.h = tempDest.w = BLOCK_WIDTH;
  tempDest.x = gridStartX;
  tempDest.y = gridStartY;

  piecesSrcRect.w = piecesSrcRect.h = 200;

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {

      if ((i + j) % 2 == 0) {
        SDL_SetRenderDrawColor(Game::renderer, 238, 238, 210, 255);
      } else {
        SDL_SetRenderDrawColor(Game::renderer, 118, 150, 86, 255);
      }
      SDL_RenderFillRect(Game::renderer, &tempDest);
      if (lastMove.made) {
        if (lastMove.end == i * 8 + j || lastMove.start == i * 8 + j) {
          SDL_SetRenderDrawColor(Game::renderer, 255, 255, 0, 200);
          SDL_RenderFillRect(Game::renderer, &tempDest);
        }
      }

      if (boardState[i * 8 + j] != 0) {
        // Draws the piece
        int x = (boardState[i * 8 + j] - 1) % 6;
        int y = (boardState[i * 8 + j] - 1) / 6;

        piecesSrcRect.x = x * 200;
        piecesSrcRect.y = y * 200;

        if (i * 8 + j == dragSquare &&
            boardState[dragSquare] == dragSquareValue) {
          SDL_SetRenderDrawColor(Game::renderer, 255, 255, 0, 120);
          SDL_RenderFillRect(Game::renderer, &tempDest);
        } else {
          TextureManager::Draw(pieceTexture, piecesSrcRect, tempDest);
        }
      }

      // Rendering Valid moves
      // TODO, put this in its own loopty loop maybe ?
      if (moves.size() != 0) {
        for (Move a : moves) {
          if (a.start == dragSquare && a.end == i * 8 + j) {
            SDL_SetRenderDrawColor(Game::renderer, 255, 0, 0, 150);
            SDL_RenderFillRect(Game::renderer, &tempDest);
          }
        }
      }
      tempDest.x += BLOCK_WIDTH;
    }
    tempDest.x -= BLOCK_WIDTH * 8;
    tempDest.y += BLOCK_WIDTH;
  }
  // Rendering the floating piece

  if (boardState[dragSquare] == dragSquareValue) {
    SDL_Rect rect;
    int x = (boardState[dragSquare] - 1) % 6;
    int y = (boardState[dragSquare] - 1) / 6;

    piecesSrcRect.x = x * 200;
    piecesSrcRect.y = y * 200;

    rect.w = rect.h = tempDest.h;
    SDL_GetMouseState(&rect.x, &rect.y);
    rect.x -= rect.w / 2;
    rect.y -= rect.h / 2;
    TextureManager::Draw(pieceTexture, piecesSrcRect, rect);
  }
}

bool Grid::generateMoves() {
  moves.clear();
  if (boardState[dragSquare] != dragSquareValue || dragSquareValue == empty) {
    return false;
  }
  Coordinate knightOffset[8] = {{-1, 2}, {1, 2},  {1, -2},  {-1, -2},
                                {2, 1},  {-2, 1}, {-2, -1}, {2, -1}};
  Coordinate slideOffset[8] = {{1, 0}, {-1, 0},  {0, -1}, {0, 1},
                               {1, 1}, {-1, -1}, {1, -1}, {-1, 1}};

  if (dragSquareValue == Wrook || dragSquareValue == Brook ||
      dragSquareValue == Wbishop || dragSquareValue == Bbishop ||
      dragSquareValue == Wqueen || dragSquareValue == Bqueen) {

    int start = (dragSquareValue == Wrook || dragSquareValue == Brook ||
                 dragSquareValue == Wqueen || dragSquareValue == Bqueen)
                    ? 0
                    : 4;
    int width =
        (dragSquareValue == Wqueen || dragSquareValue == Bqueen) ? 8 : 4;

    for (int i = start; i < start + width; i++) {
      Coordinate pieceLocation = {dragSquare / 8, dragSquare % 8};

      while (true) {
        // .i, .j refering to object ko variable, i refering to loop index
        pieceLocation.i += slideOffset[i].i;
        pieceLocation.j += slideOffset[i].j;
        if (!isValidPieceLocation(pieceLocation)) {
          break;
        }

        moves.push_back(
            {true, dragSquare, pieceLocation.i * 8 + pieceLocation.j});
        if (boardState[pieceLocation.i * 8 + pieceLocation.j] != empty) {
          break;
        }
      }
    }
    return true;

  } else if (dragSquareValue == Wpawn || dragSquareValue == Bpawn) {

    Coordinate pieceLocation = {dragSquare / 8, dragSquare % 8};

    Coordinate offset[2] = {{-1, 0}, {-2, 0}};
    Coordinate enPassantoffset[2] = {{-1, -1}, {-1, 1}};
    int initialRow;
    if (dragSquareValue == Wpawn) {
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
    if (isValidPieceLocation(pieceLocation) &&
        boardState[pieceLocation.i * 8 + pieceLocation.j] == empty) {
      moves.push_back(
          {true, dragSquare, pieceLocation.i * 8 + pieceLocation.j});

      // We check for 2 square move
      pieceLocation = {dragSquare / 8, dragSquare % 8};
      if (pieceLocation.i == initialRow) {
        pieceLocation.i += offset[1].i;
        pieceLocation.j += offset[1].j;
        if (isValidPieceLocation(pieceLocation) &&
            boardState[pieceLocation.i * 8 + pieceLocation.j] == empty) {
          moves.push_back(
              {true, dragSquare, pieceLocation.i * 8 + pieceLocation.j});
        }
      }
    }

    for (int i = 0; i < 2; i++) {
      Coordinate pieceLocation = {dragSquare / 8, dragSquare % 8};
      pieceLocation.i += enPassantoffset[i].i;
      pieceLocation.j += enPassantoffset[i].j;
      if (isValidPieceLocation(pieceLocation)) {
        moves.push_back(
            {true, dragSquare, pieceLocation.i * 8 + pieceLocation.j});
      }
    }
    return true;

  } else if (dragSquareValue == Wking || dragSquareValue == Bking) {
    Coordinate castleOffset[2] = {{0, 2}, {0, -2}};

    for (int i = 0; i < 8; i++) {
      Coordinate pieceLocation = {dragSquare / 8, dragSquare % 8};
      // .i, .j refering to object ko variable, i refering to loop index
      pieceLocation.i += slideOffset[i].i;
      pieceLocation.j += slideOffset[i].j;
      if (!isValidPieceLocation(pieceLocation)) {
        continue;
      }
      moves.push_back(
          {true, dragSquare, pieceLocation.i * 8 + pieceLocation.j});
    }

    // Checks for castle
    if ((dragSquareValue == Wking &&
         (castleAvailability[0] || castleAvailability[1])) ||
        (dragSquareValue == Bking &&
         (castleAvailability[2] || castleAvailability[3]))) {

      int start = dragSquareValue == Wking ? 0 : 2;
      for (int i = start; i < start + 2; i++) {
        if (!castleAvailability[i])
          continue;
        Coordinate pieceLocation = {dragSquare / 8, dragSquare % 8};
        bool castleable = true;

        int shiftTillFactorof8 = (i - start) ? -1 : 2;

        int j = dragSquare + castleOffset[i - start].j / 2;
        while ((j + shiftTillFactorof8) % 8 != 0) {
          if (boardState[j] != empty) {
            castleable = false;
            break;
          }
          j += castleOffset[i - start].j / 2;
        }
        pieceLocation.i += castleOffset[i - start].i;
        pieceLocation.j += castleOffset[i - start].j;

        if (isValidPieceLocation(pieceLocation) && castleable) {
          moves.push_back(
              {true, dragSquare, pieceLocation.i * 8 + pieceLocation.j});
        }
      }
    }

  } else if (dragSquareValue == Wknight || dragSquareValue == Bknight) {
    for (int i = 0; i < 8; i++) {
      Coordinate pieceLocation = {dragSquare / 8, dragSquare % 8};
      // .i, .j refering to object ko variable, i refering to loop index
      pieceLocation.i += knightOffset[i].i;
      pieceLocation.j += knightOffset[i].j;
      if (!isValidPieceLocation(pieceLocation)) {
        continue;
      }
      moves.push_back(
          {true, dragSquare, pieceLocation.i * 8 + pieceLocation.j});
    }
    return true;
  }

  else {
    return false;
  }

  return true;
}

bool Grid::isValidPieceLocation(const Coordinate &location) {
  if (location.i >= 0 && location.j >= 0 && location.i <= 7 &&
      location.j <= 7) {
    int index = location.i * 8 + location.j;
    pieceIndex indexPiece = boardState[index];

    if (indexPiece == empty) {
      if (dragSquareValue == Wpawn || dragSquareValue == Bpawn) {

        if (enPassantAvailable) {
          if (enPassant == index) {
            return true;
          }
        }

        // If it is a pawn making a diagonal move on an empty square
        if ((dragSquare - index) % 8 != 0) {
          return false;
        }
      }
      return true;
    }

    if (whiteTurn) {
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
