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
		// 他のスレッドが終わるまでだけ job の受付を行う場合
		if (suspend != NULL) {
			boost::mutex::scoped_lock lock(psearcher->getSplitMutex());
			// worker を解放
			job = false;
			psearcher->addIdleWorker();
		}

		while (true) {
			// 他のスレッドが終了したらもとの処理に戻る。
			if (suspend != NULL) {
				boost::mutex::scoped_lock lock(psearcher->getSplitMutex());
				if (!job && suspend->split.childCount == 0) {
					// worker の状態をもとに戻して再開
					tree = suspend->split.self;
					job = true;
					psearcher->reduceIdleWorker();
					return;
				}
			}

			// worker の終了
			if (shutdown) {
				break;
			}

			if (job) {
				// job の実行
				psearcher->searchChildTree(tree);

				{
					boost::mutex::scoped_lock lock(psearcher->getSplitMutex());
					// tree の解放
					psearcher->releaseTree(tree);
					// worker (自分)の解放
					job = false;
					psearcher->addIdleWorker();
				}
			}

			boost::thread::yield();
		}
	}
}
