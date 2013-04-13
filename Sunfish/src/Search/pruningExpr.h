/*
 * pruningExpr.h
 *
 *  Created on: 2013/04/13
 *      Author: ryosuke
 */

#ifndef PRUNINGEXPR_H_
#define PRUNINGEXPR_H_

#ifdef PRUN_EXPR

#include "../Util/int.h"

namespace Search {
	class PruningExpr {
	private:
		static PruningExpr ins;
		static const int MIN_VAL = 0;
		static const int MAX_VAL = 100;
		static const int MAX_DEP = 16;
		Util::uint64 fut_suc[MAX_DEP+1][MAX_VAL-MIN_VAL+1];
		Util::uint64 razor_suc[MAX_DEP+1][MAX_VAL-MIN_VAL+1];
		Util::uint64 stat_suc[MAX_DEP+1][MAX_VAL-MIN_VAL+1];
		Util::uint64 count_suc[MAX_DEP+1][MAX_VAL-MIN_VAL+1];
		Util::uint64 fut_err[MAX_DEP+1][MAX_VAL-MIN_VAL+1];
		Util::uint64 razor_err[MAX_DEP+1][MAX_VAL-MIN_VAL+1];
		Util::uint64 stat_err[MAX_DEP+1][MAX_VAL-MIN_VAL+1];
		Util::uint64 count_err[MAX_DEP+1][MAX_VAL-MIN_VAL+1];
		PruningExpr();

		static int depth(int dep);

		static int value(int val);

		static void print(const char* name,
				Util::uint64 result[][MAX_VAL-MIN_VAL+1]);

	public:
		static void success1(int dep, int fut, int count);

		static void error1(int dep, int fut, int count);

		static void success2(int dep, int razor, int stat);

		static void error2(int dep, int razor, int stat);

		static void print();

	};
}

#endif // PRUN_EXPR

#endif // PRUNINGEXPR_H_
