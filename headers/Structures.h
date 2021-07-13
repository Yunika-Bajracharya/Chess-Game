#pragma once

struct Coordinate {
  int i, j;
};

enum pieceIndex {
  empty = 0,
  Wking = 1,
  Wqueen,
  Wbishop,
  Wknight,
  Wrook,
  Wpawn,
  Bking,
  Bqueen,
  Bbishop,
  Bknight,
  Brook,
  Bpawn,
};

struct Move {
  bool made;
  int start;
  int end;

  Move() {}
  Move(bool m, int s, int e) : made(m), start(s), end(e) {}
  Move(bool m, int s, Coordinate e) : made(m), start(s) { end = e.i * 8 + e.j; }
};

struct State {
  pieceIndex boardState[64];  // The current board state
  bool isWhiteTurn;           // True if it is white's turn
  bool castleAvailability[4]; // KQkq from fen string, true if castling is
                              // available
  bool enPassantAvailable;    // True is enPassant is available
  int enPassant;              // The square in which enPassant is available

  int dragSquare;             // The index of the currently lifted piece
  pieceIndex dragSquareValue; // The value of that piece
};
