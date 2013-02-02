/*
 * learnConfig.h
 *
 *  Created on: 2013/02/02
 *      Author: ryosuke
 */

#ifndef LEARNCONFIG_H_
#define LEARNCONFIG_H_

#include "../Util/tableString.h"
#include "../Config/configure.h"

namespace Learns {
	class LearnConfig : public Configures::Configure {
	private:
		Configures::ConfigItem items[2];

		std::string dir; // 棋譜の保存場所
		int steps; // 反復回数

	protected:
		Configures::ConfigItem* itemList() {
			return items;
		}

		int itemSize() {
			return sizeof(items)/sizeof(items[0]);
		}

	public:
		LearnConfig(const char* filename = NULL);

		std::string getDir() const {
			// TODO: 共通化
			if (dir.length() > 0 && dir[dir.length()-1] == '/') {
				return dir;
			} else {
				return dir + '/';
			}
		}

		int getSteps() const {
			return steps;
		}

		std::string toString() const {
			Util::TableString table("* ", " :", "");
			table.row() << "DIR" << getDir();
			table.row() << "STEPS" << getSteps();
			return table.get();
		}
	};
}

#endif // LEARNCONFIG_H_
