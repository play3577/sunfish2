/*
 * nodeController.cpp
 *
 *  Created on: 2013/02/12
 *      Author: ryosuke
 */

#include "searcher.h"

namespace Search {
	using namespace Shogi;
	using namespace Evaluates;

	template <bool isRoot>
	void Searcher::NodeController::execute() {
		// extensions
		if (isCheckMove()) {
			depth += extension();
		} else if (stat.isRecapture() && isRecapture()) {
			depth += extension() * 3 / 4;
			newStat.unsetRecapture();
		} else if (isMateThreat()) {
			depth += extension() / 2;
		}

		if (!isHash() && moveCount != 1 && !isMateThreat()
				&& !tree.isCheck() && !isCheckMove()
				&& !isTacticalMove()
				&& depth >= (isRoot ? 2 * PLY1: PLY1)) {
			// late move reduction
			unsigned hist = tree.getHistory();
			if (!isNullWindow()) {
				if        (hist * 20U < History::SCALE) {
					reduction = Searcher::PLY1 * 3 / 2;
				} else if (hist *  6U < History::SCALE) {
					reduction = Searcher::PLY1;
				} else if (hist *  2U < History::SCALE) {
					reduction = Searcher::PLY1 / 2;
				}
			} else {
				if        (hist * 10U < History::SCALE) {
					reduction = Searcher::PLY1 * 2;
				} else if (hist *  6U < History::SCALE) {
					reduction = Searcher::PLY1 * 3 / 2;
				} else if (hist *  4U < History::SCALE) {
					reduction = Searcher::PLY1;
				} else if (hist *  1U < History::SCALE) {
					reduction = Searcher::PLY1 / 2;
				}
			}

			if (!isRoot) {
				// futility pruning
				if (standPat + estimate.getValue() + estimate.getError()
						+ getFutMgn(depth - reduction, moveCount)
						+ searcher.getGain(tree) <= alpha) {
					pruning = true;
					return;
				}
			}
		}
	}
	template void Searcher::NodeController::execute<true>();
	template void Searcher::NodeController::execute<false>();

	int Searcher::NodeController::extension() const {
		if (tree.getDepth() < rootDepth) {
			return Searcher::PLY1;
		} else if (tree.getDepth() < rootDepth * 2) {
			return Searcher::PLY1 * 3 / 4;
		} else if (tree.getDepth() < rootDepth * 3) {
			return Searcher::PLY1 / 2;
		} else {
			return Searcher::PLY1 / 4;
		}
	}

	Value Searcher::NodeController::getFutMgn(int depth, int count) {
		if (depth < Searcher::PLY1) {
			return 0;
		}
		return 120 * depth / PLY1 + 4 * count;
	}
}
