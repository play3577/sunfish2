/*
 * tree.cpp
 *
 *  Created on: 2013/02/23
 *      Author: ryosuke
 */

#include "tree.h"

namespace Search {
	void Tree::fastCopy(Tree& tree) {
		// root 局面まで戻る。
		while (depth >= 1) {
			unmakeMove();
		}

		// 同じ指し手を進めていく。
		for (int i = 0; i < tree.depth; i++) {
			if (!tree.nodes[i].isNullMove()) {
				nodes[i].setMove(tree.nodes[i].getMove());
				makeMove(tree.nodes[i].isShek());
			} else {
				nullMove(tree.nodes[i].isShek());
			}
		}

		nodes[depth].arrive();
		nodes[depth].setKiller(tree.nodes[depth]);
	}
}
