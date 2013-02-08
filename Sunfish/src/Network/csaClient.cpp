/*
 * csaClient.cpp
 *
 *  Created on: 2012/08/25
 *      Author: ryosuke
 */

#include "csaClient.h"
#include "../Csa/csaReader.h"
#include "../Csa/csaWriter.h"

#include <fstream>
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
		{ boost::regex("^LOGIN:.* OK$"), RECV_LOGIN_OK, NULL, NULL },
		{ boost::regex("^LOGIN:incorect$"), RECV_LOGIN_INC, NULL, NULL },
		{ boost::regex("^LOGOUT:completed$"), RECV_LOGOUT, NULL, NULL },
		{ boost::regex("^%.*"), RECV_MOVE_EX, NULL, NULL },
		{ boost::regex("^\\+.*"), RECV_MOVE_B, NULL, NULL },
		{ boost::regex("^-.*"), RECV_MOVE_W, NULL, NULL },
		{ boost::regex("^BEGIN Game_Summary$"), RECV_SUMMARY, _recvGameSummary, NULL },
		{ boost::regex("^START:.*"), RECV_START, NULL, NULL },
		{ boost::regex("^REJECT:.* by .*"), RECV_REJECT, NULL, NULL },
		{ boost::regex("^#WIN$"), RECV_WIN, NULL, "win" },
		{ boost::regex("^#LOSE$"), RECV_LOSE, NULL, "lose" },
		{ boost::regex("^#WIN\\(LOSE\\)$"), RECV_WIN_LOSE, NULL, "unknown" }, // CSA将棋付属の簡易サーバ用
		{ boost::regex("^#DRAW$"), RECV_DRAW, NULL, "draw" },
		{ boost::regex("^#CHUDAN$"), RECV_INTERRUPT, NULL, "chudan" },
		{ boost::regex("^#SENNICHITE$"), RECV_REPEAT, NULL, "sennichite" },
		{ boost::regex("^#OUTE_SENNICHITE$"), RECV_CHECK_REP, NULL, "oute sennichite" },
		{ boost::regex("^#ILLEGAL_MOVE$"), RECV_ILLEGAL, NULL, "illegal move" },
		{ boost::regex("^#TIME_UP$"), RECV_TIME_UP, NULL, "time up" },
		{ boost::regex("^#RESIGN$"), RECV_RESIGN, NULL, "resign" },
		{ boost::regex("^#JISHOGI$"), RECV_JISHOGI, NULL, "jishogi" },
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

				SearchConfig searchConfig = SearchConfig::getDefault();
				searchConfig.depth = config.getDepth();
				searchConfig.pvHandler = this;
				searchConfig.limitEnable = config.getLimit() != 0;
				searchConfig.limitSeconds = config.getLimit();
				SearchConfig searchConfigEnemy = searchConfig;
				searchConfigEnemy.depth = Tree::DEF_MAX_DEPTH; // TODO: 実際の値を取得
				searchConfigEnemy.limitEnable = false;

				while (1) {
#ifndef NDEBUG
					CsaWriter::write("debug.csa", record);
#endif
					Log::message << record.toString();
					if (black == record.getPosition().isBlackTurn()) {
						// my turn
						SearchResult result;
						searcher.setSearchConfig(searchConfig);
						searcher.init(record);
						searcher.idSearch(result);
						if (!result.resign && record.move(result.move)) {
							if (!sendMove(result)) {
								// TODO: ここでログアウトするのは危険
								Log::error << "ERROR :could not send a move\n";
								break;
							}
						} else {
							sendResign();
							break;
						}
					} else {
						// enemy's turn
						boost::thread enemyTurnSearchThread(
								boost::bind(&CsaClient::enemyTurnSearch,
								this, &searcher, record, searchConfigEnemy));
						// 相手番の指し手を受信
						unsigned mask = black ? RECV_MOVE_W : RECV_MOVE_B;
						std::string moveStr;
						unsigned flags = waitReceive(mask | RECV_END_MSK, &moveStr);
						// Searcher がスタートするか、もしくはスレッドがいなくなるまで待機
						while (true) {
							if (searcher.isRunning() || enemyTurnSearchThread.timed_join(boost::posix_time::milliseconds(1))) {
								break;
							}
						}
						// 強制中断
						searcher.forceInterrupt();
						enemyTurnSearchThread.join();
						if (flags & mask) {
							if (!CsaReader::parseLineMove(moveStr.c_str(), record)) {
								Log::error << "ERROR :illegal move!!\n";
								break;
							}
						} else if (flags & RECV_END_MSK) {
							break;
						} else {
							Log::error << "ERROR :unknown error. :" << __FILE__ << '(' << __LINE__ << ")\n";
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
		unsigned response = waitReceive(RECV_LOGIN_MSK);
		return (response & RECV_LOGIN_OK) != 0U;
	}

	bool CsaClient::logout() {
		if (!send("LOGOUT")) { return false; }
#if 0
		unsigned response = waitReceive(RECV_LOGOUT);
		return (response & RECV_LOGOUT) != 0U;
#else
		// floodgateがLOGOUT:completedを送信してくれない。
		return true;
#endif
	}

	bool CsaClient::agree() {
		if (!send("AGREE")) { return false; }
		unsigned response = waitReceive(RECV_AGREE_MSK);
		return (response & RECV_START) != 0U;
	}

	bool CsaClient::sendMove(const Search::SearchResult& result) {
		std::ostringstream oss;
		oss << result.move.toStringCsa();
		if (config.getFloodgate()) {
			// 評価値
			int sign = black ? 1 : -1;
			oss << ",\'* " << (result.value * sign);
			// 読み筋
			oss << ' ' << result.pv.toStringCsa(1);
		}
		if (!send(oss.str().c_str())) { return false; }
		unsigned mask = black ? RECV_MOVE_B : RECV_MOVE_W;
		unsigned response = waitReceive(mask | RECV_END_MSK);
		return (response & mask) != 0U;
	}

	bool CsaClient::sendResign() {
		if (!send("%TORYO")) { return false; }
		unsigned response = waitReceive(RECV_END_MSK);
		return (response & RECV_END_MSK) != 0U;
	}

	unsigned CsaClient::waitReceive(unsigned flags, std::string* str) {
		while (true) {
			{
				boost::mutex::scoped_lock lock(recvMutex);
				if (!recvQueue.empty()) {
					RECV_DATA data = recvQueue.front();
					recvQueue.pop();
					unsigned masked = data.flag & flags;
					if (masked) {
						if (str != NULL) {
							(*str) = data.str;
						}
						return masked;
					} else if (data.flag & RECV_LOGOUT) {
						return 0U;
					}
				}
			}
			sleep(20);
		}
	}

	void CsaClient::receiver() {
		while (con.receive()) {
			std::string recvStr = con.getReceivedString();
			bool ok = enqueue(recvStr);
			printReceivedString(recvStr, ok);
		}
	}

	bool CsaClient::enqueue(const std::string& recvStr) {
		for (int i = 0; i < RECV_NUM; i++) {
			if (boost::regex_match(recvStr, flagSets[i].regex)) {
				if (flagSets[i].func != NULL) {
					flagSets[i].func(this);
				}
				boost::mutex::scoped_lock lock(recvMutex);
				RECV_DATA data;
				data.flag = flagSets[i].flag;
				data.str = recvStr;
				recvQueue.push(data);
				endFlags |= flagSets[i].flag & RECV_END_MSK;
				return true;
			}
		}
		return false;
	}

	void CsaClient::recvGameSummary() {
		while (con.receive()) {
			std::string recvStr = con.getReceivedString();
			bool ok;
			if (recvStr == "BEGIN Time") {
				recvTime();
				ok = true;
			} else if (recvStr == "BEGIN Position") {
				recvPosition();
				ok = true;
			} else if (recvStr == "END Game_Summary") {
				printReceivedString(recvStr, true);
				return;
			} else {
				ok = inputGameSummary(recvStr);
			}
			printReceivedString(recvStr, ok);
		}
	}

	bool CsaClient::inputGameSummary(std::string recvStr) {
		std::vector<std::string> tokens;
		boost::algorithm::split(tokens, recvStr, boost::is_any_of(":"));
		if (tokens.size() < 2) {
			return false;
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
		} else {
			return false;
		}
		/*
			"Protocol_Version"
			"Protocol_Mode"
			"Format"
			"Declaration"
			"Rematch_On_Draw" // 自動再試合(1.1.3では無視してよい。)
			"To_Move"
		*/
		return true;
	}

	void CsaClient::recvTime() {
		while (con.receive()) {
			std::string recvStr = con.getReceivedString();
			if (recvStr == "END Time") {
				printReceivedString(recvStr, true);
				return;
			}
			bool ok = inputTime(recvStr);
			printReceivedString(recvStr, ok);
		}
	}

	bool CsaClient::inputTime(std::string recvStr) {
		std::vector<std::string> tokens;
		boost::algorithm::split(tokens, recvStr, boost::is_any_of(":"));
		if (tokens.size() < 2) {
			return false;
		}
		/*
			"Time_Unit"
			"Least_Time_Per_Move"
			"Total_Time"
			"Byoyomi"
		*/
		return true;
	}

	void CsaClient::recvPosition() {
		while (con.receive()) {
			std::string recvStr = con.getReceivedString();
			if (recvStr == "END Position") {
				printReceivedString(recvStr, true);
				return;
			}
			bool ok = inputPosition(recvStr);
			printReceivedString(recvStr, ok);
		}
	}

	bool CsaClient::inputPosition(std::string recvStr) {
		CsaReader::LineStat stat = CsaReader::parseLine(recvStr.c_str(), pos);
		return !(stat == CsaReader::LINE_ERROR);
	}

	void CsaClient::writeResult(Record record) {
		// 結果の保存
		// TODO: ファイル名を指定可能に
		std::ofstream fout("csaClient.csv", std::ios::out | std::ios::app);
		std::ostringstream endStatus;
		for (int i = 0; i < RECV_NUM; i++) {
			if (endFlags & flagSets[i].flag) {
				endStatus << flagSets[i].name << ' ';
			}
		}
		fout << gameId << ',' << blackName << ',' << whiteName << ',' << endStatus.str() << '\n';
		fout.close();

		// 棋譜の保存
		std::ostringstream path;
		path << config.getKifu();
		path << gameId << ".csa";
		CsaWriter::write(path.str().c_str(), record);
	}

	void CsaClient::enemyTurnSearch(Search::Searcher* psearcher,
			const Record& record, const SearchConfig& searchConfig) {
		if (config.getEnemy()) {
			SearchResult result;
			psearcher->setSearchConfig(searchConfig);
			psearcher->init(record);
			psearcher->idSearch(result);
		}
	}
}
