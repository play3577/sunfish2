/*
 * pv.h
 *
 *  Created on: 2012/07/04
 *      Author: ryosuke
 */

#ifndef PV_H_
#define PV_H_

#include "sstream"
#include "../Shogi/move.h"

namespace Search {
	class Pv {
	private:
		static const int MAX_DEPTH = 64;
		int num;
		Shogi::Move moves[MAX_DEPTH];

		int min(int a, int b) {
			return a < b ? a : b;
		}

	public:
		Pv() {
			init();
		}

		Pv(const Pv& pv) {
			copy(pv);
		}

		void copy(const Pv& pv) {
			num = pv.num;
			memcpy(moves, pv.moves, sizeof(Shogi::Move) * num);
		}

		void init() {
			num = 0;
		}

		int size() const {
			return num;
		}

		int set(const Shogi::Move& move, const Pv& pv) {
			moves[0] = move;
			num = min(pv.num + 1, MAX_DEPTH);
			memcpy(&moves[1], pv.moves, sizeof(Shogi::Move) * (num - 1));
			return num;
		}

		const Shogi::Move& get(int depth) {
			return moves[depth];
		}

		std::string toString() const {
			std::ostringstream oss;
			for (int i = 0; i < num; i++) {
				oss << moves[i].toString() << ' ';
			}
			return oss.str();
		}
	};
}

#endif // PV_H_
