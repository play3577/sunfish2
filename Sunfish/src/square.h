/*
 * square.h
 *
 *  Created on: 2012/05/10
 *      Author: ryosuke
 */

#ifndef SQUARE_H_
#define SQUARE_H_

#include "piece.h"
#include "direction.h"

namespace Shogi {
	class Square {
	private:
		static const unsigned UPPERMOST = 0x20;
		static const unsigned LOWERMOST = 0xA0;
		static const unsigned RIGHTMOST = 0x01;
		static const unsigned LEFTMOST = 0x09;
		static const unsigned MASK_FILE = 0x0F;
		static const unsigned MASK_RANK = 0xF0;
		static const unsigned SHIFT_RANK = 4;
		static const unsigned OFFSET = 0x10;
		static const unsigned WIDTH = 0x10;
		static const unsigned nextTable[];
		unsigned square;

	public:
		static const unsigned SIZE = 0xD0;
		static const unsigned TOP_W = 0x00;
		static const unsigned TOP = 0x21;
		static const unsigned END = 0xA9;
		static const unsigned END_W = 0xCF;
		static const unsigned FILE_NUM = 9;
		static const unsigned RANK_NUM = 9;

		Square() {
			square = TOP;
		}

		Square(int square) {
			this->square = square;
		}

		Square(int file, int rank) {
			square = file + (rank << SHIFT_RANK) + OFFSET;
		}

		Square(const Square& sq) {
			square = sq.square;
		}

		void next() {
			square = nextTable[square];
		}

		void inc() {
			square++;
		}

		void right() {
			square--;
		}

		void left() {
			square++;
		}

		void up() {
			square -= WIDTH;
		}

		void down() {
			square += WIDTH;
		}

		void to(const Direction& dir) {
			square += (unsigned)dir;
		}

		void to(const Direction& dir, int num) {
			square += (unsigned)dir * num;
		}

		void operator+=(const Direction& dir) {
			square += (unsigned)dir;
		}

		Square add(const Direction& dir) const {
			Square sq(*this);
			sq += dir;
			return sq;
		}

		Square operator+(const Direction& dir) const {
			return add(dir);
		}

		operator unsigned() const {
			return square;
		}

		void rightmost() {
			square = (square & MASK_RANK) | RIGHTMOST;
		}

		void leftmost() {
			square = (square & MASK_RANK) | LEFTMOST;
		}

		void uppermost() {
			square = (square & MASK_FILE) | UPPERMOST;
		}

		void lowermost() {
			square = (square & MASK_FILE) | LOWERMOST;
		}

		bool valid() const {
			return square <= END_W;
		}

		unsigned getIndex() const {
			return square;
		}

		unsigned getFile() const {
			return (square & MASK_FILE);
		}

		unsigned getRank() const {
			return ((square & MASK_RANK) - OFFSET) >> SHIFT_RANK;
		}

		template <bool black>
		bool isPromotableRank() {
			if (black) {
				return square <= Square(9, 3).getIndex();
			} else {
				return square >= Square(1, 7).getIndex();
			}
		}

		bool isCompulsoryPromotion(const Piece& piece) {
			switch (piece.getInteger()) {
			case Piece::BPAWN: case Piece::BLANCE:
				return square <= Square(9, 1).getIndex();
			case Piece::BKNIGHT:
				return square <= Square(9, 2).getIndex();
			case Piece::WPAWN: case Piece::WLANCE:
				return square >= Square(1, 8).getIndex();
			case Piece::WKNIGHT:
				return square >= Square(1, 7).getIndex();
			default:
				return false;
			}
		}

		std::string toString() const;
	};
}

#endif /* SQUARE_H_ */
