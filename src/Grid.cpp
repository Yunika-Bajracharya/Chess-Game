#include "Grid.h"
#include <vector>

Grid::Grid() {
  pieceTexture = TextureManager::loadTexture("assets/pieces.png");
  for (int i = 0; i < 64; i++) {
    state.boardState[i] = empty;
  }
  gridStartX = WINDOW_WIDTH / 2 - BLOCK_WIDTH * 4;
  gridStartY = WINDOW_HEIGHT / 2 - BLOCK_WIDTH * 4;
  state.dragSquare = 0;
  state.dragSquareValue = empty;
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

void Grid::handleMouseButtonDown(SDL_Event &event) {

  int x = event.button.x - gridStartX;
  int y = event.button.y - gridStartY;

  if (x > 0 && y > 0 && x < 8 * BLOCK_WIDTH && y < 8 * BLOCK_WIDTH) {
    int i = y / BLOCK_WIDTH;
    int j = x / BLOCK_WIDTH;
    std::cout << "Pos: (" << i << ", " << j << ")" << std::endl;

    // We check if the piece is of the correct color
    int index = i * 8 + j;
    pieceIndex indexPiece = state.boardState[index];
    if (state.isWhiteTurn) {
      if (indexPiece > empty && indexPiece <= Wpawn) {
        // Drag square for the highlight
        state.dragSquare = index;
        state.dragSquareValue = indexPiece;
        Engine::generateMoves(state, moves);

      } else
        state.dragSquareValue = empty;
    } else {
      if (indexPiece >= Bking) {
        // Drag square for the highlight
        state.dragSquare = index;
        state.dragSquareValue = indexPiece;
        Engine::generateMoves(state, moves);
      } else
        state.dragSquareValue = empty;
    }
  } else {
    return;
  }
}

void Grid::handleMouseButtonUp(SDL_Event &event) {
  int x = event.button.x - gridStartX;
  int y = event.button.y - gridStartY;

  if (state.dragSquareValue == empty) {
    return;
  }

  if (x > 0 && y > 0 && x < 8 * BLOCK_WIDTH && y < 8 * BLOCK_WIDTH) {
    int i = y / BLOCK_WIDTH;
    int j = x / BLOCK_WIDTH;
    pieceIndex temp =
        state.boardState[state.dragSquare]; // Mouse button down bha ko square

    // Check if it is a valid move
    bool valid = false;
    for (Move move : moves) {
      if (move.end == i * 8 + j) {
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
    lastMove.end = i * 8 + j;

    // If either the start or end of last move touches rook, we disable castling

    // Play sound
    Mix_PlayChannel(-1, moveSound, 0);

    // Make enPassant work
    if (temp == Wpawn || temp == Bpawn) {
      int offset = (Wpawn == temp) ? 16 : -16;
      if (state.enPassantAvailable && (i * 8 + j) == state.enPassant) {
        int killLocation = i * 8 + j + offset / 2;
        state.boardState[killLocation] = empty;
        state.enPassantAvailable = false;
      } else {
        state.enPassantAvailable = false;
      }
      if ((state.dragSquare - (i * 8 + j)) == offset) {
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
      if (i == 0 || i == 7) {
        temp = (temp == Wpawn) ? Wqueen : Bqueen;
      }
    }

    // We change the location to new
    std::cout << "Pos: (" << i << ", " << j << ")" << std::endl;
    state.dragSquare = i * 8 + j; // Now we change the drag square value
    state.boardState[state.dragSquare] = temp; // Put the piece is that square
    state.dragSquareValue = empty;             // Make the start square empty

    state.isWhiteTurn = !state.isWhiteTurn;

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

      if (state.boardState[i * 8 + j] != 0) {
        // Draws the piece
        int x = (state.boardState[i * 8 + j] - 1) % 6;
        int y = (state.boardState[i * 8 + j] - 1) / 6;

        piecesSrcRect.x = x * 200;
        piecesSrcRect.y = y * 200;

        if (i * 8 + j == state.dragSquare &&
            state.boardState[state.dragSquare] == state.dragSquareValue) {
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
          if (a.start == state.dragSquare && a.end == i * 8 + j) {
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

  if (state.boardState[state.dragSquare] == state.dragSquareValue) {
    SDL_Rect rect;
    int x = (state.boardState[state.dragSquare] - 1) % 6;
    int y = (state.boardState[state.dragSquare] - 1) / 6;

    piecesSrcRect.x = x * 200;
    piecesSrcRect.y = y * 200;

    rect.w = rect.h = tempDest.h;
    SDL_GetMouseState(&rect.x, &rect.y);
    rect.x -= rect.w / 2;
    rect.y -= rect.h / 2;
    TextureManager::Draw(pieceTexture, piecesSrcRect, rect);
  }
}

/*
void Grid::filterLegalMoves() {
  // TOO later
  std::vector<pieceIndex> pieces;
  for (int i = 0; i < 64; i++) {
  }

  for (Move move : moves) {
  }
}
*/
