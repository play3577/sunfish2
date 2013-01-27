/*
 * controller.h
 *
 *  Created on: 2012/07/01
 *      Author: ryosuke
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "../Evaluates/param.h"
#include "../Evaluates/initializer.h"
#include "../Search/pvHandler.h"
#include "../Records/record.h"
#include "../Shogi/moveGenerator.h"
#include "../Log/logger.h"
#include <iomanip>

namespace Cui {
	class Controller : public Search::PvHandler {
	private:
		enum Command {
			EMPTY = 0,
			QUIT,
			HELP,
			PREV,
			NEXT,
			TOP,
			END,
			SEARCH,
			MOVES,
			CAPTURES,
			NOCAPTURES,
			CHECK,
			MATED,
			MATE1,
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

		void showCheck(const Shogi::Position& pos);

		void showMoves(Shogi::MoveGenerator& gen);

		void SeeTest(const Shogi::Position& pos, const Evaluates::Param& param);

		Evaluates::Param* pparam;

		// settings
		struct {
			const char* filename;
			bool autoBlack;
			bool autoWhite;
			int depth;
			int limit;
		} config;

		Command inputCommand(const char* str);

		bool inputMove(const char* str, const Shogi::Position& pos, Shogi::Move& move) const;

		void printPosition(const Records::Record& record) const;

		void printStatus(const Records::Record& record) const;

	public:
		Controller() {
			pparam = new Evaluates::Param();
			Evaluates::Initializer::apply(*pparam);
			if (!pparam->read("evdata")) {
				Log::error << "ERROR : can not read \"evdata\"\n";
			}
			initSettings();
		}

		virtual ~Controller() {
			delete pparam;
		}

		void initSettings() {
			config.filename = NULL;
			config.autoBlack = false;
			config.autoWhite = false;
			config.depth = 32;
			config.limit = 10;
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

		void setDepth(int depth) {
			config.depth = depth;
		}

		void setLimit(int limit) {
			config.limit = limit;
		}

		void pvHandler(const Search::Pv& pv, Evaluates::Value value,
				Util::uint64 nodes, int depth, double seconds) {
			std::cout << std::setw(2) << depth << std::setw(10) << nodes
					<< ' ' << pv.toString() << ':' << value
					<< " (" << seconds << "sec)\n";
		}

		bool play();
	};
}

#endif // CONTROLLER_H_
