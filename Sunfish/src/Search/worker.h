/*
 * worker.h
 *
 *  Created on: 2013/02/17
 *      Author: ryosuke
 */

#ifndef WORKER_H_
#define WORKER_H_

#define BOOST_THREAD_USE_LIB
#include <boost/thread.hpp>

namespace Search {
	class Searcher;

	class Worker {
	private:
		boost::thread* thread;
		Searcher* psearcher;
		int tree;
		int worker;
		bool job;
		bool shutdown;

		void waitForJob();

	public:
		Worker(int worker = 0, Searcher* psearcher = NULL) :
			worker(worker),
			psearcher(psearcher) {
		}

		void init(worker, Searcher* psearcher) {
			this->worker = worker;
			this->psearcher = psearcher;
		}

		void start() {
			shutdown = false;
			job = false;
			thread = new boost::thread(boost::bind(
					&Worker::waitForJob, this));
		}

		void stop() {
			shutdown = true;
			thread->join();
			delete thread;
		}
	};
}

#endif // WORKER_H_
