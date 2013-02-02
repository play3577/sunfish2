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
		// $B=i4|CM$rBeF~(B
		ConfigItem* items = itemList();
		int size = itemSize();
		for (int i = 0; i < size; i++){
			// $B@_Dj9`L\$N%G!<%?7?Kh$KJQ49(B
			if (!convert(items[i], items[i].defaultValue)) {
				Log::error << "Unknown Error.." << __FILE__ << "(" << __LINE__ << ")\n";
			}
		}
	}

	bool Configure::readLine(const char* line) {
		if (line[0] == '\0' || line[0] == '#') {
			return true;
		}
		// '=' $B$G:8JUCM$H1&JUCM$KJ,2r(B
		std::vector<std::string> tokens;
		boost::algorithm::split(tokens, line, boost::is_any_of("="));
		if (tokens.size() != 2) {
			return false;
		}
		// $B:8JUCM$K0lCW$9$k9`L\$rC5$9!#(B
		ConfigItem* items = itemList();
		int size = itemSize();
		for (int i = 0; i < size; i++){
			if (tokens[0] == items[i].name) {
				// $B@_Dj9`L\$N%G!<%?7?Kh$KJQ49(B
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
		// $B@_Dj9`L\$N%G!<%?7?Kh$KJQ49(B
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
