#pragma once
#include "Defines.h"

enum Piece : BYTE {
	Pawn,
	Bishop,
	Knight,
	Rook,
	Queen,
	King
};

enum Color : BYTE {
	White,
	Black
};

enum CRightsFlags : BYTE {
	WhiteKing = 1,
	WhiteQueen = 2,
	BlackKing = 4,
	BlackQueen = 8
};
