#include "Grid.h"

Grid::Grid() {}

Grid::~Grid() {}

void Grid::render() {
  SDL_Rect temp;
  temp.h = temp.w = BLOCK_WIDTH;
  temp.x = WINDOW_WIDTH / 2 - BLOCK_WIDTH * 4;
  temp.y = WINDOW_HEIGHT / 2 - BLOCK_WIDTH * 4;
  std::cout << "X: " << temp.x << ", "
            << "Y: " << temp.y << std::endl;

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if ((i + j) % 2 == 0) {
        SDL_SetRenderDrawColor(Game::renderer, 238, 238, 210, 255);

      } else {
        SDL_SetRenderDrawColor(Game::renderer, 118, 150, 86, 255);
      }
      SDL_RenderFillRect(Game::renderer, &temp);

      temp.x += BLOCK_WIDTH;
    }
    temp.x -= BLOCK_WIDTH * 8;
    temp.y += BLOCK_WIDTH;
  }
}
