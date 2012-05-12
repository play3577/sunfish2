/*
 * sunfish.cpp
 *
 *  Created on: 2012/05/08
 *      Author: ryosuke
 */

#include <boost/thread.hpp>
#include <iostream>
#include "sunfish.h"
#include "board.h"

int main(int argc, char* argv[]) {
	std::cout << SUNFISH_NAME << ' ';
	std::cout << SUNFISH_VERSION << '\n';
	std::cout << SUNFISH_COPYRIGHT << '\n';

	Shogi::Board board(Shogi::EVEN);
	std::cout << board.toString();

	boost::xtime xt;
	boost::xtime_get(&xt, boost::TIME_UTC);
	for (int i = 0; i < 100; i++) {
		std::cout << "test:" << i << '\n';
		xt.nsec += 500 * 1000;
		boost::thread::sleep(xt);
	}

	return 0;
}
