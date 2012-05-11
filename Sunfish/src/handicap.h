/*
 * handicap.h
 *
 *  Created on: 2012/05/11
 *      Author: ryosuke
 */

#ifndef HANDICAP_H_
#define HANDICAP_H_

namespace Shogi {
	enum Handicap {
		EMPTY = 0,
		EVEN,
		LANCE,
		BISHOP,
		ROOK,
		ROOK_LANCE,
		HANDICAP_2PIECES,
		HANDICAP_4PIECES,
		HANDICAP_6PIECES,
		HANDICAP_8PIECES,
	};
}

#endif /* HANDICAP_H_ */
