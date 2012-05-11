/*
 * position.h
 *
 *  Created on: 2012/05/10
 *      Author: ryosuke
 */

#ifndef POSITION_H_
#define POSITION_H_

namespace Shogi {
	class Position {
	private:
		static const unsigned UPPERMOST = 0x20;
		static const unsigned LOWERMOST = 0xA0;
		static const unsigned RIGHTMOST = 0x01;
		static const unsigned LEFTMOST = 0x09;
		static const unsigned MASK_FILE = 0x0F;
		static const unsigned MASK_RANK = 0xF0;
		static const unsigned WIDTH = 0x10;
		static const unsigned nextTable[];
		unsigned position;

	public:
		static const unsigned SIZE = 0xD0;
		static const unsigned TOP_W = 0x00;
		static const unsigned TOP = 0x21;
		static const unsigned END = 0xA9;
		static const unsigned END_W = 0xCF;
		static const unsigned FILE_NUM = 9;
		static const unsigned RANK_NUM = 9;

		Position() {
			position = TOP;
		}

		Position(int position) {
			this->position = position;
		}

		Position(const Position& pos) {
			position = pos.position;
		}

		void next() {
			position = nextTable[position];
		}

		void inc() {
			position++;
		}

		void right() {
			position--;
		}

		void left() {
			position++;
		}

		void up() {
			position -= WIDTH;
		}

		void down() {
			position += WIDTH;
		}

		void rightmost() {
			position = (position & MASK_RANK) | RIGHTMOST;
		}

		void leftmost() {
			position = (position & MASK_RANK) | LEFTMOST;
		}

		void uppermost() {
			position = (position & MASK_FILE) | UPPERMOST;
		}

		void lowermost() {
			position = (position & MASK_FILE) | LOWERMOST;
		}

		bool inside() const {
			return position <= END_W;
		}

		unsigned getIndex() const {
			return position;
		}

		unsigned getFile() const {
			return (position & MASK_FILE);
		}

		unsigned getRank() const {
			return (position & MASK_RANK) - 0x10;
		}
	};
}

#endif /* POSITION_H_ */
