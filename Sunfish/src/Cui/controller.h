/*
 * controller.h
 *
 *  Created on: 2012/07/01
 *      Author: ryosuke
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "../Evaluate/param.h"
#include "../Evaluate/initializer.h"
#include "../Search/pvHandler.h"
#include "../Record/record.h"

namespace Cui {
	class Controller : public Search::PvHandler {
	private:
		enum Command {
			QUIT = 0,
			HELP,
			PREV,
			NEXT,
			SEARCH,
			MOVES,
			CAPTURES,
			NOCAPTURES,
#ifndef NDEBUG
			SEE,
#endif // ifndef NDEBUG
			CMD_NUM,

			UNKNOWN,
		};

		struct CommandSet {
			const char* shortStr;
			const char* longStr;
			Command command;
			const char* description;
		};
		static const CommandSet commandSet[CMD_NUM];

		void showHelp();

		void showLegalMoves(const Shogi::Position& pos);

		void showCaptures(const Shogi::Position& pos);

		void showNoCaptures(const Shogi::Position& pos);

		void SeeTest(const Shogi::Position& pos, const Evaluate::Param& param);

		Evaluate::Param* pparam;

		// settings
		struct {
			const char* filename;
			bool autoBlack;
			bool autoWhite;
		} config;

		Command inputCommand(const char* str);

	public:
		Controller() {
			pparam = new Evaluate::Param();
			Evaluate::Initializer::apply(*pparam);
			pparam->read("evdata");
			initSettings();
		}

		virtual ~Controller() {
			delete pparam;
		}

		void initSettings() {
			config.filename = NULL;
			config.autoBlack = false;
			config.autoWhite = false;
		}

		void setFilename(const char* filename) {
			config.filename = filename;
		}

		void setAutoBlack(bool autoBlack) {
			config.autoBlack = autoBlack;
		}

		void setAutoWhite(bool autoWhite) {
			config.autoWhite = autoWhite;
		}

		void pvHandler(const Search::Pv& pv, Evaluate::Value value) {
			std::cout << pv.toString() << ':' << value << '\n';
		}

		bool play();
	};
}

#endif // CONTROLLER_H_
