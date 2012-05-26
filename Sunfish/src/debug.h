/*
 * debug.h
 *
 *  Created on: 2012/05/12
 *      Author: ryosuke
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <iostream>

#ifdef NDEBUG
#define DEBUG_PRINT_LINE		do { } while (false)
#else
#define DEBUG_PRINT_LINE		do { std::cout << __LINE__ << '\n'; } while (false)
#endif

namespace Tools{
	class Debug {
	private:
		Debug() {
		}

	public:
		static void Print( const char* str, ... );
	};
}

#endif /* DEBUG_H_ */
