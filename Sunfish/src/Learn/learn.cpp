/*
 * learn.cpp
 *
 *  Created on: 2013/02/02
 *      Author: ryosuke
 */

#include "learn.h"
#include "../Log/logger.h"
#include "../Csa/csaReader.h"
#include "../Search/searcher.h"
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#ifndef NLEARN

namespace Learns {
	const char* Learn::DEFAULT_CONFIG_FILE = "lconf";

	using namespace Records;
	using namespace Csa;
	using namespace Shogi;
	using namespace Search;

	void Learn::execute() {
		// load settings
		if (!config.read(configFilename)) {
			return;
		}
		Log::message << config.toString();

		for (int step = 1; step <= config.getSteps(); step++) {
			analyzeAllFiles();
		}
	}

	void Learn::analyzeAllFiles() {
		// 棋譜ファイルの列挙
		namespace fs = boost::filesystem;
		const fs::path path(config.getDir());
		BOOST_FOREACH(const fs::path& p, std::make_pair(
				fs::directory_iterator(path),
				fs::directory_iterator())) {
			if (!fs::is_directory(p)) {
				analyzeFile(p.c_str());
			}
		}
	}

	void Learn::analyzeFile(const char* path) {
		Log::message << path << '\n';

		// 棋譜の読み込み
		Record record;
		CsaReader::read(path, record);

		// 探索エージェント
		Searcher searcher(*pparam);
		SearchConfig searchConfig;
		searchConfig.depth = config.getDepth();
		searchConfig.pvHandler = NULL;
		searchConfig.limitEnable = false;
		searcher.setSearchConfig(searchConfig);

		// 初手から順に棋譜を見ていく。
		record.begin();
		while (record.next()) {
			// 棋譜の指し手
			Move recMove;
			record.getPrevMove(recMove);
			Log::debug << recMove.toString() << ' ';

			// その他の手
		}
		Log::debug << '\n';
	}
}

#endif // NLEARN
