/*
 * record.h
 *
 *  Created on: 2012/06/16
 *      Author: ryosuke
 */

#ifndef RECORD_H_
#define RECORD_H_

#include "../Shogi/position.h"

namespace Records {
	struct HashStack {
		const Util::uint64* stack;
		int size;
		HashStack(const Util::uint64* stack, int size) {
			this->stack = stack;
			this->size = size;
		}
	};

	class Record {
	private:
		static const int STACK_SIZE = 1024;
		Shogi::Position pos;
		int num;
		int cur;
		Shogi::Move moveStack[STACK_SIZE];
		Shogi::Change changeStack[STACK_SIZE];
		Util::uint64 hashStack[STACK_SIZE];

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
			hashStack[0] = pos.getHash();
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
				hashStack[cur] = pos.getHash();
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

		void begin() {
			while (prev())
				;
		}

		void end() {
			while (next())
				;
		}

		bool to(int index) {
			while (cur > index && prev())
				;
			while (cur < index && next())
				;
			return cur == index;
		}

		const Shogi::Position& getPosition() const {
			return pos;
		}

		int getSize() const {
			return num;
		}

		int getCurrent() const {
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

		Util::uint64 getHash(int index) {
			return hashStack[index];
		}

		bool isRepetition(bool full = true) const;

		const HashStack getHashStack() const {
			return HashStack(hashStack, cur);
		}

		std::string toString() const {
			return pos.toString();
		}
	};
}

#endif // RECORD_H_
