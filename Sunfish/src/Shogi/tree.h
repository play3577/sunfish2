/*
 * tree.h
 *
 *  Created on: 2012/06/16
 *      Author: ryosuke
 */

#ifndef TREE_H_
#define TREE_H_

#include "position.h"
#include "node.h"

namespace Shogi {
	class Tree {
	private:
		static const MAX_DEPTH = 1024;
		Position position;
		Node node[MAX_DEPTH];

	public:

	};
}

#endif // TREE_H_
