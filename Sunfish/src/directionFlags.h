/*
 * directionBit.h
 *
 *  Created on: 2012/05/16
 *      Author: ryosuke
 */

#ifndef DIRECTIONFLAGS_H_
#define DIRECTIONFLAGS_H_

#include "directionAndRange.h"

namespace Shogi {
	class DirectionFlags {
	private:
		static const unsigned int firstBit[];
		static const unsigned int direction[];
		static const unsigned int _dir2bit[];
		static const unsigned int* dir2bit;
		unsigned bits;

		unsigned longRangeShift(unsigned bits) {
			return bits << LONG_SHIFT;
		}

	public:
		enum {
			NON               = 0x000000,
			SHORT_RIGHT_UP    = 0x000001,
			SHORT_UP          = 0x000002,
			SHORT_LEFT_UP     = 0x000004,
			SHORT_RIGHT       = 0x000008,
			SHORT_LEFT        = 0x000010,
			SHORT_RIGHT_DOWN  = 0x000020,
			SHORT_DOWN        = 0x000040,
			SHORT_LEFT_DOWN   = 0x000080,
			SHORT_RIGHT_UP2   = 0x000100,
			SHORT_LEFT_UP2    = 0x000200,
			SHORT_RIGHT_DOWN2 = 0x000400,
			SHORT_LEFT_DOWN2  = 0x000800,
			LONG_RIGHT_UP     = 0x001000,
			LONG_UP           = 0x002000,
			LONG_LEFT_UP      = 0x004000,
			LONG_RIGHT        = 0x008000,
			LONG_LEFT         = 0x010000,
			LONG_RIGHT_DOWN   = 0x020000,
			LONG_DOWN         = 0x040000,
			LONG_LEFT_DOWN    = 0x080000,
			WALL              = 0x100000,

			LONG_SHIFT        = 12,
			SHORT_MASK        = 0x000fff,
			LONG_MASK         = 0x0ff000,

			PIECE = SHORT_UP,
			LANCE = LONG_UP,
			KNIGHT = SHORT_RIGHT_UP2 | SHORT_LEFT_UP2,
			SILVER = SHORT_RIGHT_UP | SHORT_UP | SHORT_LEFT_UP | SHORT_RIGHT_DOWN | SHORT_LEFT_DOWN,
			GOLD = SHORT_RIGHT_UP | SHORT_UP | SHORT_LEFT_UP | SHORT_RIGHT | SHORT_LEFT | SHORT_DOWN,
			BISHOP = LONG_RIGHT_UP | LONG_LEFT_UP | LONG_RIGHT_DOWN | LONG_LEFT_DOWN,
			ROOK = LONG_UP | LONG_LEFT | LONG_RIGHT | LONG_DOWN,
			KING = SHORT_RIGHT_UP | SHORT_UP | SHORT_LEFT_UP | SHORT_RIGHT | SHORT_LEFT | SHORT_RIGHT_DOWN | SHORT_DOWN | SHORT_LEFT_DOWN,
			HORSE = LONG_RIGHT_UP | LONG_LEFT_UP | LONG_RIGHT_DOWN | LONG_LEFT_DOWN | SHORT_UP | SHORT_RIGHT | SHORT_LEFT | SHORT_DOWN,
			DRAGON = LONG_UP | LONG_LEFT | LONG_RIGHT | LONG_DOWN | SHORT_RIGHT_UP  | SHORT_LEFT_UP | SHORT_RIGHT_DOWN | SHORT_LEFT_DOWN,

			BPIECE = PIECE,
			BLANCE = LANCE,
			BKNIGHT = KNIGHT,
			BSILVER = SILVER,
			BGOLD = GOLD,
			BBISHOP = BISHOP,
			BROOK = ROOK,
			BKING = KING,
			BHORSE = HORSE,
			BDRAGON = DRAGON,

			WPIECE = SHORT_DOWN,
			WLANCE = LONG_DOWN,
			WKNIGHT = SHORT_RIGHT_DOWN2 | SHORT_LEFT_DOWN2,
			WSILVER = SHORT_RIGHT_DOWN | SHORT_DOWN | SHORT_LEFT_DOWN | SHORT_RIGHT_UP | SHORT_LEFT_UP,
			WGOLD = SHORT_RIGHT_DOWN | SHORT_DOWN | SHORT_LEFT_DOWN | SHORT_RIGHT | SHORT_LEFT | SHORT_UP,
			WBISHOP = LONG_RIGHT_DOWN | LONG_LEFT_DOWN | LONG_RIGHT_UP | LONG_LEFT_UP,
			WROOK = LONG_DOWN | LONG_LEFT | LONG_RIGHT | LONG_UP,
			WKING = SHORT_RIGHT_DOWN | SHORT_DOWN | SHORT_LEFT_DOWN | SHORT_RIGHT | SHORT_LEFT | SHORT_RIGHT_UP | SHORT_UP | SHORT_LEFT_UP,
			WHORSE = LONG_RIGHT_DOWN | LONG_LEFT_DOWN | LONG_RIGHT_UP | LONG_LEFT_UP | SHORT_DOWN | SHORT_RIGHT | SHORT_LEFT | SHORT_UP,
			WDRAGON = LONG_DOWN | LONG_LEFT | LONG_RIGHT | LONG_UP | SHORT_RIGHT_DOWN  | SHORT_LEFT_DOWN | SHORT_RIGHT_UP | SHORT_LEFT_UP,
		};

		DirectionFlags() : bits(0) {
		}

		DirectionFlags(unsigned bits) : bits(bits) {
		}

		DirectionFlags(const DirectionFlags& bit) : bits(bit.bits) {
		}

		DirectionFlags(const Direction& dir) {
			set(dir);
		}

		DirectionFlags(const DirectionAndRange& dir) {
			set(dir);
		}

		unsigned getInteger() const {
			return bits;
		}

		void set(unsigned bits) {
			this->bits = bits;
		}

		void set(const Direction& dir) {
			bits = dir2bit[(int)dir];
		}

		void set(const DirectionAndRange& dir) {
			if (dir.isLongRange()) {
				bits = longRangeShift(dir2bit[(int)dir]);
			} else {
				bits = dir2bit[(int)dir];
			}
		}

		void set(const DirectionFlags& bit) {
			bits = bit.bits;
		}

		void add(const DirectionFlags& bit) {
			bits |= bit.bits;
		}

		void add(const Direction& dir) {
			bits |= DirectionFlags(dir).bits;
		}

		void add(const DirectionAndRange& dir) {
			bits |= DirectionFlags(dir).bits;
		}

		void remove(const DirectionFlags& bit) {
			bits &= ~DirectionFlags(bit).bits;
		}

		void remove(const Direction& dir) {
			bits &= ~DirectionFlags(dir).bits;
		}

		void remove(const DirectionAndRange& dir) {
			bits &= ~DirectionFlags(dir).bits;
		}

		bool isZero() const {
			return bits == NON;
		}

		bool isNonZero() const {
			return !isZero();
		}

		bool isWall() const {
			return bits == WALL;
		}

		bool isLongRange() const {
			return bits & LONG_MASK;
		}

		bool isShortRange() const {
			return bits & SHORT_MASK;
		}

		DirectionFlags getLongRangeOnly() const {
			return DirectionFlags(bits & LONG_MASK);
		}

		DirectionFlags getShortRangeOnly() const {
			return DirectionFlags(bits & SHORT_MASK);
		}

		DirectionFlags pop() {
			unsigned mask = bits-1;
			unsigned temp = bits;
			bits = bits & mask;
			return DirectionFlags(temp & ~mask);
		}

		Direction toDirection() {
			int b;
			if( bits == 0U ){ b = 0; }
			else if( ( b = firstBit[ bits     &0xff] ) != 0 ){ }
			else if( ( b = firstBit[(bits>> 8)&0xff] ) != 0 ){ b += 8; }
			else if( ( b = firstBit[(bits>>16)&0xff] ) != 0 ){ b += 16; }
			else {     b = firstBit[(bits>>24)     ] + 24; }
			return Direction(direction[b] );
		}

		DirectionAndRange toDirectionAndRange() {
			return DirectionAndRange(toDirection(), !isShortRange() );
		}
	};
}

#endif /* DIRECTIONFLAGS_H_ */
