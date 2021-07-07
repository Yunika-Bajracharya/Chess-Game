#pragma once

#include <string.h>
enum Color { White, Black };
class Player {
public:
  Player(){};
  Player(const char *, Color);
  ~Player(){};

private:
  char name[20];
  Color color;
};
