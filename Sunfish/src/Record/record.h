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

		void init(const Shogi::Position& pos) {
			initStack();
			this->pos.copy(pos);
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

		const Shogi::Position& getPosition() const {
			return pos;
		}

		int getNumberOfMoves() const {
			return num;
		}

		int getNumberOfCurrent() const {
			return cur;
		}

		bool getNextMove(Shogi::Move& move) const {
			if (cur < num) {
				move = moveStack[cur];
				return true;
			}
			return false;
		}

		bool getPrevMove(Shogi::Move& move) const {
			if (cur > 0) {
				move = moveStack[cur-1];
				return true;
			}
			return false;
		}

		std::string toString() const {
			return pos.toString();
		}
	};
}

#endif // RECORD_H_
