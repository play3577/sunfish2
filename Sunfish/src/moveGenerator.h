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

		template <bool black>
		void generate1Step(const Piece piece, const Square from, const Direction dir);

		template <bool black>
		void generateStraight(const Piece piece, const Square from, const Direction dir);

	public:
		MoveGenerator(const Position& pos);

		void clear() {
			num = 0;
			curr = 0;
		}

		unsigned generate();

		const Move* next() {
			if (curr < num) {
				return &moves[curr++];
			} else {
				return NULL;
			}
		}
	};
}

#endif /* MOVEGENERATOR_H_ */
