/*
 * worker.cpp
 *
 *  Created on: 2013/02/17
 *      Author: ryosuke
 */

#include "worker.h"
#include "searcher.h"

namespace Search {
	void Worker::waitForJob(Tree* suspend) {
		if (suspend != NULL) {
			boost::mutex::scoped_lock lock(psearcher->getSplitMutex());
			job = false;
		}

		while (true) {
			if (shutdown) {
				break;
			}
			boost::thread::yield();
		}
	}
}
