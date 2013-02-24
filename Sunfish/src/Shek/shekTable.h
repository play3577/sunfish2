/*
 * shekTable.h
 *
 *  Created on: 2012/11/03
 *      Author: ryosuke
 */

#ifndef SHEKTABLE_H_
#define SHEKTABLE_H_

#include "shekEntity.h"
#include "../Shogi/position.h"
#include "../Records/hashStack.h"

namespace Shek {
	class ShekTable : public Table::BaseTable<ShekEntity> {
	public:
		ShekTable() : 
				Table::BaseTable<ShekEntity>() {
		}

		ShekTable(unsigned bits) : 
				Table::BaseTable<ShekEntity>(bits) {
		}

		void set(const Records::HashStack& hashStack) {
			for (int i = 0; i < hashStack.size; i++) {
				const Records::HashData& hashData = hashStack.stack[i];
				_getEntity(hashData.boardHash)
					.set(hashData.handSet, hashData.blackTurn);
			}
		}

		void unset(const Records::HashStack& hashStack) {
			for (int i = 0; i < hashStack.size; i++) {
				const Records::HashData& hashData = hashStack.stack[i];
				_getEntity(hashData.boardHash).unset();
			}
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
			getEntity(pos.getBoardHash()).debugPrint(
					HandSet(hand), pos.isBlackTurn());
		}
	};
}

#endif // SHEKTABLE_H_
