/*
 * learn.cpp
 *
 *  Created on: 2013/02/02
 *      Author: ryosuke
 */

#include "learn.h"
#include "../Log/logger.h"

#ifndef NLEARN

namespace Learns {
	const char* Learn::DEFAULT_CONFIG_FILE = "lconf";

	void Learn::execute() {
		// load settings
		if (!config.read(configFilename)) {
			return;
		}
		Log::message << config.toString();
	}
}

#endif // NLEARN
