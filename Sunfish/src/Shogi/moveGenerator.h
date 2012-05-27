/*
 * moveGenerator.h
 *
 *  Created on: 2012/05/13
 *      Author: ryosuke
 */

#ifndef MOVEGENERATOR_H_
#define MOVEGENERATOR_H_

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

		void generateOnBoard();

		template <bool black>
		void generateOnBoard();

//		template <bool black>
//		void generate1Step(const Piece piece, const Square from, const Direction dir);

		template <bool black, bool oneStep, bool promotable>
		void generateStraight(const Piece piece, const Square from, const Direction dir, const Direction pin);

		template <bool black, bool check>
		void generateKing();

		template <bool black, bool check>
		void generateKingDirection(const Direction& dir);

		template <bool black>
		void generateDrop();

		template <bool black, unsigned piece>
		void generateDropPieces();

		template <bool black>
		void generateEvasion();

	public:
		MoveGenerator(const Position& pos);

		void clear() {
			num = 0;
			curr = 0;
		}

		unsigned generate();

		unsigned getNumber() const {
			return num;
		}

		void setCurrent(unsigned curr) {
			this->curr = curr;
		}

		unsigned getCurrent() const {
			return curr;
		}

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