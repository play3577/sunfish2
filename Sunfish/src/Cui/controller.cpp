/*
 * controller.cpp
 *
 *  Created on: 2012/07/01
 *      Author: ryosuke
 */

#include "../Record/record.h"
#include "controller.h"
#include "../Search/searcher.h"
#include "../Evaluate/initializer.h"

namespace Cui {
	using namespace Shogi;
	using namespace Search;
	using namespace Evaluate;

	Controller::Command Controller::inputCommand(const char* str) {
		if (0 == strcmp(str, "prev") ||
				0 == strcmp(str, "p")) {
			return PREV;
		} else if (0 == strcmp(str, "next") ||
				0 == strcmp(str, "n")) {
			return NEXT;
		} else if (0 == strcmp(str, "moves") ||
				0 == strcmp(str, "m")) {
			return MOVES;
		} else if (0 == strcmp(str, "captures") ||
				0 == strcmp(str, "cap")) {
			return CAPTURES;
		} else if (0 == strcmp(str, "nocaptures") ||
				0 == strcmp(str, "ncap")) {
			return NOCAPTURES;
		} else if (0 == strcmp(str, "search") ||
				0 == strcmp(str, "s")) {
			return SEARCH;
		}
		return UNKNOWN;
	}

	void Controller::showLegalMoves(Position pos) {
		MoveGenerator gen(pos);
		gen.generate();
		const Move* pmove;
		while ((pmove = gen.next()) != NULL) {
			std::cout << pmove->toString() << ' ';
		}
		std::cout << '\n';
	}

	void Controller::showCaptures(Position pos) {
		MoveGenerator gen(pos);
		gen.generateCapture();
		const Move* pmove;
		while ((pmove = gen.next()) != NULL) {
			std::cout << pmove->toString() << ' ';
		}
		std::cout << '\n';
	}

	void Controller::showNoCaptures(Position pos) {
		MoveGenerator gen(pos);
		gen.generateNocapture();
		const Move* pmove;
		while ((pmove = gen.next()) != NULL) {
			std::cout << pmove->toString() << ' ';
		}
		std::cout << '\n';
	}

	bool Controller::play() {
		char line[1024];
		Command prevCommand;
		Record::Record record;
		Param param;
		Initializer::apply(param);
		Searcher searcher(param);
		SearchConfig config;
		SearchResult result;

		config.depth = 5;
		searcher.setSearchConfig(config);

		std::cout << record.toString();

		while (true) {
			bool printBoard = false;
			Move move;

			// user input
			std::cin.getline(line, sizeof(line));
			if (std::cin.eof()) {
				break;
			}
			Command command = line[0] != '\0' ? inputCommand(line) : prevCommand;
			switch(prevCommand = command) {
			case PREV:
				if (record.prev()) {
					printBoard = true;
				} else {
					std::cout << "There is no previous move.\n";
				}
				break;
			case NEXT:
				if (record.next()) {
					printBoard = true;
				} else {
					std::cout << "There is no next move.\n";
				}
				break;
			case MOVES:
				showLegalMoves(record.getPosition());
				break;
			case CAPTURES:
				showCaptures(record.getPosition());
				break;
			case NOCAPTURES:
				showNoCaptures(record.getPosition());
				break;
			case SEARCH:
				searcher.init(record.getPosition());
				if (searcher.search(result)) {
					std::cout << result.move.toString() << '(' << (int)result.value << ")\n";
					std::cout << result.pv.toString() <<  '\n';
				} else {
					std::cout << "lose.\n";
				}
				break;
			default:
				if ((record.getPosition().inputMoveCsa(line, move)) ||
						(record.getPosition().inputMove(line, move))) {
					if (record.move(move)) {
						std::cout << move.toString() << '\n';
						printBoard = true;
					} else {
						std::cout << "illegal move!!\n";
					}
				} else {
					std::cout << "unknown command!!\n";
				}
			}

			if (printBoard) {
				std::cout << record.toString();
			}
		}

		return true;
	}
}
