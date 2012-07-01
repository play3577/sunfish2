/*
 * record.h
 *
 *  Created on: 2012/06/16
 *      Author: ryosuke
 */

#ifndef RECORD_H_
#define RECORD_H_

#include "../Shogi/position.h"

namespace Record {
	class Record {
	private:
		static const int STACK_SIZE = 1024;
		Shogi::Position pos;
		int num;
		int cur;
		Shogi::Move moveStack[STACK_SIZE];
		Shogi::Change changeStack[STACK_SIZE];

	public:
		Record() {
			initStack();
		}

		Record(const Shogi::Handicap& handicap) : pos(handicap) {
			initStack();
		}

		Record(const Shogi::Position& pos) : pos(pos) {
			initStack();
		}

		void initStack() {
			num = 0;
			cur = 0;
		}

		bool move(const Shogi::Move& move) {
			Shogi::Change change;
			if (cur < STACK_SIZE && pos.move(move, change)) {
				moveStack[cur] = move;
				changeStack[cur] = change;
				num = ++cur;
				return true;
			}
			return false;
		}

		bool next() {
			if (cur < num) {
				pos.move(moveStack[cur]);
				cur++;
				return true;
			}
			return false;
		}

		bool prev() {
			if (cur > 0) {
				cur--;
				pos.back(changeStack[cur]);
				return true;
			}
			return false;
		}

		int getNumberOfMoves() const {
			return num;
		}

		int getNumberOfCurrent() const {
			return cur;
		}

		std::string toString() const {
			return pos.toString();
		}
	};
}

#endif // RECORD_H_