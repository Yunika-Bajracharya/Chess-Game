#include "Grid.h"

Grid::Grid() {
  pieceTexture = TextureManager::loadTexture("assets/pieces.png");
  for (int i = 0; i < 64; i++) {
    boardState[i] = 0;
  }
  boardState[1] = Wking;
  boardState[2] = Bqueen;
}

Grid::~Grid() {}

void Grid::render() {
  SDL_Rect tempDest;
  SDL_Rect piecesSrcRect;

  tempDest.h = tempDest.w = BLOCK_WIDTH;
  tempDest.x = WINDOW_WIDTH / 2 - BLOCK_WIDTH * 4;
  tempDest.y = WINDOW_HEIGHT / 2 - BLOCK_WIDTH * 4;

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
        int x = (boardState[i * 8 + j] - 1) % 2;
        int y = (boardState[i * 8 + j] - 1) / 6;

        piecesSrcRect.x = x * 200;
        piecesSrcRect.y = y * 200;
        TextureManager::Draw(pieceTexture, piecesSrcRect, tempDest);
      }

      tempDest.x += BLOCK_WIDTH;
    }
    tempDest.x -= BLOCK_WIDTH * 8;
    tempDest.y += BLOCK_WIDTH;
  }
}
