/*
 * csaClientConfig.cpp
 *
 *  Created on: 2012/09/08
 *      Author: ryosuke
 */

#include "csaClientConfig.h"
#include "../Log/logger.h"
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace Network {
	CsaClientConfig::CsaClientConfig(const char* filename) {
		int i = 0;
		items[i++] = ConfigItem("host", STRING, &host);
		items[i++] = ConfigItem("port", INTEGER, &port);
		items[i++] = ConfigItem("user", STRING, &user);
		items[i++] = ConfigItem("pass", STRING, &pass);
		items[i++] = ConfigItem("depth", INTEGER, &depth);
		items[i++] = ConfigItem("limit", INTEGER, &limit);
		items[i++] = ConfigItem("repeat", INTEGER, &repeat);
		items[i++] = ConfigItem("enemy", BOOL, &enemy);
		items[i++] = ConfigItem("keepalive", INTEGER, &keepalive);
		items[i++] = ConfigItem("keepidle", INTEGER, &keepidle);
		items[i++] = ConfigItem("keepintvl", INTEGER, &keepintvl);
		items[i++] = ConfigItem("keepcnt", INTEGER, &keepcnt);
		items[i++] = ConfigItem("kifu", STRING, &kifu);

		if (filename != NULL) {
			read(filename);
		}
	}

	bool CsaClientConfig::read(const char* filename) {
		std::ifstream fin(filename);
		if (!fin) {
			Log::error << "ERROR: can't open a file :\"" << filename << "\"\n";
			return false;
		}
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
		fin.close();
		return true;
	}

	bool CsaClientConfig::readLine(const char* line) {
		if (line[0] == '\0' || line[0] == '#') {
			return true;
		}
		std::vector<std::string> tokens;
		boost::algorithm::split(tokens, line, boost::is_any_of("="));
		if (tokens.size() != 2) {
			return false;
		}
		for (unsigned i = 0; i < sizeof(items)/sizeof(items[0]); i++){
			if (tokens[0] == items[i].name) {
				if (items[i].type == STRING) {
					*(std::string*)items[i].data = tokens[1];
				} else if (items[i].type == INTEGER) {
					*(int*)items[i].data = boost::lexical_cast<int>(tokens[1]);
				} else if (items[i].type == BOOL) {
					*(bool*)items[i].data = boost::lexical_cast<bool>(tokens[1]);
				} else {
					Log::error << "Unknown Error.." << __FILE__ << "(" << __LINE__ << ")\n";
				}
				return true;
			}
		}
		return false;
	}
}
