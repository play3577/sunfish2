/*
 * aspWindow.h
 *
 *  Created on: 2012/08/19
 *      Author: ryosuke
 */

#ifndef ASP_WINDOW_H_
#define ASP_WINDOW_H_

#include "../Evaluate/value.h"

namespace Search {
	using namespace Evaluate;

	extern const int aspWind[2];
	extern const int ASP_MAX;

	template<int sign>
	class AspWindow {
	private:
		int aspPhase;
		Value aspValue;

	public:
		AspWindow(int value) {
			init(value);
		}

		AspWindow() {
			init();
		}

		void init(int value) {
			aspValue = value;
			aspPhase = 0;
		}

		void init() {
			aspPhase = ASP_MAX;
		}

		bool next() {
			if (aspPhase == ASP_MAX) {
				return false;
			} else {
				aspPhase++;
				return true;
			}
		}

		Value getValue() const {
			if (aspPhase == ASP_MAX) {
				return Value::MAX * sign;
			} else {
				return aspValue + aspWind[aspPhase] * sign;
			}
		}

		operator int() const {
			return (int)getValue();
		}

		AspWindow<sign>& operator=(AspWindow<sign> asp) {
			aspPhase = asp.aspPhase;
			aspValue = asp.aspValue;
			return *this;
		}
	};
}

#endif // ASP_WINDOW_H_
