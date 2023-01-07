#include "Position.h"

MoveGenData Position::GeneratePseudoLegalMoves(Move buffer[], const size_t bufferIndex) {
	size_t curBufferIndex = bufferIndex;
	Color opponent = (Color)((mTurn + 1) & 1);
	U64 opponentBB = mBoard.getColor(opponent);
	U64 occ = mBoard.getOccupance();
	// TODO: Pervent buffer overflow.
	auto serializeMoves = [&buffer, &curBufferIndex](Square from, U64 moves, MoveFlags flags) {
		while (moves) {
			buffer[curBufferIndex++] = Move(from, bitScanForward(moves), flags);
			moves &= moves - 1;
		}
	};
	U64 checkingPieces = getCheckingPieces();
	if (checkingPieces & (checkingPieces - 1)) {  // double check
		Square kingSq = bitScanForward(mBoard.getColoredKing(mTurn));
		serializeMoves(kingSq, kingAttacks[kingSq] & opponentBB, MoveFlags::Capture);
		serializeMoves(kingSq, kingAttacks[kingSq] & ~occ, MoveFlags::Quiet);
		return MoveGenData(curBufferIndex - bufferIndex);
	}
	U64 pawnBB = mBoard.getColoredPieces(mTurn, Piece::Pawn);
	U64 promoters = pawnBB & (rank2 | rank7);
	while (promoters) {
		Square promoterSq = bitScanForward(promoters);
		U64 promotionAttacks = pawnAttacks[mTurn][promoterSq] & opponentBB;
		while (promotionAttacks) {
			Square to = bitScanForward(promotionAttacks);
			buffer[curBufferIndex++] = Move(promoterSq, to, MoveFlags::QueenPromoCapture);
			buffer[curBufferIndex++] = Move(promoterSq, to, MoveFlags::RookPromoCapture);
			buffer[curBufferIndex++] = Move(promoterSq, to, MoveFlags::KnightPromoCapture);
			buffer[curBufferIndex++] = Move(promoterSq, to, MoveFlags::BishopPromoCapture);
			promotionAttacks &= promotionAttacks - 1;
		}
		// TODO: Implement quiet promotions.
		promoters &= promoters - 1;
	}

	// TODO: Finish implementing this.

	throw "Not implemented";
}