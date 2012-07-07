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
	private:
		static const unsigned MAX_MOVES_NUM = 1024;
		const Position& pos;
		unsigned num;
		unsigned curr;
		Move moves[MAX_MOVES_NUM];

		template <bool genCap, bool genNocap>
		unsigned generate();

		template <bool black, bool genCap, bool genNocap>
		void generateOnBoard();

		template <bool black, bool oneStep, bool promotable, bool genCap, bool genNocap>
		void generateStraight(const Piece piece, const Square from, const Direction dir, const Direction pin);

		template <bool black, bool promotable>
		void generateMoveOneMove(const Piece piece, const Square from, const Square to);

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

		bool swapTop(Move move);

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
