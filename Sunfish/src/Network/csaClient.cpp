/*
 * csaClient.cpp
 *
 *  Created on: 2012/08/25
 *      Author: ryosuke
 */

#include "csaClient.h"
#include "../Csa/csaReader.h"
#include "../Csa/csaWriter.h"
#include "../Search/searcher.h"

#include <sstream>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>


namespace Network {
	using namespace Shogi;
	using namespace Csa;
	using namespace Search;
	using namespace Records;

	const char* CsaClient::DEFAULT_CONFIG_FILE = "nconf";

	const CsaClient::ReceiveFlagSet CsaClient::flagSets[RECV_NUM] = {
		{ boost::regex("^LOGIN:.* OK$"), RECV_LOGIN_OK, NULL },
		{ boost::regex("^LOGIN:incorect$"), RECV_LOGIN_INC, NULL },
		{ boost::regex("^LOGOUT:completed$"), RECV_LOGOUT, NULL },
		{ boost::regex("^%.*"), RECV_MOVE_EX, recvMove },
		{ boost::regex("^\\+.*"), RECV_MOVE_B, recvMove },
		{ boost::regex("^-.*"), RECV_MOVE_W, recvMove },
		{ boost::regex("^BEGIN Game_Summary$"), RECV_SUMMARY, st_recvGameSummary },
		{ boost::regex("^START:.*"), RECV_START, NULL },
		{ boost::regex("^REJECT:.* by .*"), RECV_REJECT, NULL },
		{ boost::regex("^#WIN$"), RECV_WIN, NULL },
		{ boost::regex("^#LOSE$"), RECV_LOSE, NULL },
		{ boost::regex("^#DRAW$"), RECV_DRAW, NULL },
		{ boost::regex("^#CHUDAN$"), RECV_INTERRUPT, NULL },
		{ boost::regex("^#SENNICHITE$"), RECV_REPEAT, NULL },
		{ boost::regex("^#OUTE_SENNICHITE$"), RECV_CHECK_REP, NULL },
		{ boost::regex("^#ILLEGAL_MOVE$"), RECV_ILLEGAL, NULL },
		{ boost::regex("^#TIME_UP$"), RECV_TIME_UP, NULL },
		{ boost::regex("^#RESIGN$"), RECV_RESIGN, NULL },
		{ boost::regex("^#JISHOGI$"), RECV_JISHOGI, NULL },
	};

	bool CsaClient::execute() {
		// 設定の読み込み
		if (!config.read(configFilename)) {
			Log::error << "ERROR : can not read configurations.\n";
			return false;
		}

		con.setHost(config.getHost());
		con.setPort(config.getPort());
		con.setKeepalive(config.getKeepalive(), config.getKeepidle(),
			config.getKeepintvl(), config.getKeepcnt());

		// 連続対局
		for (int i = 0; i < config.getRepeat(); i++) {
			if (!con.connect()) {
				Log::error << "ERROR : can not connect to [" << config.getHost()
						<< "] (port:" << config.getPort() << ")\n";
				return false;
			}
			init();
			boost::thread receiverThread(boost::bind(&CsaClient::receiver, this));

			// login
			if (!login()) {
				Log::message << "login failed!\n";
				goto lab_end;
			}
			Log::message << "login ok!!\n";

			// wait for match-make and agree
			if (waitGameSummary() && agree()) {
				Record record(pos);
				Searcher searcher(*pparam);
				SearchConfig searchConfig;

				searchConfig.depth = config.getDepth();
				searchConfig.pvHandler = this;
				searchConfig.limitEnable = config.getLimit() != 0;
				searchConfig.limitSeconds = config.getLimit();
				searcher.setSearchConfig(searchConfig);

				while (1) {
					Log::message << record.toString();
					if (black == record.getPosition().isBlackTurn()) {
						// my turn
						SearchResult result;
						searcher.init(record.getPosition());
						searcher.idSearch(result);
						if (!result.resign && record.move(result.move)) {
							if (!sendMove(result.move)) {
								break;
							}
						} else {
							sendResign();
							break;
						}
					} else {
						// enemy's turn
						unsigned mask = black ? RECV_MOVE_W : RECV_MOVE_B;
						unsigned flags = waitReceive(mask | RECV_END_MSK);
						if (flags & mask) {
							if (!CsaReader::parseLineMove(moveStr.c_str(), record)) {
								Log::error << "ERROR :illegal move!!\n";
								break;
							}
						} else if (flags & RECV_END_MSK) {
							break;
						} else {
							Log::error << "ERROR :see receive-log.\n";
							break;
						}
					}
				}
				writeResult(record);
			}

			// logout
			logout();

lab_end:
			con.disconnect();
			receiverThread.join();
		}
		return true;
	}

	bool CsaClient::login() {
		std::ostringstream os;
		os << "LOGIN " << config.getUser() << ' ' << config.getPass();
		if (!send(os.str().c_str())) { return false; }
		unsigned result = waitReceive(RECV_LOGIN_MSK);
		return (result & RECV_LOGIN_OK) != 0U;
	}

	bool CsaClient::logout() {
		if (!send("LOGOUT")) { return false; }
#if 0
		unsigned result = waitReceive(RECV_LOGOUT);
		return (result & RECV_LOGOUT) != 0U;
#else
		// floodgateがLOGOUT:completedを送信してくれない。
		return true;
#endif
	}

	bool CsaClient::agree() {
		if (!send("AGREE")) { return false; }
		unsigned result = waitReceive(RECV_AGREE_MSK);
		return (result & RECV_START) != 0U;
	}

	bool CsaClient::sendMove(const Shogi::Move& move) {
		if (!send((move.toStringCsa()).c_str())) { return false; }
		unsigned mask = black ? RECV_MOVE_B : RECV_MOVE_W;
		unsigned result = waitReceive(mask & RECV_END_MSK);
		return (result & mask) != 0U;
	}

	bool CsaClient::sendResign() {
		if (!send("%TORYO")) { return false; }
		unsigned result = waitReceive(RECV_END_MSK);
		return (result & RECV_END_MSK) != 0U;
	}

	unsigned CsaClient::waitReceive(unsigned flags) {
		while (true) {
			{
				boost::mutex::scoped_lock lock(recvMutex);
				unsigned masked = recvFlags & flags;
				endFlags |= recvFlags & RECV_END_MSK;
				if (masked) {
					recvFlags &= ~masked;
					return masked;
				}
				if (recvFlags & RECV_LOGOUT) {
					recvFlags &= ~RECV_LOGOUT;
					return 0U;
				}
			}
			sleep(20);
		}
	}

	void CsaClient::receiver() {
		while (con.receive()) {
			recvStr = con.getReceivedString();
			printReceivedString();
			for (int i = 0; i < RECV_NUM; i++) {
				if (boost::regex_match(recvStr, flagSets[i].regex)) {
					if (flagSets[i].func != NULL) {
						flagSets[i].func(this);
					}
					boost::mutex::scoped_lock lock(recvMutex);
					recvFlags |= flagSets[i].flag;
				}
			}
		}
	}

	void CsaClient::recvGameSummary() {
		while (con.receive()) {
			recvStr = con.getReceivedString();
			printReceivedString();
			if (recvStr == "BEGIN Time") {
				recvTime();
				continue;
			} else if (recvStr == "BEGIN Position") {
				recvPosition();
				continue;
			} else if (recvStr == "END Game_Summary") {
				return;
			}

			std::vector<std::string> tokens;
			boost::algorithm::split(tokens, recvStr, boost::is_any_of(":"));
			if (tokens.size() < 2) {
				continue;
			}

			if (tokens[0] == "Your_Turn") {
				if (tokens[1] == "+") {
					black = true;
				} else if (tokens[1] == "-") {
					black = false;
				}
			} else if (tokens[0] == "Game_ID") {
				gameId = tokens[1];
			} else if (tokens[0] == "Name+") {
				blackName = tokens[1];
			} else if (tokens[0] == "Name-") {
				whiteName = tokens[1];
			}
			/*
				"Protocol_Version"
				"Protocol_Mode"
				"Format"
				"Declaration"
				"Rematch_On_Draw" // 自動再試合(1.1.3では無視してよい。)
				"To_Move"
			*/
		}
	}

	void CsaClient::recvTime() {
		while (con.receive()) {
			recvStr = con.getReceivedString();
			printReceivedString();
			if (recvStr == "END Time") {
				return;
			}
			std::vector<std::string> tokens;
			boost::algorithm::split(tokens, recvStr, boost::is_any_of(":"));
			if (tokens.size() < 2) {
				continue;
			}
			/*
				"Time_Unit"
				"Least_Time_Per_Move"
				"Total_Time"
				"Byoyomi"
			*/
		}
	}

	void CsaClient::recvPosition() {
		pos.setBoard(Square(5,5), Piece::BPAWN);
		Log::debug << pos.toString();
		while (con.receive()) {
			recvStr = con.getReceivedString();
			printReceivedString();
			if (recvStr == "END Position") {
				Log::debug << pos.toString();
				return;
			} else {
				CsaReader::LineStat stat =
						CsaReader::parseLine(recvStr.c_str(), pos);
				if (stat == CsaReader::LINE_ERROR) {
					Log::error << "ERROR :" << recvStr << "\n";
				}
			}
		}
	}

	void CsaClient::writeResult(Record record) {
		// 結果の保存

		// 棋譜の保存
		std::ostringstream path;
		path << config.getKifu();
		path << gameId << ".csa";
		CsaWriter::write(path.str().c_str(), record);
	}
}
