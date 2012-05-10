/*
 * position.h
 *
 *  Created on: 2012/05/10
 *      Author: ryosuke
 */

#ifndef POSITION_H_
#define POSITION_H_

class Position {
private:
	static const unsigned nextTable[];
	unsigned position;

public:
	static const unsigned WTOP = 0x00;
	static const unsigned TOP = 0x21;
	static const unsigned END = 0xA9;
	static const unsigned WEND = 0xCF;
	static const unsigned SIZE = 0xD0;
	static const unsigned MASK_FILE = 0x0F;
	static const unsigned MASK_RANK = 0xF0;
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

	bool inside() {
		return position <= WEND;
	}

	unsigned getIndex() {
		return position;
	}

	unsigned getFile() {
		return position & MASK_FILE;
	}

	unsigned getRank() {
		return position & MASK_RANK;
	}
};

#endif /* POSITION_H_ */
