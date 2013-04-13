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
		memset(razor_suc, 0, sizeof(razor_suc));
		memset(stat_suc, 0, sizeof(stat_suc));
		memset(count_suc, 0, sizeof(count_suc));
		memset(fut_err, 0, sizeof(fut_err));
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

	void PruningExpr::success1(int dep, int fut, int count) {
		ins.fut_suc[depth(dep)][value(fut)]++;
		ins.count_suc[depth(dep)][value(count)]++;
	}

	void PruningExpr::error1(int dep, int fut, int count) {
		ins.fut_err[depth(dep)][value(fut)]++;
		ins.count_err[depth(dep)][value(count)]++;
	}

	void PruningExpr::success2(int dep,
			int razor, int stat) {
		ins.razor_suc[depth(dep)][value(razor)]++;
		ins.stat_suc[depth(dep)][value(stat)]++;
	}

	void PruningExpr::error2(int dep,
			int razor, int stat) {
		ins.razor_err[depth(dep)][value(razor)]++;
		ins.stat_err[depth(dep)][value(stat)]++;
	}

	void PruningExpr::print(const char* name,
			Util::uint64 result[][MAX_VAL-MIN_VAL+1]) {
		Log::expr << name << '\n';
		Log::expr << ',';
		for (int val = MIN_VAL; val <= MAX_VAL; val++) {
			Log::expr << (val*10) << ',';
		}
		Log::expr << '\n';
		for (int dep = 0; dep <= MAX_DEP; dep++) {
			Log::expr << dep << ',';
			for (int val = MIN_VAL; val <= MAX_VAL; val++) {
				Log::expr << result[dep][val-MIN_VAL] << ',';
			}
			Log::expr << '\n';
		}
		Log::expr << '\n';
	}

	void PruningExpr::print() {
		print("fut suc", ins.fut_suc);
		print("fut err", ins.fut_err);
		print("razor suc", ins.razor_suc);
		print("razor err", ins.razor_err);
		print("stat suc", ins.stat_suc);
		print("stat err", ins.stat_err);
		print("count suc", ins.count_suc);
		print("count err", ins.count_err);
	}
}

#endif // PRUN_EXPR
