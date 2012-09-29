/*
 * codeGenerator.h
 *
 *  Created on: 2012/09/29
 *      Author: ryosuke
 */

#include <string>
#include <fstream>
#include "../Log/logger.h"

namespace Develop {
	class CodeGenerator {
	private:

	public:
		CodeGenerator() {
		}

		virtual std::string develop() = 0;

		bool write(const char* filename) {
			std::ofstream fout(filename);
			if (!fout) {
				Log::error << "ERROR: can't open a file :\"" << filename << "\"\n";
				return false;
			}
			fout << develop();
			fout.close();
			return true;
		}
	};
}
