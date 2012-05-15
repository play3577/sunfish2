/*
 * directionBit.h
 *
 *  Created on: 2012/05/16
 *      Author: ryosuke
 */

#ifndef DIRECTIONBIT_H_
#define DIRECTIONBIT_H_

namespace Shogi {
	class DirectionBit {
	private:
		static const unsigned int firstBit[];
		static const unsigned int direction[];
		unsigned bits;

	public:
		enum {
			NON              = 0x0000,
			SHORT_RIGHT_UP   = 0x0001,
			SHORT_UP         = 0x0002,
			SHORT_LEFT_UP    = 0x0004,
			SHORT_RIGHT      = 0x0008,
			SHORT_LEFT       = 0x0010,
			SHORT_RIGHT_DOWN = 0x0020,
			SHORT_DOWN       = 0x0040,
			SHORT_LEFT_DOWN  = 0x0080,
			LONG_RIGHT_UP    = 0x0100,
			LONG_UP          = 0x0200,
			LONG_LEFT_UP     = 0x0400,
			LONG_RIGHT       = 0x0800,
			LONG_LEFT        = 0x1000,
			LONG_RIGHT_DOWN  = 0x2000,
			LONG_DOWN        = 0x4000,
			LONG_LEFT_DOWN   = 0x8000,
		};

		DirectionBit() : bits(0) {
		}

		DirectionBit(unsigned bits) : bits(bits) {
		}

		DirectionBit(const DirectionBit& bit) : bits(bit.bits) {
		}

		unsigned getInteger() const {
			return bits;
		}

		void set(unsigned bits) {
			this->bits = bits;
		}

		void set(const DirectionBit& bit) {
			bits = bit.bits;
		}

		void add(DirectionBit bit) {
			bits |= bit.bits;
		}

		void remove(DirectionBit bit) {
			bits &= ~bit.bits;
		}

		DirectionBit pop() {
			unsigned mask = bits-1;
			unsigned temp = bits;
			bits = bits & mask;
			return DirectionBit(temp & ~mask);
		}

		Direction toDirection() const {
			int b;
			if( bits == 0U ){ b = 0; }
			else if( ( b = firstBit[ bits     &0xff] ) != 0 ){ }
			else if( ( b = firstBit[(bits>> 8)&0xff] ) != 0 ){ b += 8; }
			else if( ( b = firstBit[(bits>>16)&0xff] ) != 0 ){ b += 16; }
			else {     b = firstBit[(bits>>24)     ] + 24; }
			return Direction(direction[b]);
		}
	};
}

#endif /* DIRECTIONBIT_H_ */
