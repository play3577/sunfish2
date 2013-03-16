/*
 * bookManager.cpp
 *
 *  Created on: 2013/03/14
 *      Author: ryosuke
 */

#include "bookManager.h"
#include "../Log/logger.h"
#include "../Csa/csaReader.h"
#include "../Util/fileList.h"

namespace Books {
	using namespace Records;
	using namespace Csa;
	using namespace Shogi;
	using namespace Util;

	bool BookManager::importFile(const char* path) {
		// 棋譜の読み込み
		Log::message << path << '\n';
		Record record;
		if (!CsaReader::read(path, record)) {
			return false;
		}
		// 最終手から順に棋譜を見ていく。
		record.end();
		while (record.prev()) {
			Move move;
			record.getNextMove(move);
			book.addMove(record.getPosition().getHash(), move);
		}
		return true;
	}

	bool BookManager::importDirectory(const char* directory) {
		// 棋譜の列挙
		FileList fileList;
		fileList.enumerate(directory, "csa");
		while (!fileList.isEnd()) {
			if (!importFile(fileList.pop().c_str())) {
				return false;
			}
		}
		return true;
	}
}
