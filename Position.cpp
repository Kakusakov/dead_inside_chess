#include "Position.h"

MoveGenData Position::GeneratePseudoLegalMoves(Move buffer[], const size_t bufferIndex) {
	size_t curBufferIndex = bufferIndex;
	Color opponent = (Color)((mTurn + 1) & 1);
	U64 opponentBB = mBoard.getColor(opponent);
	U64 occ = mBoard.getOccupance();
	// TODO: Check for / eliminate possible buffer overflow.
	auto serializeMoves = [&buffer, &curBufferIndex](Square from, U64 moves, MoveFlags flags) {
		while (moves) {
			buffer[curBufferIndex++] = Move(from, bitScanForward(moves), flags);
			moves &= moves - 1;
		}
	};
	// if double check
	{
		U64 checkingPieces = getCheckingPieces();
		if (checkingPieces & (checkingPieces - 1)) {  // double check
			Square kingSq = bitScanForward(mBoard.getColoredKing(mTurn));
			serializeMoves(kingSq, kingAttacks[kingSq] & opponentBB, MoveFlags::Capture);
			serializeMoves(kingSq, kingAttacks[kingSq] & ~occ, MoveFlags::Quiet);
			return MoveGenData(curBufferIndex - bufferIndex);
		}
	}
	U64 pawnBB = mBoard.getColoredPieces(mTurn, Piece::Pawn);
	// pawn attacks
	{
		U64 promoters = pawnBB & (mTurn == Color::White ? rank7 : rank2);
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
			promoters &= promoters - 1;
		}
		U64 nonPromoters = pawnBB & ~promoters;
		U64 enPassantPos = fileMask((Square)mEPCol) & (mTurn == Color::White ? rank6 : rank3);
		while (nonPromoters) {
			Square from = bitScanForward(nonPromoters);
			serializeMoves(from, pawnAttacks[mTurn][from] & opponentBB, MoveFlags::Capture);
			serializeMoves(from, pawnAttacks[mTurn][from] & enPassantPos, MoveFlags::EPCapture);
			nonPromoters &= nonPromoters - 1;
		}
	}
	
	// pawn pushes
	{
		U64 pawnPushes = singlePushTargets(pawnBB, ~occ, mTurn);
		U64 promotionPushes = pawnPushes & (mTurn == Color::White ? rank8 : rank1);
		pawnPushes &= ~promotionPushes;
		int pushOffset = mTurn == Color::White ? -8 : 8;
		while (pawnPushes) {
			Square to = bitScanForward(pawnPushes);
			buffer[curBufferIndex++] = Move((Square)(to + pushOffset), to, MoveFlags::Quiet);
			pawnPushes &= pawnPushes - 1;
		}
		U64 dblPawnPushes = singlePushTargets(pawnPushes, ~occ, mTurn) & (mTurn == Color::White ? rank4 : rank5);
		while (dblPawnPushes) {
			Square to = bitScanForward(dblPawnPushes);
			buffer[curBufferIndex++] = Move((Square)(to + pushOffset * 2), to, MoveFlags::DoublePawn);
			dblPawnPushes &= dblPawnPushes - 1;
		}
		while (promotionPushes) {
			Square to = bitScanForward(promotionPushes);
			Square from = (Square)(to + pushOffset);
			buffer[curBufferIndex++] = Move(from, to, MoveFlags::QueenPromo);
			buffer[curBufferIndex++] = Move(from, to, MoveFlags::RookPromo);
			buffer[curBufferIndex++] = Move(from, to, MoveFlags::KnightPromo);
			buffer[curBufferIndex++] = Move(from, to, MoveFlags::BishopPromo);
			promotionPushes &= promotionPushes - 1;
		}
	}

	// knights
	{
		U64 knightBB = mBoard.getColoredPieces(mTurn, Piece::Knight);
		while (knightBB) {
			Square knightSq = bitScanForward(knightBB);
			serializeMoves(knightSq, knightAttacks[knightSq] & opponent, MoveFlags::Capture);
			serializeMoves(knightSq, knightAttacks[knightSq] & ~occ, MoveFlags::Quiet);
			knightBB &= knightBB - 1;
		}
	}

	// king
	{
		Square kingSq = bitScanForward(mBoard.getColoredKing(mTurn));
		serializeMoves(kingSq, kingAttacks[kingSq] & opponentBB, MoveFlags::Capture);
		serializeMoves(kingSq, kingAttacks[kingSq] & ~occ, MoveFlags::Quiet);
	}

	// bishop likes
	{
		U64 bishopLike = mBoard.getBishopLikeSliders() & mBoard.getColor(mTurn);
		while (bishopLike) {
			Square from = bitScanForward(bishopLike);
			U64 attacks = bishopAttacks(occ, from);
			serializeMoves(from, attacks & opponentBB, MoveFlags::Capture);
			serializeMoves(from, attacks & ~occ, MoveFlags::Quiet);
			bishopLike &= bishopLike - 1;
		}
	}

	// rook likes
	{
		U64 rookLike = mBoard.getRookLikeSliders() & mBoard.getColor(mTurn);
		while (rookLike) {
			Square from = bitScanForward(rookLike);
			U64 attacks = rookAttacks(occ, from);
			serializeMoves(from, attacks & opponentBB, MoveFlags::Capture);
			serializeMoves(from, attacks & ~occ, MoveFlags::Quiet);
			rookLike &= rookLike - 1;
		}
	}

	// TODO: Generate castles.

	return MoveGenData(curBufferIndex - bufferIndex);
}

void Position::makeMove(const Move move)
{
	U64 fromBB = C64(1) << move.getFrom();
	U64 toBB = C64(1) << move.getTo();
	MoveFlags flags = move.getFlags();
	
	switch (flags) {
	case MoveFlags::Quiet:
	case MoveFlags::Capture:
		Piece piece;  // does this violate anything?
		if (mBoard.getColoredPieces(mTurn, Piece::Pawn) & fromBB) piece = Piece::Pawn;
		else if (mBoard.getColoredPieces(mTurn, Piece::Bishop) & fromBB) piece = Piece::Bishop;
		else if (mBoard.getColoredPieces(mTurn, Piece::Knight) & fromBB) piece = Piece::Knight;
		else if (mBoard.getColoredPieces(mTurn, Piece::Rook) & fromBB) piece = Piece::Rook;
		else if (mBoard.getColoredPieces(mTurn, Piece::Queen) & fromBB) piece = Piece::Queen;
		else piece = Piece::King;

		if (flags == MoveFlags::Capture) {
			// Do I need to save some data here or will it be in the MoveGen?
			// TODO: remove enemy piece
		}

		// TODO: make move

		break;
	default:
		throw "Not implemented.";
		break;
	}
	// TODO: implement this.

	throw "Not implemented.";
}

void Position::unmakeMove(const Move move)
{
	// TODO: implement this.

	throw "Not implemented.";
}
