/*
 * moveGenerator.cpp
 *
 *  Created on: 2012/05/13
 *      Author: ryosuke
 */

#include "moveGenerator.h"

namespace Shogi {
	MoveGenerator::MoveGenerator(const Position& pos) : pos(pos) {
		clear();
	}

	unsigned MoveGenerator::generate() {
		clear();
		return num;
	}
}
