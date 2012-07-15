/*
 * phasedMoveGenerator.h
 *
 *  Created on: 2012/07/06
 *      Author: ryosuke
 */

#ifndef PHASED_MOVE_GENERATOR_H_
#define PHASED_MOVE_GENERATOR_H_

#include "../Shogi/moveGenerator.h"
#include "../Evaluate/param.h"
#include "hashMove.h"
#include "history.h"

namespace Search {
	enum PHASE {
		PHASE_BEGIN = 0,
		PHASE_END,

		PHASE_CAPTURE,
		PHASE_NOCAPTURE,
	};

	class PhasedMoveGenerator : public Shogi::MoveGenerator {
	private:
		PHASE phase;
		HashMove hashMove;
		const Evaluate::Param& param;

		void sortSee(int begin, int num);

	public:
		typedef Shogi::MoveGenerator Super;

		PhasedMoveGenerator(const Shogi::Position& pos,
				const Evaluate::Param& param) :
				Shogi::MoveGenerator(pos),
				param(param) {
			init();
		}

		void init() {
			clear();
			phase = PHASE_BEGIN;
			hashMove.init();
		}

		const Shogi::Move* next();

		void setHashMove(const HashMove& hashMove) {
			this->hashMove = hashMove;
		}
	};
}

#endif // PHASED_MOVE_GENERATOR_H_
