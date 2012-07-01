/*
 * debug.h
 *
 *  Created on: 2012/05/12
 *      Author: ryosuke
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <iostream>
#include "../Search/tree.h"

#ifdef NDEBUG
#define DEBUG_PRINT_LINE		do { } while (false)
#else
#define DEBUG_PRINT_LINE		do { std::cout << __FILE__ << "(" << __LINE__ << ")\n"; } while (false)
#endif

namespace Tools{
	class Debug {
	private:
		Debug() {
		}

	public:
		static void Print( const char* str, ... );

		static bool PositionOk(const Shogi::Position pos);

		static bool GeneratorRandomTest(const char* filename);

		static bool GeneratorTest(const char* filename);

		static void TreeSearch(Search::Tree& tree);

		static bool TreeTest(const char* filename);
	};
}

#endif /* DEBUG_H_ */
