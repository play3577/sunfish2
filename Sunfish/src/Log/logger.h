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
		std::ostream* pout;

	public:
		Logger() {
			pout = &std::cout;
		}

		void setStream(std::ostream& o) {
			pout = &o;
		}

		std::ostream& out() {
			if (pout != NULL) {
				return *pout;
			} else {
				return std::cout;
			}
		}

		template <class T>
		Logger& operator<<(T t) {
			out() << t;
			return *this;
		}
	};

	extern Logger error;
	extern Logger warning;
	extern Logger message;
	extern Logger debug;
}
