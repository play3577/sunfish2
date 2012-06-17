/*
 * node.h
 *
 *  Created on: 2012/06/16
 *      Author: ryosuke
 */

#ifndef NODE_H_
#define NODE_H_

#include "moveGenerator.h"

namespace Shogi {
	class Node {
	private:
		MoveGenerator<Move> gen;
		Move move;

	public:

	};
}

#endif // NODE_H_
