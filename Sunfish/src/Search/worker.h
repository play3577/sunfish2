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
	class Tree;
	class Searcher;

	class Worker {
	private:
		boost::thread* thread;
		Searcher* psearcher;
		int tree;
		int worker;
		bool job;
		bool shutdown;

		void waitForJob(Tree* suspend = NULL);

	public:
		Worker(int worker = 0, Searcher* psearcher = NULL) :
			psearcher(psearcher), worker(worker) {
		}

		void init(Searcher* psearcher, int worker) {
			this->psearcher = psearcher;
			this->worker = worker;
		}

		void start() {
			shutdown = false;
			thread = new boost::thread(boost::bind(
					&Worker::waitForJob, this, (Tree*)NULL));
		}

		void stop() {
			shutdown = true;
			thread->join();
			delete thread;
		}
	};
}

#endif // WORKER_H_
