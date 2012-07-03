/*
 * controller.cpp
 *
 *  Created on: 2012/07/01
 *      Author: ryosuke
 */

#include "../Record/record.h"
#include "controller.h"

namespace Cui {
	using namespace Shogi;

	Controller::Command Controller::inputCommand(const char* str) {
		if (0 == strcmp(str, "prev") ||
				0 == strcmp(str, "p")) {
			return PREV;
		} else if (0 == strcmp(str, "next") ||
				0 == strcmp(str, "n")) {
			return NEXT;
		}
		return UNKNOWN;
	}

	bool Controller::play() {
		char line[1024];
		Command prevCommand;
		Record::Record record;

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
