#pragma once
#include "Defines.h"

enum Piece : BYTE {
	Pawn,
	Knight,
	Bishop,
	Rook,
	Queen,
	King
};

enum Color : BYTE {
	White,
	Black
};

enum CRightsFlags : BYTE {
	WhiteKing = 0b1,
	WhiteQueen = 0b10,
	BlackKing = 0b100,
	BlackQueen = 0b1000
};


enum EnPassant {  
	// Careful when packing EnPassant, it must have at least 4 bits to represent the 'None' state.
	AFile = 0,
	BFile = 1,
	CFile = 2,
	DFile = 3,
	EFile = 4,
	FFile = 5,
	GFile = 6,
	HFile = 7,
	None = 8
};
