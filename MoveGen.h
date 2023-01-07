#pragma once
#include "Defines.h"
#include "Position.h"
#include <stdexcept>

#define MAX_PLY 256
#define MOVE_GEN_BUF 50
#define MOVE_BUF_CAPACITY MAX_PLY * MOVE_GEN_BUF

enum MoveFlags {
	SpecFlag0 = 1,
	SpecFlag1 = 2,
	CaptureFlag = 4,
	PromoFlag = 8,

	Quiet = 0,
	DoublePawn = 1,
	KCastle = 2,
	QCastle = 3,
	Capture = 4,
	EPCapture = 5,
	KnightPromo = 8,
	BishopPromo = 9,
	RookPromo = 10,
	QueenPromo = 11,
	KnightPromoCapture = 12,
	BishopPromoCapture = 13,
	RookPromoCapture = 14,
	QueenPromoCapture = 15,
};

class Move {
private:
	WORD mData;
public:
	Move(Square from, Square to, MoveFlags flags) {
		mData = ((flags & 0xf) << 12) | ((from & 0x3f) << 6) | (to & 0x3f);
	}
	inline Square getTo() const { return (Square)(mData & 0x3f); }
	inline Square getFrom() const { return (Square)((mData >> 6) & 0x3f); }
	inline MoveFlags getFlags() const { return (MoveFlags)((mData >> 12) & 0x0f); }
	void setTo(Square to) {
		mData &= ~0x3f; 
		mData |= to & 0x3f; 
	}
	void setFrom(Square from) {
		mData &= ~0xfc0; 
		mData |= (from & 0x3f) << 6; 
	}
};

struct MoveGenData {
	BYTE moveCount;
	MoveGenData(BYTE moveCount) {
		this->moveCount = moveCount;
	}
};

class MoveGen {
private:
	Position mPos;
	MoveGenData mMoveHistory[MAX_PLY];
	size_t mPlyIdx = 0;
	Move mMoveBuf[MOVE_BUF_CAPACITY];
	size_t mBufIdx = 0;
public:
	inline const Position& getPosition() const { return mPos; }
	inline void GeneratePseudoLegalMoves() {
		if (mPlyIdx == MAX_PLY) throw std::out_of_range("mMoveHistory overflowed");
		MoveGenData data = mPos.GeneratePseudoLegalMoves(mMoveBuf, mBufIdx);
		mBufIdx += data.moveCount;
		mMoveHistory[mPlyIdx++] = data;
	}
	inline void AbandonMove() {
		if (mPlyIdx == 0) throw std::out_of_range("mMoveHistory underflowed");
		MoveGenData data = mMoveHistory[--mPlyIdx];
		mBufIdx -= data.moveCount;
	}
	inline bool NextMove() {
		if (mMoveHistory[mPlyIdx].moveCount == 0) return false;
		// TODO: implement this.
	}
};
