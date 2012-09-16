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
		if (tokens[0] == "host") {
			host = tokens[1];
		} else if (tokens[0] == "port") {
			port = boost::lexical_cast<int>(tokens[1]);
		} else if (tokens[0] == "user") {
			user = tokens[1];
		} else if (tokens[0] == "pass") {
			pass = tokens[1];
		} else if (tokens[0] == "depth") {
			depth = boost::lexical_cast<int>(tokens[1]);
		} else if (tokens[0] == "limit") {
			limit = boost::lexical_cast<int>(tokens[1]);
		} else if (tokens[0] == "repeat") {
			repeat = boost::lexical_cast<int>(tokens[1]);
		} else if (tokens[0] == "keepalive") {
			keepalive = boost::lexical_cast<int>(tokens[1]);
		} else if (tokens[0] == "keepidle") {
			keepidle = boost::lexical_cast<int>(tokens[1]);
		} else if (tokens[0] == "keepintvl") {
			keepintvl = boost::lexical_cast<int>(tokens[1]);
		} else if (tokens[0] == "keepcnt") {
			keepcnt = boost::lexical_cast<int>(tokens[1]);
		} else if (tokens[0] == "kifu") {
			kifu = tokens[1];
		} else {
			return false;
		}
		return true;
	}
}
