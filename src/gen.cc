/*
 * Demolito, a UCI chess engine.
 * Copyright 2015 Lucas Braesch.
 *
 * Demolito is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Demolito is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include <cassert>
#include <iostream>
#include "gen.h"
#include "bitboard.h"

namespace {

template <bool Promotion>
Move *serialize_moves(Move& m, bitboard_t tss, Move *mlist)
{
	while (tss) {
		m.tsq = bb::pop_lsb(tss);
		if (Promotion) {
			for (m.prom = QUEEN; m.prom >= KNIGHT; m.prom--)
				*mlist++ = m;
		} else
			*mlist++ = m;
	}

	return mlist;
}

}	// namespace

namespace gen {

Move *pawn_moves(const Position& pos, Move *mlist, bitboard_t targets)
{
	assert(!pos.checkers());
	int us = pos.turn(), them = opp_color(us), push = push_inc(us);
	bitboard_t capturable = pos.occ(them) | pos.ep_square_bb();
	bitboard_t fss, tss;
	Move m;

	// Non promotions
	fss = pos.occ(us, PAWN) & ~bb::relative_rank(us, RANK_7);
	while (fss) {
		m.fsq = bb::pop_lsb(fss);

		// Calculate to squares: captures, single pushes and double pushes
		tss = bb::pattacks(us, m.fsq) & capturable & targets;
		if (bb::test(~pos.occ(), m.fsq + push)) {
			if (bb::test(targets, m.fsq + push))
				bb::set(tss, m.fsq + push);
			if (relative_rank(us, m.fsq) == RANK_2
			&& bb::test(targets & ~pos.occ(), m.fsq + 2 * push))
				bb::set(tss, m.fsq + 2 * push);
		}

		// Generate moves
		m.prom = NB_PIECE;
		mlist = serialize_moves<false>(m, tss, mlist);
	}

	// Promotions
	fss = pos.occ(us, PAWN) & bb::relative_rank(us, RANK_7);
	while (fss) {
		m.fsq = bb::pop_lsb(fss);

		// Calculate to squares: captures and single pushes
		tss = bb::pattacks(us, m.fsq) & capturable & targets;
		if (bb::test(targets & ~pos.occ(), m.fsq + push))
			bb::set(tss, m.fsq + push);

		// Generate moves (or promotions)
		mlist = serialize_moves<true>(m, tss, mlist);
	}

	return mlist;
}

Move *piece_moves(const Position& pos, Move *mlist, bitboard_t targets,
	bool kingMoves)
{
	assert(!pos.checkers());
	int us = pos.turn();
	bitboard_t fss, tss;

	Move m;
	m.prom = NB_PIECE;

	// King moves
	if (kingMoves) {
		m.fsq = pos.king_square(us);
		tss = bb::kattacks(m.fsq) & targets;
		mlist = serialize_moves<false>(m, tss, mlist);
	}

	// Knight moves
	fss = pos.occ(us, KNIGHT);
	while (fss) {
		m.fsq = bb::pop_lsb(fss);
		tss = bb::nattacks(m.fsq) & targets;
		mlist = serialize_moves<false>(m, tss, mlist);
	}

	// Rook moves
	fss = pos.occ_RQ(us);
	while (fss) {
		m.fsq = bb::pop_lsb(fss);
		tss = bb::rattacks(m.fsq, pos.occ()) & targets;
		mlist = serialize_moves<false>(m, tss, mlist);
	}

	// Bishop moves
	fss = pos.occ_BQ(us);
	while (fss) {
		m.fsq = bb::pop_lsb(fss);
		tss = bb::battacks(m.fsq, pos.occ()) & targets;
		mlist = serialize_moves<false>(m, tss, mlist);
	}

	return mlist;
}

Move *castling_moves(const Position& pos, Move *mlist)
{
	assert(!pos.checkers());
	Move m;
	m.fsq = pos.king_square(pos.turn());
	m.prom = NB_PIECE;

	bitboard_t tss = pos.castlable_rooks() & pos.occ(pos.turn());
	while (tss) {
		m.tsq = bb::pop_lsb(tss);
		if (bb::count(bb::segment(m.fsq, m.tsq) & pos.occ()) == 2)
			*mlist++ = m;
	}

	return mlist;
}

Move *check_escapes(const Position& pos, Move *mlist)
{
	assert(pos.checkers());
	bitboard_t ours = pos.occ(pos.turn());
	int ksq = pos.king_square(pos.turn());
	bitboard_t tss;
	Move m;

	// King moves
	tss = bb::kattacks(ksq) & ~ours;
	m.prom = NB_PIECE;
	mlist = serialize_moves<false>(m, tss, mlist);

	if (!bb::several(pos.checkers())) {
		// Single checker
		int checkerSquare = bb::lsb(pos.checkers());
		int checkerPiece = pos.piece_on(checkerSquare);

		// Piece moves must cover the checking segment for a sliding check,
		// or capture the checker otherwise.
		tss = BISHOP <= checkerPiece && checkerPiece <= QUEEN
			? bb::segment(ksq, checkerSquare)
			: pos.checkers();

		mlist = piece_moves(pos, mlist, ~ours, false);

		// if checked by a Pawn and epsq is available, then the check must result
		// from a pawn double push, and we also need to consider capturing it en
		// passant to solve the check
		if (checkerPiece == PAWN && square_ok(pos.ep_square()))
			bb::set(tss, pos.ep_square());

		mlist = pawn_moves(pos, mlist, tss);
	}

	return mlist;
}

Move *all_moves(const Position& pos, Move *mlist)
{
	if (pos.checkers())
		return check_escapes(pos, mlist);
	else {
		bitboard_t targets = ~pos.occ(pos.turn());
		Move *m = mlist;

		m = pawn_moves(pos, m, targets);
		m = piece_moves(pos, m, targets);
		m = castling_moves(pos, m);
		return m;
	}
}

}	// namespace gen
