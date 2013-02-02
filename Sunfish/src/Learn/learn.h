/*
 * learn.h
 *
 *  Created on: 2013/02/02
 *      Author: ryosuke
 */

#ifndef LEARN_H_
#define LEARN_H_

#include "learnConfig.h"

namespace Learns {
	class Learn {
	private:
		const char* configFilename;

		LearnConfig config;

	public:
		static const char* DEFAULT_CONFIG_FILE;

		Learn() {
			configFilename = DEFAULT_CONFIG_FILE;
		}

		void setConfigFile(const char* filename) {
			configFilename = filename;
		}

		void execute();
	};
}

#endif // LEARN_H_
