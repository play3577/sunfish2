/*
 * learnConfig.cpp
 *
 *  Created on: 2013/02/03
 *      Author: ryosuke
 */

#include "learnConfig.h"

namespace Learns {
	using namespace Configures;

	LearnConfig::LearnConfig(const char* filename) {
		int i = 0;
		items[i++] = ConfigItem("dir", STRING, &dir, "Csa");
		items[i++] = ConfigItem("steps", INTEGER, &steps, "1");

		if (filename != NULL) {
			read(filename);
		}
	}
}
