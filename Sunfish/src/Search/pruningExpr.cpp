/*
 * pruningExpr.cpp
 *
 *  Created on: 2013/04/13
 *      Author: ryosuke
 */

#include "pruningExpr.h"
#include "../Log/logger.h"
#include <cstring>
#include <sstream>

#ifdef PRUN_EXPR

namespace Search {
	PruningExpr PruningExpr::ins;

	PruningExpr::PruningExpr() {
		memset(fut_suc, 0, sizeof(fut_suc));
		memset(ext_fut_suc, 0, sizeof(ext_fut_suc));
		memset(razor_suc, 0, sizeof(razor_suc));
		memset(stat_suc, 0, sizeof(stat_suc));
		memset(count_suc, 0, sizeof(count_suc));
		memset(fut_err, 0, sizeof(fut_err));
		memset(ext_fut_err, 0, sizeof(ext_fut_err));
		memset(razor_err, 0, sizeof(razor_err));
		memset(stat_err, 0, sizeof(stat_err));
		memset(count_err, 0, sizeof(count_err));
	}

	int PruningExpr::depth(int dep) {
		dep /= 4;
		return dep < MAX_DEP ? dep : MAX_DEP;
	}

	int PruningExpr::value(int val) {
		val /= 10;
		return val <= MIN_VAL ? MIN_VAL :
				(val < MAX_VAL ? val : MAX_VAL);
	}

	void PruningExpr::success1(int dep, bool isFut, int fut,
			bool isExtFut, int extFut, bool isCount, int count) {
		if (isFut) {
			ins.fut_suc[depth(dep)][value(fut)]++;
		}
		if (isExtFut) {
			ins.ext_fut_suc[depth(dep)][value(extFut)]++;
		}
		if (isCount) {
			ins.count_suc[depth(dep)][value(count)]++;
		}
	}

	void PruningExpr::error1(int dep, bool isFut, int fut,
			bool isExtFut, int extFut, bool isCount, int count) {
		if (isFut) {
			ins.fut_err[depth(dep)][value(fut)]++;
		}
		if (isExtFut) {
			ins.ext_fut_err[depth(dep)][value(extFut)]++;
		}
		if (isCount) {
			ins.count_err[depth(dep)][value(count)]++;
		}
	}

	void PruningExpr::success2(int dep, bool isRazor, int razor) {
		if (isRazor) {
			ins.razor_suc[depth(dep)][value(razor)]++;
		}
	}

	void PruningExpr::error2(int dep, bool isRazor, int razor) {
		if (isRazor) {
			ins.razor_err[depth(dep)][value(razor)]++;
		}
	}

	void PruningExpr::success3(int dep, bool isStat, int stat) {
		if (isStat) {
			ins.stat_suc[depth(dep)][value(stat)]++;
		}
	}

	void PruningExpr::error3(int dep, bool isStat, int stat) {
		if (isStat) {
			ins.stat_err[depth(dep)][value(stat)]++;
		}
	}

	void PruningExpr::print(const char* name,
			Util::uint64 suc[][MAX_VAL-MIN_VAL+1],
			Util::uint64 err[][MAX_VAL-MIN_VAL+1]
			) {
		Log::expr << name << '\n';
		Log::expr << ',';
		for (int val = MIN_VAL; val <= MAX_VAL; val++) {
			Log::expr << (val*10) << ',';
		}
		Log::expr << '\n';
		for (int dep = 0; dep <= MAX_DEP; dep++) {
			Log::expr << dep << ',';
			for (int val = MIN_VAL; val <= MAX_VAL; val++) {
				int s = suc[dep][val-MIN_VAL];
				int e = err[dep][val-MIN_VAL];
				double rate = s / (double)(s + e);
				Log::expr << rate << ',';
			}
			Log::expr << '\n';
		}
		Log::expr << '\n';
	}

	void PruningExpr::print() {
		print("fut", ins.fut_suc, ins.fut_err);
		print("extFut", ins.ext_fut_suc, ins.ext_fut_err);
		print("razor", ins.razor_suc, ins.razor_err);
		print("stat", ins.stat_suc, ins.stat_err);
		print("count", ins.count_suc, ins.count_err);
	}
}

#endif // PRUN_EXPR
