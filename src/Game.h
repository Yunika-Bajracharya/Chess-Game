#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>

#include "Grid.h"

#define WINDOW_HEIGHT 640
#define WINDOW_WIDTH 800

class Game {

public:
  Game();
  ~Game();

  void init(const char *title, int xpos, int ypos, int width, int height,
            bool fullscreen);

  void handleEvents();
  void update();
  void render();
  void clean();

  bool running();

  static SDL_Renderer *renderer;

private:
  bool isRunning;
  SDL_Window *window;

  int count = 0;
};
