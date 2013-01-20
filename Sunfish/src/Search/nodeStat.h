/*
 * nodeStat.h
 *
 *  Created on: 2013/01/13
 *      Author: ryosuke
 */

#ifndef NODESTAT_H_
#define NODESTAT_H_ 

namespace Search {
	class NodeStat {
	private:
		enum {
			NULL_MOVE = 0x0001,
			RECAPTURE = 0x0002,

			DEF_STAT = NULL_MOVE | RECAPTURE
		};

		unsigned stat;

		void set(unsigned flag) {
			stat |= flag;
		}

		void unset(unsigned flag) {
			stat &= ~flag;
		}

	public:
		NodeStat() : stat(DEF_STAT) {
		}

		NodeStat(unsigned stat) : stat(stat) {
		}

		NodeStat(const NodeStat& nodeStat) : stat(nodeStat.stat) {
		}

		NodeStat& setNullMove() {
			set(NULL_MOVE);
			return *this;
		}

		NodeStat& unsetNullMove() {
			unset(NULL_MOVE);
			return *this;
		}

		bool isNullMove() const {
			return stat & NULL_MOVE;
		}

		NodeStat& setRecapture() {
			set(RECAPTURE);
			return *this;
		}

		NodeStat& unsetRecapture() {
			unset(RECAPTURE);
			return *this;
		}

		bool isRecapture() const {
			return stat & RECAPTURE;
		}
	};
}

#endif // NODESTAT_H_ 
