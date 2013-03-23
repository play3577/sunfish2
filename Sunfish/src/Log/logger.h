/*
 * log.h
 * 
 *  Created on: 2012/07/29
 *      Author: Kubo Ryosuke
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <iostream>
#include <vector>

#define __THIS__			__FILE__ << '(' << __LINE__ << ')'

namespace Log {
	class Logger {
	private:
		struct STREAM {
			std::ostream* pout;
			const char* before;
			const char* after;
		};

		const char* name;

		std::vector<STREAM> os;

		void putPrefix();

		void _formatError(const char* str = NULL, int column = -1);

	public:
		Logger(const char* name = NULL) : name(name) {
		}

		void addStream(std::ostream& o) {
			addStream(o, NULL, NULL);
		}

		void addStream(std::ostream& o,
				const char* before,
				const char* after) {
			STREAM s = { &o, before, after };
			os.push_back(s);
		}

		template <class T>
		Logger& operator<<(T t) {
			std::vector<STREAM>::iterator it;
			for (it = os.begin(); it != os.end(); it++) {
				if (it->before != NULL) {
					*(it->pout) << it->before;
				}
				*(it->pout) << t;
				if (it->after != NULL) {
					*(it->pout) << it->after;
				}
				it->pout->flush();
			}
			return *this;
		}

		void unknownError(const char* detail = NULL);

		void fileOpenMessage(const char* filename);

		void fileOpenError(const char* filename);

		void fileIoError(const char* filename,
				const char* detail = NULL);

		void ioError(const char* detail = NULL);

		void fileFormatError(const char* filename,
				const char* str = NULL, int column = -1);

		void fileFormatError(const char* filename, int line,
				const char* str = NULL, int column = -1);

		void formatError(const char* str = NULL, int column = -1);

		void formatError(int line, const char* str = NULL, int column = -1);
	};

	extern Logger error;
	extern Logger warning;
	extern Logger message;
	extern Logger send;
	extern Logger receive;
#ifndef NDEBUG
	extern Logger debug;
	extern Logger test;
#endif //NDEBUG
}

#endif // LOGGER_H_
