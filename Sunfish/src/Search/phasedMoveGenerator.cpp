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
	using namespace Evaluates;

	void PhasedMoveGenerator::sort(Value values[]) {
		for (unsigned i = 0; i < getNumber(); i++) {
			unsigned j;
			Value value = values[i];
			for (j = i; j > 0; j--) {
				if (values[j-1] >= value) {
					break;
				}
				values[j] = values[j-1];
			}
			values[j] = value;
			insertBefore(i, j);
		}
	}

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

	void PhasedMoveGenerator::sortHistory(int begin, int num) {
		Value values[num];
		for (int i = 0; i < num; i++) {
			Value value = history.get(get(begin+i));
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
					hashNum++;
				}
				if (getPosition().isLegalMove(hashMove.getHash2(), true)) {
					add(hashMove.getHash2());
					hashNum++;
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
				sortHistory(prevNum, getNumber()-prevNum);
				phase = PHASE_END;
				break;
			case PHASE_TACTICAL:
				generateTactical();
				sortSee(prevNum, getNumber()-prevNum);
				phase = PHASE_END;
				break;
			case PHASE_CAPTURE_ONLY:
				generateCapture();
				sortSee(prevNum, getNumber()-prevNum);
				phase = PHASE_END;
				break;
			case PHASE_END:
				return NULL;
			}
		}
	}
}
