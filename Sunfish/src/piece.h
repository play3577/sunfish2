/*
 * piece.h
 *
 *  Created on: 2012/05/10
 *      Author: ryosuke
 */

#ifndef PIECE_H_
#define PIECE_H_

#include <string>

namespace Shogi {
	class Piece {
	private:
		unsigned piece;
		static const char pieceName[][4];
		static const char UNKNOWN_NAME[];

	public:
		enum {
			EMPTY = 0,

			PROMOTE = 0x08, // 8 (mask)

			TURN = 0x10,    // 16 (mask)

			WALL = 0x20,    // 32

			BPAWN = 1,      // 1
			BLANCE,         // 2
			BKNIGHT,        // 3
			BSILVER,        // 4
			BGOLD,          // 5
			BBISHOP,        // 6
			BROOK,          // 7
			BKING,          // 8
			BTOKIN,         // 9
			BPLANCE,        // 10
			BPKNIGHT,       // 11
			BPSILVER,       // 12

			BHORSE = 14,    // 14
			BDRAGON,        // 15

			WPAWN = 17,     // 17
			WLANCE,         // 18
			WKNIGHT,        // 19
			WSILVER,        // 20
			WGOLD,          // 21
			WBISHOP,        // 22
			WROOK,          // 23
			WKING,          // 24
			WTOKIN,         // 25
			WPLANCE,        // 26
			WPKNIGHT,       // 27
			WPSILVER,       // 28

			WHORSE = 30,    // 30
			WDRAGON,        // 31
		};

		Piece() {
			piece = EMPTY;
		}

		Piece(unsigned piece) {
			this->piece = piece;
		}

		Piece(const Piece& p) {
			piece = p.piece;
		}

		void promote() {
			piece |= PROMOTE;
		}

		void unPromote() {
			piece &= ~PROMOTE;
		}

		void turnBlack() {
			piece &= TURN;
		}

		void turnWhite() {
			piece |= TURN;
		}

		Piece getPromoted() const {
			Piece p(*this);
			p.promote();
			return p;
		}

		Piece getTurnedBlack() const {
			Piece p(*this);
			p.turnBlack();
			return p;
		}

		Piece getTurnedWhite() const {
			Piece p(*this);
			p.turnWhite();
			return p;
		}

		Piece& operator=(const Piece& p) {
			piece = p.piece;
			return *this;
		}

		unsigned getInteger() const {
			return piece;
		}

		bool isPromoted() const {
			return piece & PROMOTE;
		}

		bool isEmpty() const {
			return piece == EMPTY;
		}

		bool isWall() const {
			return piece == WALL;
		}

		bool isBlack() const {
			return !isWhite();
		}

		bool isWhite() const {
			return piece & TURN;
		}

		std::string toString() const {
			if (piece <= WDRAGON) {
				return pieceName[piece];
			} else {
				return UNKNOWN_NAME;
			}
		}
	};
}

#endif /* PIECE_H_ */
