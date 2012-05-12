/*
 * debug.h
 *
 *  Created on: 2012/05/12
 *      Author: ryosuke
 */

#ifndef DEBUG_H_
#define DEBUG_H_

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
