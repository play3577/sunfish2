/*
 * position.cpp
 *
 *  Created on: 2012/05/11
 *      Author: ryosuke
 */

#include "position.h"

namespace Shogi {
	const unsigned Position::nextTable[] = {
			0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0,
			0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0,
			0xD0, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x31, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0,
			0xD0, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0,
			0xD0, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x51, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0,
			0xD0, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x61, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0,
			0xD0, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x71, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0,
			0xD0, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x81, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0,
			0xD0, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x91, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0,
			0xD0, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0xA1, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0,
			0xD0, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0,
			0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0,
			0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0, 0xD0,
	};
}
