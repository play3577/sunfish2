/*
 * csaClient.h
 *
 *  Created on: 2012/08/25
 *      Author: ryosuke
 */

#ifndef CSACLIENT_H_
#define CSACLIENT_H_

#include "csaClientConfig.h"
#include "../Log/logger.h"
#include "../Evaluates/param.h"
#include "../Evaluates/initializer.h"
#include "../Records/record.h"
#include "../Search/searcher.h"
#include "connection.h"
#include <iomanip>
#include <queue>
#define BOOST_THREAD_USE_LIB
#include <boost/thread.hpp>
#define BOOST_REGEX_USE_LIB
#include <boost/regex.hpp>

namespace Network {
	class CsaClient : public Search::PvHandler {
	private:
		boost::mutex recvMutex;

		enum RECV_FLAG {
			RECV_NULL      = 0x000000,

			RECV_LOGIN_OK  = 0x000001,
			RECV_LOGIN_INC = 0x000002,
			RECV_LOGIN_MSK = RECV_LOGIN_OK | RECV_LOGIN_INC,

			RECV_LOGOUT    = 0x000004,

			RECV_MOVE_B    = 0x000008,
			RECV_MOVE_W    = 0x000010,
			RECV_MOVE_EX   = 0x000020,

			RECV_SUMMARY   = 0x000040,

			RECV_START     = 0x000080,
			RECV_REJECT    = 0x000100,
			RECV_AGREE_MSK = RECV_START | RECV_REJECT,

			RECV_WIN       = 0x000200,
			RECV_LOSE      = 0x000400,
			RECV_DRAW      = 0x000800,
			RECV_INTERRUPT = 0x001000,
			RECV_REPEAT    = 0x002000,
			RECV_CHECK_REP = 0x004000,
			RECV_ILLEGAL   = 0x008000,
			RECV_TIME_UP   = 0x010000,
			RECV_RESIGN    = 0x020000,
			RECV_JISHOGI   = 0x040000,

			RECV_WIN_LOSE  = RECV_WIN | RECV_LOSE, // CSA将棋付属の簡易サーバ用

			RECV_END_MSK   = RECV_WIN | RECV_LOSE
					| RECV_DRAW | RECV_INTERRUPT
					//| RECV_REPEAT | RECV_CHECK_REP
					//| RECV_ILLEGAL | RECV_TIME_UP
					//| RECV_RESIGN | RECV_JISHOGI
					,

			RECV_ERROR     = 0x800000,

			RECV_NUM       = 20,
		};

		struct RECV_DATA {
			unsigned flag;
			std::string str;
		};

		std::queue<RECV_DATA> recvQueue;
		unsigned endFlags;

		struct ReceiveFlagSet {
			boost::regex regex;
			RECV_FLAG flag;
			void (*func)(CsaClient*);
			const char* name;
		};
		static const ReceiveFlagSet flagSets[RECV_NUM];

		const char* configFilename;
		CsaClientConfig config;

		Shogi::Position pos;
		Evaluates::Param* pparam;

		Connection con; // コネクション

		bool black; // 自分の手番が黒か
		std::string gameId; // 対局ID
		std::string blackName; // 先手の名前
		std::string whiteName; // 後手の名前

		void receiver();

		bool enqueue(const std::string& recvStr);

		bool login();

		bool logout();

		bool agree();

		bool sendMove(const Search::SearchResult& result);

		bool sendResign();

		bool send(const char* str) {
			printSentString(str);
			return con.sendln(str);
		}

		bool waitGameSummary() {
			return waitReceive(RECV_SUMMARY) == RECV_SUMMARY;
		}

		void init() {
			while (!recvQueue.empty()) {
				recvQueue.pop();
			}
			endFlags = RECV_NULL;
			gameId = "";
			blackName = "";
			whiteName = "";
		}

		unsigned waitReceive(unsigned flags, std::string* str = NULL);

		static void _recvGameSummary(CsaClient* p) {
			p->recvGameSummary();
		}

		void recvGameSummary();

		bool inputGameSummary(std::string recvStr);

		void recvTime();

		bool inputTime(std::string recvStr);

		void recvPosition();

		bool inputPosition(std::string recvStr);

		void sleep(unsigned msec) {
			boost::thread::sleep(boost::get_system_time()
				+ boost::posix_time::milliseconds(msec));
		}

		void printSentString(const char* str) {
			Log::send << '<' << str << '\n';
		}

		void printReceivedString(std::string recvStr, bool ok) {
			Log::receive << '>' << recvStr;
			if (!ok) {
				Log::receive << " ..!!!parsing error!!!";
			}
			Log::receive << '\n';
		}

		void writeResult(Records::Record record);

		void enemyTurnSearch(Search::Searcher* psearcher,
			const Records::Record& record,
			const Search::SearchConfig& searchConfig);

	public:
		static const char* DEFAULT_CONFIG_FILE;

		CsaClient() {
			pparam = new Evaluates::Param();
			Evaluates::Initializer::apply(*pparam);
			pparam->read("evdata");
			configFilename = DEFAULT_CONFIG_FILE;
		}

		~CsaClient() {
			delete pparam;
		}

		void setConfigFile(const char* filename) {
			configFilename = filename;
		}

		bool execute();

		void pvHandler(const Search::Pv& pv, Evaluates::Value value,
				Util::uint64 nodes, int depth, double seconds) {
			Log::message << std::setw(2) << depth << std::setw(10) << nodes
					<< ' ' << pv.toString() << ':' << value
					<< " (" << seconds << "sec)\n";
		}
	};
}

#endif // CSACLIENT_H_
