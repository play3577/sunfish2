/*
 * random.h
 *
 *  Created on: 2013/02/06
 *      Author: ryosuke
 */

#ifndef RANDOM_H_
#define RANDOM_H_

#include <boost/random.hpp>

namespace Util {
	class Random {
	private:
		static const unsigned MASK = 0xffff;

		boost::mt19937 rgen;
		boost::uniform_int<> dst;
		boost::variate_generator<boost::mt19937&, boost::uniform_int<> > r;
		unsigned bits;
		unsigned bnum;

	public:
		Random() :
				rgen(static_cast<unsigned>(time(NULL))),
				dst(0, MASK),
				r(rgen, dst) {
			bnum = 0;
		}

		unsigned getInt32() {
			return ((unsigned)r() << 16) ^ (unsigned)r();
		}

		Util::uint64 getInt64() {
			return ((Util::uint64)r() << 48) ^ ((Util::uint64)r() << 32)
					^ ((Util::uint64)r() << 16) ^ (Util::uint64)r();
		}

		unsigned getBit() {
			if (bnum == 0) {
				bits = getInt32();
				bnum = 32;
			}
			bnum--;
			unsigned ret = bits & 0x01;
			bits >>= 1;
			return ret;
		}
	};
}

#endif // RANDOM_H_
