/*
 * phasedMoveGenerator.cpp
 *
 *  Created on: 2012/07/06
 *      Author: ryosuke
 */

#include "phasedMoveGenerator.h"
#include "attackers.h"

namespace Search {
	using namespace Shogi;
	using namespace Evaluate;

	void PhasedMoveGenerator::sortSee(int begin, int num) {
		Value values[num];
		for (int i = 0; i < num; i++) {
			Attackers attackers(param, getPosition(), get(begin + i));
			Value value = attackers.see();
			int j;
			for (j = i; j > 0; j--) {
				if (values[j-1] >= value) {
					break;
				}
				values[j] = values[j-1];
			}
			values[j] = value;
			insertBefore(begin + i, begin + j);
		}
	}

	const Move* PhasedMoveGenerator::next() {
		while (true) {
			const Move* pmove = Super::next();
			if (pmove != NULL) {
				return pmove;
			}
			int prevNum = getNumber();
			switch (phase) {
			case PHASE_BEGIN:
				if (getPosition().isLegalMove(hashMove.getHash1(), true)) {
					add(hashMove.getHash1());
				}
				if (getPosition().isLegalMove(hashMove.getHash2(), true)) {
					add(hashMove.getHash2());
				}
				phase = PHASE_CAPTURE;
				break;
			case PHASE_CAPTURE:
				generateCapture();
				sortSee(prevNum, getNumber()-prevNum);
				phase = PHASE_NOCAPTURE;
				break;
			case PHASE_NOCAPTURE:
				generateNocapture();
				phase = PHASE_END;
				break;
			case PHASE_END:
				return NULL;
			}
		}
	}
}
