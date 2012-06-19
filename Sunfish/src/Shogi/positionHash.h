/*
 * positionHash.h
 *
 *  Created on: 2012/06/10
 *      Author: ryosuke
 */

#ifndef POSITIONHASH_H_
#define POSITIONHASH_H_

#include "../Util/int.h"
#include "piece.h"
#include "square.h"
#include "hand.h"

namespace Shogi {
	class PositionHash {
	private:
		Util::uint64 board[Piece::WDRAGON+1][Square::END+1];
		Util::uint64 blackHand[Piece::WDRAGON+1][Hand::MAX];
		Util::uint64 whiteHand[Piece::WDRAGON+1][Hand::MAX];
		Util::uint64 black;

	public:
		static const char* FILE_NAME;

		PositionHash() {
			init();
		}

		PositionHash(const char* filename) {
			if (!read(filename)) {
				init();
				write(filename);
			}
		}

		void init();

		bool read(const char* filename);

		bool write(const char* filename);

		Util::uint64 getBoard(const Piece& piece, const Square& square) {
			return board[piece][square];
		}

		Util::uint64 getBlackHand(const Piece& piece, int h) {
			return blackHand[piece.getTurnedBlack()][h];
		}

		Util::uint64 getWhiteHand(const Piece& piece, int h) {
			return whiteHand[piece.getTurnedBlack()][h];
		}

		Util::uint64 getBlack() {
			return black;
		}
	};
}

#endif // POSITIONHASH_H_
