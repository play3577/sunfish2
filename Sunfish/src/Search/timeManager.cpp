/*
 * timeManager.cpp
 *
 *  Created on: 2013/03/02
 *      Author: ryosuke
 */

#include "timeManager.h"

namespace Search {
	using namespace Evaluates;

	void TimeManager::init() {
		stack[depth].first = Value::MIN;
		stack[depth].second = Value::MIN;
	}

	bool TimeManager::closure(double elapsed) {
		if (depth == 0) {
			return 0;
		}

		int easyDepth = depth - 5;
		if (easyDepth < 0) {
			easyDepth = 0;
		}
		Value easy1st = stack[easyDepth].first;
		Value easy2nd = stack[easyDepth].second;
		Value easyDiff = easy1st - easy2nd;
		Value prev1st = stack[depth-1].first;
		Value first = stack[depth].first;
		Value second = stack[depth].second;
		Value diff = first - second;

		if (diff >= 400) {
			if (easyDiff >= 600) {
				if (first >= easy1st - 200 &&
						first <= easy1st + 400 &&
						Value::abs(first) <= 500) {
					return true;
				}
			} else if (easyDiff >= 400) {
				if (first >= easy1st - 100 &&
						first <= easy1st + 200 &&
						Value::abs(first) <= 200) {
					return true;
				}
			}
		}

		double baseSeconds = limitEnable ?  limitSeconds / 10.0 : 5;
		if (elapsed >= baseSeconds) {
			if (first >= prev1st && first <= prev1st + 150) {
				return true;
			} else if (diff >= 300 && first >= prev1st - 100) {
				return true;
			}
		}

		return false;
	}
}
