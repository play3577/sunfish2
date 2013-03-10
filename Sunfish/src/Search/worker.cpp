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
			unsetJob();
			psearcher->addIdleWorker();
		}

		while (true) {
			// 他のスレッドが終了したらもとの処理に戻る。
			if (suspend != NULL) {
				//boost::mutex::scoped_lock lock(suspend->getMutex());
				if (suspend->split.childCount == 0) {
					boost::mutex::scoped_lock lock(psearcher->getSplitMutex());
					if (!hasJob()) {
						// worker の状態をもとに戻して再開
						setJob(suspend->split.self);
						psearcher->reduceIdleWorker();
						return;
					}
				}
			}

			// worker の終了
			if (shutdown) {
				return;
			}

			if (hasJob()) {
				// job の実行
				psearcher->searchChildTree(tree);

				{
					boost::mutex::scoped_lock lock(psearcher->getSplitMutex());
					// tree の解放
					psearcher->releaseTree(tree);
					// worker (自分)の解放
					unsetJob();
					psearcher->addIdleWorker();
					if (suspend != NULL && suspend->split.childCount == 0) {
						// worker の状態をもとに戻して再開
						setJob(suspend->split.self);
						psearcher->reduceIdleWorker();
						return;
					}
				}
			}

			boost::thread::yield();
		}
	}
}
