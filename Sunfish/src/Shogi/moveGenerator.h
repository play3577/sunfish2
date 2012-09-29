/*
 * moveGenerator.h
 *
 *  Created on: 2012/05/13
 *      Author: ryosuke
 */

#ifndef MOVEGENERATOR_H_
#define MOVEGENERATOR_H_

#include <boost/range/algorithm.hpp>
#include "position.h"
#include "move.h"

namespace Shogi {
	class MoveGenerator {
	public:
		static const unsigned MAX_MOVES_NUM = 1024;

	private:
		const Position& pos;
		unsigned num;
		unsigned curr;
		Move moves[MAX_MOVES_NUM];

		template <bool genCap, bool genNocap>
		unsigned generate();

		template <bool black, bool genCap, bool genNocap, bool genPro>
		void generateOnBoard();

		template <bool black, bool oneStep, bool promotable, bool genCap, bool genNocap, bool genPro>
		void generateStraight(const Piece piece, const Square from, const Direction dir, const Direction pin);

		template <bool black>
		void generateMoveOneMove(const Piece piece, const Square from, const Square to) {
			if (!to.isCompulsoryPromotion(piece)) {
				moves[num++] = Move(from, to, false, false, piece);
			}
		}

		template <bool black>
		void generateMoveOneMovePro(const Piece piece, const Square from, const Square to) {
			if (to.isPromotableRank(black) || from.isPromotableRank(black)) {
				moves[num++] = Move(from, to, true, false, piece);
			}
		}

		template <bool black, bool check, bool genCap, bool genNocap>
		void generateKing();

		template <bool black, bool check, bool genCap, bool genNocap>
		void generateKingDirection(const Direction& dir);

		template <bool black>
		void generateDrop();

		template <bool black, unsigned piece>
		void generateDropPieces();

		template <bool black, bool genCap, bool genNocap>
		void generateEvasion();

		template <bool black>
		void generateEvasionOnBoard(Square to);

		template <bool black, unsigned piece>
		void generateEvasionDrop(Square to);

	public:
		MoveGenerator(const Position& pos) : pos(pos) {
			clear();
		}

		const Position& getPosition() const {
			return pos;
		}

		void clear() {
			num = 0;
			curr = 0;
		}

		unsigned generate() {
			return generate<true, true>();
		}

		unsigned generateTardy();

		unsigned generateCapture() {
			return generate<true, false>();
		}

		unsigned generateNocapture() {
			return generate<false, true>();
		}

		unsigned generateTactical();

		unsigned add(const Move& move) {
			moves[num++] = move;
			return num;
		}

		unsigned getNumber() const {
			return num;
		}

		void setCurrent(unsigned curr) {
			this->curr = curr;
		}

		unsigned getCurrent() const {
			return curr;
		}

		void sort() {
			sort(0, num);
		}

		void sort(unsigned begin) {
			sort(begin, num - begin);
		}

		void sort(unsigned begin, unsigned size);

		void insertBefore(int from, int to) {
			Move move = moves[from];
			for (int i = from; i > to; i--) {
				moves[i] = moves[i-1];
			}
			moves[to] = move;
		}

		bool swapTop(Move move) {
			if (num > 0 && moves[0] == move) {
				return true;
			}
			for (int i = 1; (unsigned)i < num; i++) {
				if (moves[i] == move) {
					moves[i] = moves[0];
					moves[0] = move;
					return true;
				}
			}
			return false;
		}

		bool equals(const MoveGenerator gen) const;

		const Move* next() {
			if (curr < num) {
				return &moves[curr++];
			} else {
				return NULL;
			}
		}

		const Move& get(unsigned index) const {
			return moves[index];
		}
	};
}

#endif /* MOVEGENERATOR_H_ */
