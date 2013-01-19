/*
 * shekTable.h
 *
 *  Created on: 2012/11/03
 *      Author: ryosuke
 */

#include "shekEntity.h"

namespace Shek {
	class ShekTable : public Table::BaseTable<ShekEntity> {
	public:
		ShekTable(unsigned bits = DEF_BITS) : 
				Table::BaseTable<ShekEntity>(bits) {
		}

		void set(const Shogi::Position& pos) {
			const Shogi::Hand& hand = pos.isBlackTurn()
					? pos.getBlackHand() : pos.getBlackHand();
			_getEntity(pos.getBoardHash())
					.set(HandSet(hand), pos.isBlackTurn());
		}

		void unset(const Shogi::Position& pos) {
			_getEntity(pos.getBoardHash()).unset();
		}

		ShekStat check(const Shogi::Position& pos) const {
			const Shogi::Hand& hand = pos.isBlackTurn()
					? pos.getBlackHand() : pos.getBlackHand();
			return getEntity(pos.getBoardHash())
					.check(HandSet(hand), pos.isBlackTurn());
		}

		unsigned getCount(const Shogi::Position& pos) const {
			return getEntity(pos.getBoardHash()).getCount();
		}

		void debugPrint(const Shogi::Position& pos) const {
			const Shogi::Hand& hand = pos.isBlackTurn()
					? pos.getBlackHand() : pos.getBlackHand();
			getEntity(pos.getBoardHash()).debugPrint(HandSet(hand));
		}
	};
}
