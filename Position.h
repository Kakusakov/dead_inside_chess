#pragma once
#include "Defines.h"
#include "PositionData.h"
#include "Board.h"
#include "MoveGen.h"

class Position
	// TODO: Position and MoveGen classes should be merged.
{
private:
	Board mBoard;
	Color mTurn;
	CRightsFlags mCRights;
	EnPassant mEnPassant;  
	BYTE mHMClock;
public:
	inline U64 getCheckingPieces() { 
		return (mBoard.getAttacksTo(
			bitScanForward(mBoard.getColoredKing(mTurn))
		) & mBoard.getColor((Color)((mTurn + 1) & 1)));
	}
	inline bool canCaptureKing() {
		return (mBoard.getAttacksTo(
			bitScanForward(mBoard.getColoredKing((Color)((mTurn + 1) & 1)))
		) & mBoard.getColor(mTurn)) != 0;
	}
	// TODO: may want to separate move generation into stages and generate those move in
	// separate functions. 
	// The functions should probably be private since it may be good to abstract the
	// exact workings of stages away from the end user.
	MoveGenData GenerateMoves(Move buffer[], const size_t bufferIndex);
	void makeMove(const Move move);
	void unmakeMove(const Move move);
};

