#pragma once
#include "Defines.h"
#include "PositionData.h"
#include "Board.h"
#include "MoveGen.h"

class Position
{
private:
	Board mBoard;
	Color mTurn;
	CRightsFlags mCRights;
	BYTE mEPCol;
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
	MoveGenData GeneratePseudoLegalMoves(Move buffer[], const size_t bufferIndex);
	void makeMove(const Move move);
	void unmakeMove(const Move move);
};

