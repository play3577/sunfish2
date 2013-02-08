/*
 * learn.cpp
 *
 *  Created on: 2013/02/02
 *      Author: ryosuke
 */

#include "learn.h"
#include "pvReader.h"
#include "../Log/logger.h"
#include "../Csa/csaReader.h"
#include "../Search/searcher.h"
#include "../Shogi/moveGenerator.h"
//#include <boost/filesystem.hpp>
//#include <boost/foreach.hpp>
#include "../Util/fileList.h"

#ifndef NLEARN

namespace Learns {
	const char* Learn::DEFAULT_CONFIG_FILE = "lconf";

	using namespace Records;
	using namespace Csa;
	using namespace Shogi;
	using namespace Search;
	using namespace Util;
	using namespace Evaluates;

	void Learn::execute() {
		// load settings
		if (!config.read(configFilename)) {
			return;
		}
		Log::message << config.toString();

		// learning
		for (int step = 1; step <= config.getSteps(); step++) {
			analyzeAllFiles();

			Gradient* pg = new Gradient();
			generateGradient(*pg);
			pg->symAdd();

			adjustParam(*pg);
			pparam->symCopy();
			pparam->write("evdata");

			delete pg;
		}
	}

	void Learn::analyzeAllFiles() {
		// PVの保存
		PvWriter writer("pv.tmp");

		// 棋譜ファイルの列挙
		/* TODO
		namespace fs = boost::filesystem;
		const fs::path path(config.getDir());
		BOOST_FOREACH(const fs::path& p, std::make_pair(
				fs::directory_iterator(path),
				fs::directory_iterator())) {
			if (!fs::is_directory(p)) {
				analyzeFile(p.c_str(), writer);
			}
		}
		*/
		FileList fileList;
		fileList.enumerate(config.getDir().c_str(), "csa");
		while (!fileList.isEnd()) {
			analyzeFile(fileList.pop().c_str(), writer);
		}
	}

	void Learn::analyzeFile(const char* path, PvWriter& writer) {
		Log::message << path << '\n';

		// 棋譜の読み込み
		Record record;
		CsaReader::read(path, record);

		// 探索エージェント
		Searcher searcher(*pparam);
		SearchConfig searchConfig = SearchConfig::getDefault();
		searchConfig.depth = config.getDepth();
		searchConfig.isLearning = true;
		searcher.setSearchConfig(searchConfig);

		// 初手から順に棋譜を見ていく。
		record.end();
		while (record.prev()) {
			// 棋譜の指し手
			Move recMove;
			record.getNextMove(recMove);
			Log::debug << recMove.toString() << ' ';
			SearchResult recResult;
			if (record.move(recMove)) { // 一手進める。
				searcher.init(record);
				searcher.search(recResult); // 探索
				record.prev(); // 一手戻す。
			} else {
				Log::error << "ERROR:" << __THIS__ << ":illegal move!!\n";
				Log::error << record.getPosition().toString();
				Log::error << recMove.toString() << '\n';
				exit(1);
			}

			// 詰みは除外
			if (recResult.value >= Value::MATE ||
					recResult.value <= -Value::MATE) {
				continue;
			}

			if (recResult.pv.size() == 0) {
				continue;
			}

			// 探索窓
			Value alpha = Value::safeSub(recResult.value, 256); // TODO: magic number
			Value beta = Value::safeAdd(recResult.value, 256); // TODO: magic number

			// その他の手
			MoveGenerator gen(record.getPosition());
			gen.generate();
			int rm = gen.removeUnstable(recMove); // 棋譜の手を除去
			if (rm >= 2) {
				Log::error << __THIS__ << ":" << rm << " moves were removed\n";
				Log::error << record.getPosition().toString();
				Log::error << recMove.toString() << '\n';
			} else if (rm == 0) {
				Log::warning << __THIS__ << ":" << ":no matching move..\n";
				Log::warning << record.getPosition().toString();
				Log::warning << recMove.toString() << '\n';
			}
			if (gen.getNumber() == 0) {
				// 棋譜の手以外に指し手がない場合
				continue;
			}

			// write
			writer.writePosition(record.getPosition());
			writer.writeMove(recMove);
			writer.writePv(recResult.pv);

			const Move* pmove;
			while ((pmove = gen.next()) != NULL) {
				SearchResult result;
				if (record.move(*pmove)) { // 一手進める。
					searcher.init(record);
					searcher.search(result, alpha, beta); // 探索
					record.prev(); // 一手戻す。
				} else {
					Log::error << "ERROR" << __THIS__ << ":illegal move!!\n";
					Log::error << record.getPosition().toString();
					Log::error << pmove->toString() << '\n';
					exit(1);
				}

				if (result.pv.size() == 0) {
					continue;
				}

				// write
				writer.writeMove(*pmove);
				writer.writePv(result.pv);
			}
		}
		Log::debug << '\n';
	}

	double Learn::loss(double x) {
		const double swin = 256; // TODO: magic number
		const double dt = swin / 7.0;
		if (x > swin) { x = swin; }
		else if (x < -swin) { x = -swin; }
		return 1.0 / (1.0 + exp(x / dt));
	}

	double Learn::dLoss(double x) {
		const double swin = 256; // TODO: magic number
		const double dt = swin / 7.0;
		double d;
		if (x >= swin || x <= -swin) {
			return 0.0;
		}
		x = exp(x / dt);
		d = x + 1.0;
		return x / (dt * d * d);
	}

	void Learn::generateGradient(Evaluates::Gradient& g) {
		// PVの読み込み
		PvReader reader("pv.tmp");

		// 局面評価
		Evaluate eval(*pparam);

		while (reader.nextFlag().is(PvIoFlag::POSITION)) {
			Position pos;
			reader.readPosition(pos);
			Record record(pos);

			bool black = pos.isBlackTurn();
			int sign = black ? 1 : -1;

			// 棋譜の指し手
			Move recMove;
			Pv recPv;
			reader.readMove(recMove);
			reader.readPv(recPv);

			Log::debug << pos.toString() << '\n';
			Log::debug << recMove.toString() << ' ';
			Log::debug << recPv.toString() << '\n';

			// PV の末端
			record.move(recMove);
			for (int i = 0; i < recPv.size(); i++) {
				record.move(recPv.get(i));
			}
			Value recValue = eval.getValue(record.getPosition()) * sign;
			for (int i = 0; i < recPv.size(); i++) {
				record.prev();
			}
			record.prev();

			// その他の手
			double dSum = 0.0;
			while (reader.nextFlag().is(PvIoFlag::MOVE)) {
				Move move;
				Pv pv;
				reader.readMove(move);
				reader.readPv(pv);

				Log::debug << move.toString() << ' ';
				Log::debug << pv.toString() << '\n';

				// PV の末端
				record.move(move);
				for (int i = 0; i < pv.size(); i++) {
					record.move(pv.get(i));
				}
				Value value = eval.getValue(record.getPosition()) * sign;
				// 勾配
				double d = dLoss(recValue - value);
				Feature::incValue(record.getPosition(), &g, -d);
				dSum += d;
				// 損失
				double l = loss(recValue - value);
				// TODO: 損失の足しこみ
				for (int i = 0; i < pv.size(); i++) {
					record.prev();
				}
				record.prev();
			}

			// PV の末端 (棋譜の指し手)
			record.move(recMove);
			for (int i = 0; i < recPv.size(); i++) {
				record.move(recPv.get(i));
			}
			Feature::incValue(record.getPosition(), &g, dSum);
			assert(dSum >= 0.0);
			for (int i = 0; i < recPv.size(); i++) {
				record.prev();
			}
			record.prev();
		}
	}

	void Learn::adjustParam(const Gradient& g) {
		// TODO: 駒割

		for (int index = 0; index < KPP_MAX; index++) {
			ValueS prev = pparam->getKPP(index);
			ValueF grad = g.getKPP(index);
			ValueS value = adjustValue(prev, grad);
			pparam->setKPP(index, value);
		}

		for (int index = 0; index < KKP_MAX; index++) {
			ValueS prev = pparam->getKKP(index);
			ValueF grad = g.getKKP(index);
			ValueS value = adjustValue(prev, grad);
			pparam->setKKP(index, value);
		}
	}

	ValueS Learn::adjustValue(ValueS prev, ValueF grad) {
		int x = (int)random.getBit() + (int)random.getBit();

		if (prev > 0) {
			grad -= PENALTY;
		} else if (prev < 0) {
			grad += PENALTY;
		}

		if (grad > 0.0) {
			return prev + ValueS(x);
		} else if (grad < 0.0) {
			return prev - ValueS(x);
		}
		return prev;
	}
}

#endif // NLEARN
