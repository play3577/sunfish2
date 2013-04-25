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
#include <cmath>

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

	int PruningExpr::record(int rec) {
		return rec <= MAX_REC ? rec : MAX_REC;
	}

	void PruningExpr::summarize(Util::uint64 suc[], Util::uint64 err[], int size,
			double& average, double& deviation, double& max, double& min) {
		average = 0.0;
		deviation = 0.0;
		max = 0.0;
		min = 1.0;
		int count = 0;
		for (int i = 0; i < size; i++) {
			Util::uint64 all = suc[i] + err[i];
			if (all >= 1000) {
				double rate = (double)suc[i] / all;
				if (rate > max) {
					max = rate;
				}
				if (rate < min) {
					min = rate;
				}
				average += rate;
				deviation += rate * rate;
				count++;
			}
		}
		if (count >= 2) {
			average /= count;
			deviation /= count;
			deviation = sqrt(deviation - average * average) * count / (count-1);
		} else if (count == 1) {
			deviation = 0.0;
		}
	}

	void PruningExpr::success1(int dep, int rec, bool isFut, int fut,
			bool isExtFut, int extFut, bool isCount, int count) {
		if (isFut) {
			ins.fut_suc[depth(dep)][value(fut)][record(rec)]++;
		}
		if (isExtFut) {
			ins.ext_fut_suc[depth(dep)][value(extFut)][record(rec)]++;
		}
		if (isCount) {
			ins.count_suc[depth(dep)][value(count)][record(rec)]++;
		}
	}

	void PruningExpr::error1(int dep, int rec, bool isFut, int fut,
			bool isExtFut, int extFut, bool isCount, int count) {
		if (isFut) {
			ins.fut_err[depth(dep)][value(fut)][record(rec)]++;
		}
		if (isExtFut) {
			ins.ext_fut_err[depth(dep)][value(extFut)][record(rec)]++;
		}
		if (isCount) {
			ins.count_err[depth(dep)][value(count)][record(rec)]++;
		}
	}

	void PruningExpr::success2(int dep, int rec, bool isRazor, int razor) {
		if (isRazor) {
			ins.razor_suc[depth(dep)][value(razor)][record(rec)]++;
		}
	}

	void PruningExpr::error2(int dep, int rec, bool isRazor, int razor) {
		if (isRazor) {
			ins.razor_err[depth(dep)][value(razor)][record(rec)]++;
		}
	}

	void PruningExpr::success3(int dep, int rec, bool isStat, int stat, bool isNull) {
		if (isStat) {
			ins.stat_suc[depth(dep)][value(stat)][record(rec)]++;
		}
		if (isNull) {
			ins.null_suc[depth(dep)][record(rec)]++;
		}
	}

	void PruningExpr::error3(int dep, int rec, bool isStat, int stat, bool isNull) {
		if (isStat) {
			ins.stat_err[depth(dep)][value(stat)][record(rec)]++;
		}
		if (isNull) {
			ins.null_err[depth(dep)][record(rec)]++;
		}
	}

	void PruningExpr::print(const char* name,
			Util::uint64 suc[][MAX_VAL-MIN_VAL+1][MAX_REC+1],
			Util::uint64 err[][MAX_VAL-MIN_VAL+1][MAX_REC+1]
			) {
		Log::expr << name << '\n';
		for (int val = MIN_VAL; val <= MAX_VAL; val++) {
			Log::expr << ',' << (val*10) << ',';
		}
		Log::expr << '\n';
		for (int val = MIN_VAL; val <= MAX_VAL; val++) {
			Log::expr << ",ave,dev";
		}
		Log::expr << '\n';
		for (int val = MIN_VAL; val <= MAX_VAL; val++) {
			Log::expr << ",min,max";
		}
		Log::expr << '\n';
		for (int dep = 0; dep <= MAX_DEP; dep++) {
			double average, deviation, max, min;
			std::ostringstream line1, line2;
			for (int val = MIN_VAL; val <= MAX_VAL; val++) {
				summarize(suc[dep][val], err[dep][val], MAX_REC+1, average, deviation, max, min);
				line1 << ',' << average << ',' << deviation;
				line2 << ',' << min << ',' << max;
			}
			Log::expr << dep;
			Log::expr << line1.str() << '\n';
			Log::expr << line2.str() << '\n';
		}
		Log::expr << '\n';
	}

	void PruningExpr::print(const char* name,
			Util::uint64 suc[][MAX_REC+1],
			Util::uint64 err[][MAX_REC+1]) {
		Log::expr << name << ",ave,dev,min,max\n";
		for (int dep = 0; dep <= MAX_DEP; dep++) {
			double average, deviation, max, min;
			summarize(suc[dep], err[dep], MAX_REC+1, average, deviation, max, min);
			Log::expr << dep << ',' << average << ',' << deviation << ',' << min << ',' << max << '\n';
		}
		Log::expr << '\n';
	}

	void PruningExpr::print() {
		print("fut", ins.fut_suc, ins.fut_err);
		print("extFut", ins.ext_fut_suc, ins.ext_fut_err);
		print("razor", ins.razor_suc, ins.razor_err);
		print("stat", ins.stat_suc, ins.stat_err);
		print("count", ins.count_suc, ins.count_err);
		print("null", ins.null_suc, ins.null_err);
	}
}

#endif // PRUN_EXPR
