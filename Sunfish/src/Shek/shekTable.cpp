/*
 * shekTable.cpp
 *
 *  Created on: 2012/11/03
 *      Author: ryosuke
 */

#include "shekTable.h"

namespace Shek {
	using namespace Records;

	void ShekTable::set(const HashStack& hashStack) {
		for (int i = 0; i < hashStack.size; i++) {
			const HashData& hashData = hashStack.stack[i];
			_getEntity(hashData.boardHash)
				.set(hashData.handSet, hashData.blackTurn);
		}
	}

	void ShekTable::unset(const HashStack& hashStack) {
		for (int i = 0; i < hashStack.size; i++) {
			const HashData& hashData = hashStack.stack[i];
			_getEntity(hashData.boardHash).unset();
		}
	}
}
