/*
 * phasedMoveGenerator.cpp
 *
 *  Created on: 2012/07/06
 *      Author: ryosuke
 */

#include "phasedMoveGenerator.h"

namespace Search {
	using namespace Shogi;

	const Move* PhasedMoveGenerator::next() {
		while (true) {
			const Move* pmove = Super::next();
			if (pmove != NULL) {
				return pmove;
			}
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
