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

		const Shogi::Move* getTop() const {
			if (num > 0) {
				return &moves[0];
			} else {
				return NULL;
			}
		}

		const Shogi::Move& get(int depth) const {
			return moves[depth];
		}

		std::string toString(int beginIndex = 0) const {
			std::ostringstream oss;
			for (int i = beginIndex; i < num; i++) {
				oss << moves[i].toString() << ' ';
			}
			return oss.str();
		}

		std::string toStringCsa(int beginIndex = 0) const {
			std::ostringstream oss;
			for (int i = beginIndex; i < num; i++) {
				oss << moves[i].toStringCsa() << ' ';
			}
			return oss.str();
		}
	};
}

#endif // PV_H_
