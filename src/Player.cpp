#include "../headers/Player.h"

Player::Player(const char *name, Color color) {
  strcpy(this->name, name);
  this->color = color;
}
