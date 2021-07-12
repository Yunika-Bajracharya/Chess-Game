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

  Engine::setupFEN(state,
                   "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

  players[0] = Player("Suban", White);
  players[1] = Player("Orabin", Black);
}

Grid::~Grid() {
  SDL_DestroyTexture(pieceTexture);
  Mix_FreeChunk(moveSound);
}

void Grid::handleMouseButtonDown(SDL_Event &event) {

  int x = event.button.x - gridStartX;
  int y = event.button.y - gridStartY;

  if (x > 0 && y > 0 && x < 8 * BLOCK_WIDTH && y < 8 * BLOCK_WIDTH) {
    int i = y / BLOCK_WIDTH;
    int j = x / BLOCK_WIDTH;
    std::cout << "Pos: (" << i << ", " << j << ")" << std::endl;
    Coordinate location = {i, j};
    Engine::handlePieceSelection(location, state, moves);
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
    Coordinate location = {i, j};

    Engine::handlePiecePlacement(location, state, moves, lastMove);
    Mix_PlayChannel(-1, moveSound, 0);

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
