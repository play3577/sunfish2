/*
 * configure.cpp
 *
 *  Created on: 2013/02/03
 *      Author: ryosuke
 */

#include "configure.h"
#include "../Log/logger.h"
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace Configures {
	bool Configure::read(const char* filename) {
		// open a configure file.
		std::ifstream fin(filename);
		if (!fin) {
			Log::error << "ERROR: can't open a file :\"" << filename << "\"\n";
			return false;
		}
		// input
		for (int l = 0; ; l++) {
			char line[LINE_BUFFER_SIZE];
			fin.getline(line, sizeof(line));
			if (fin.eof()) { break; }
			if (fin.fail()) {
				Log::error << "ERROR: unknown i/o error.\n";
				return false;
			}
			if (!readLine(line)) {
				Log::error << "ERROR: line(" << l << ") :\"" << line << "\"\n";
			}
		}
		fin.close(); // close a configure file.
		return true;
	}

	void Configure::init() {
		// 初期値を代入
		ConfigItem* items = itemList();
		int size = itemSize();
		for (int i = 0; i < size; i++){
			// 設定項目のデータ型毎に変換
			if (!convert(items[i], items[i].defaultValue)) {
				Log::error << "Unknown Error.." << __FILE__ << "(" << __LINE__ << ")\n";
			}
		}
	}

	bool Configure::readLine(const char* line) {
		if (line[0] == '\0' || line[0] == '#') {
			return true;
		}
		// '=' で左辺値と右辺値に分解
		std::vector<std::string> tokens;
		boost::algorithm::split(tokens, line, boost::is_any_of("="));
		if (tokens.size() != 2) {
			return false;
		}
		// 左辺値に一致する項目を探す。
		ConfigItem* items = itemList();
		int size = itemSize();
		for (int i = 0; i < size; i++){
			if (tokens[0] == items[i].name) {
				// 設定項目のデータ型毎に変換
				if (convert(items[i], tokens[1])) {
					return true;
				} else {
					Log::error << "Unknown Error.." << __FILE__ << "(" << __LINE__ << ")\n";
					return false;
				}
			}
		}
		return false;
	}

	bool Configure::convert(ConfigItem& item, const std::string& str) {
		// 設定項目のデータ型毎に変換
		if (item.type == STRING) {
			*(std::string*)item.data = str;
		} else if (item.type == INTEGER) {
			*(int*)item.data = boost::lexical_cast<int>(str);
		} else if (item.type == BOOL) {
			*(bool*)item.data = boost::lexical_cast<bool>(str);
		} else {
			return false;
		}
		return true;
	}
}
