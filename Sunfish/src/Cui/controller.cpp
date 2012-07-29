/*
 * controller.cpp
 *
 *  Created on: 2012/07/01
 *      Author: ryosuke
 */

#include "controller.h"
#include "../Search/searcher.h"
#include "../Search/attackers.h"
#include "../Csa/csaReader.h"

namespace Cui {
	using namespace Shogi;
	using namespace Search;
	using namespace Evaluate;
	using namespace Csa;

	void Controller::init(int argc, char* argv[]) {
		for (int i = 0; i < argc; i++) {
			if (0 == strcmp(argv[i], "-f")) {
				if (argc > i + 1) {
					config.filename = argv[++i];
				}
			} else if (0 == strcmp(argv[i], "-b")) {
				config.autoBlack = true;
			} else if (0 == strcmp(argv[i], "-w")) {
				config.autoWhite = true;
			}
		}
	}

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
#ifndef NDEBUG
		} else if (0 == strcmp(str, "see")) {
			return SEE;
#endif // ifndef NDEBUG
		}
		return UNKNOWN;
	}

	void Controller::showLegalMoves(const Position& pos) {
		MoveGenerator gen(pos);
		gen.generate();
		const Move* pmove;
		while ((pmove = gen.next()) != NULL) {
			std::cout << pmove->toString() << ' ';
		}
		std::cout << '\n';
	}

	void Controller::showCaptures(const Position& pos) {
		MoveGenerator gen(pos);
		gen.generateCapture();
		const Move* pmove;
		while ((pmove = gen.next()) != NULL) {
			std::cout << pmove->toString() << ' ';
		}
		std::cout << '\n';
	}

	void Controller::showNoCaptures(const Position& pos) {
		MoveGenerator gen(pos);
		gen.generateNocapture();
		const Move* pmove;
		while ((pmove = gen.next()) != NULL) {
			std::cout << pmove->toString() << ' ';
		}
		std::cout << '\n';
	}

	void Controller::SeeTest(const Position& pos, const Param& param) {
		MoveGenerator gen(pos);
		gen.generateCapture();
		const Move* pmove;
		while ((pmove = gen.next()) != NULL) {
			Attackers attackers(param, pos, *pmove);
			std::cout << pmove->toString() << ':' << (int)attackers.see() << '\n';
		}
	}

	bool Controller::play() {
		char line[1024];
		Command prevCommand = UNKNOWN;
		Record::Record record;
		Searcher searcher(*pparam);
		SearchConfig searchConfig;
		SearchResult result;

		// 棋譜読み込み
		if (config.filename != NULL) {
			if (!CsaReader::read(config.filename, record)) {
				std::cout << "ERROR: can't read a file :" << config.filename << '\n';
			}
		}

		// 探索設定
		searchConfig.depth = 5;
		searchConfig.pvHandler = this;
		searcher.setSearchConfig(searchConfig);

		std::cout << record.toString();

		while (true) {
			bool printBoard = false;
			Move move;

			// コンピュータによる着手
			if ((record.getPosition().isBlackTurn() && config.autoBlack) ||
					(record.getPosition().isWhiteTurn() && config.autoWhite)) {
				searcher.init(record.getPosition());
				searcher.idSearch(result);
				std::cout << result.toString();
				record.move(result.move);
				std::cout << record.toString();
				continue;
			}

			// ユーザからのコマンド入力
			std::cin.getline(line, sizeof(line));
			if (std::cin.eof()) {
				break;
			}
			Command command = line[0] != '\0' ? inputCommand(line) : prevCommand;
			switch(prevCommand = command) {
			case PREV: // 1手進む。
				if (record.prev()) {
					printBoard = true;
				} else {
					std::cout << "There is no previous move.\n";
				}
				break;
			case NEXT: // 1手戻る。
				if (record.next()) {
					printBoard = true;
				} else {
					std::cout << "There is no next move.\n";
				}
				break;
			case MOVES: // 指し手列挙
				showLegalMoves(record.getPosition());
				break;
			case CAPTURES: // 駒を取る手列挙
				showCaptures(record.getPosition());
				break;
			case NOCAPTURES: // 駒を取らない手列挙
				showNoCaptures(record.getPosition());
				break;
			case SEARCH: // 探索
				searcher.init(record.getPosition());
				searcher.idSearch(result);
				std::cout << result.toString();
				if (result.noMoves) {
					std::cout << "lose.\n";
				}
				break;
#ifndef NDEBUG
			case SEE:
				SeeTest(record.getPosition(), *pparam);
				break;
#endif // ifndef NDEBUG
			default: // 指し手入力
				if ((record.getPosition().inputMoveCsa(line, move)) ||
						(record.getPosition().inputMove(line, move))) {
					// 合法手チェック
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

			// 盤面の表示
			if (printBoard) {
				std::cout << record.toString();
			}
		}

		return true;
	}
}
