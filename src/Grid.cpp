#include "Grid.h"

Grid::Grid() {
  pieceTexture = TextureManager::loadTexture("assets/pieces.png");
  for (int i = 0; i < 64; i++) {
    boardState[i] = 0;
  }
  gridStartX = WINDOW_WIDTH / 2 - BLOCK_WIDTH * 4;
  gridStartY = WINDOW_HEIGHT / 2 - BLOCK_WIDTH * 4;
  dragSquare = 0;
  dragSquareValue = 0;

  setupFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

Grid::~Grid() {}

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
    dragSquare = i * 8 + j;
    dragSquareValue = boardState[dragSquare];
  } else {
    return;
  }
}

void Grid::handleMouseButtonUp(SDL_Event &event) {
  int x = event.button.x - gridStartX;
  int y = event.button.y - gridStartY;

  if (x > 0 && y > 0 && x < 8 * BLOCK_WIDTH && y < 8 * BLOCK_WIDTH) {
    int i = y / BLOCK_WIDTH;
    int j = x / BLOCK_WIDTH;
    int temp = boardState[dragSquare]; // Mouse button down bha ko square
    boardState[dragSquare] = 0;

    std::cout << "Pos: (" << i << ", " << j << ")" << std::endl;
    dragSquare = i * 8 + j;
    boardState[dragSquare] = temp;
    dragSquareValue = 0;
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

      if (boardState[i * 8 + j] != 0) {
        // Draws the piece
        int x = (boardState[i * 8 + j] - 1) % 6;
        int y = (boardState[i * 8 + j] - 1) / 6;

        piecesSrcRect.x = x * 200;
        piecesSrcRect.y = y * 200;

        if (i * 8 + j == dragSquare &&
            boardState[dragSquare] == dragSquareValue) {

        } else {
          TextureManager::Draw(pieceTexture, piecesSrcRect, tempDest);
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
