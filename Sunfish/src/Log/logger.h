/*
 * log.h
 * 
 *  Created on: 2012/07/29
 *      Author: Kubo Ryosuke
 */

#include <iostream>

namespace Log {
	class Logger {
	private:
		static std::ostream* pout;

		Logger() {
		}

	public:
		static void setStream(std::ostream& o) {
			pout = &o;
		}

		static std::ostream& out() {
			if (pout != NULL) {
				return *pout;
			} else {
				return std::cout;
			}
		}
	};
}
