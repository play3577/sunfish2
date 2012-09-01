/*
 * csaClient.h
 *
 *  Created on: 2012/08/25
 *      Author: ryosuke
 */

#ifndef CSACLIENT_H_
#define CSACLIENT_H_

#include "../Log/logger.h"
#include "connection.h"
#define BOOST_THREAD_USE_LIB
#include <boost/thread.hpp>
#define BOOST_REGEX_USE_LIB
#include <boost/regex.hpp>

namespace Network {
	class CsaClient {
	private:
		enum RECV_FLAG {
			RECV_NULL      = 0x000000,

			RECV_LOGIN_OK  = 0x000001,
			RECV_LOGIN_INC = 0x000002,
			RECV_LOGIN_MSK = RECV_LOGIN_OK | RECV_LOGIN_INC,

			RECV_LOGOUT    = 0x000004,

			RECV_MOVE      = 0x000008,

			RECV_SUMMARY   = 0x000010,

			RECV_START     = 0x000020,
			RECV_REJECT    = 0x000040,

			RECV_WIN       = 0x000080,
			RECV_LOSE      = 0x000100,
			RECV_DRAW      = 0x000200,
			RECV_INTERRUPT = 0x000400,
			RECV_REPEAT    = 0x000800,
			RECV_CHECK_REP = 0x001000,
			RECV_ILLEGAL   = 0x002000,
			RECV_TIME_UP   = 0x004000,
			RECV_RESIGN    = 0x008000,
			RECV_JISHOGI   = 0x010000,

			RECV_ERROR     = 0x800000,

			RECV_NUM       = 19,
		};
		unsigned recvFlags;

		struct ReceiveFlagSet {
			boost::regex regex;
			RECV_FLAG flag;
			void (*func)(CsaClient*);
		};
		static const ReceiveFlagSet flagSets[RECV_NUM];

		Connection con;

		std::string recvStr;

		std::string moveStr;

		void receiver();

		bool login();

		bool logout();

		bool waitGameSummary() {
			return waitReceive(RECV_SUMMARY) == RECV_SUMMARY;
		}

		void init() {
			recvFlags = RECV_NULL;
		}

		unsigned waitReceive(unsigned flags);

		static void recvMove(CsaClient* p) {
			p->moveStr = p->recvStr;
		}

		static void recvGameSummary(CsaClient* p);

		void sleep(unsigned msec) {
			boost::thread::sleep(boost::get_system_time()
				+ boost::posix_time::milliseconds(msec));
		}

		void printReceivedString() {
			Log::message << "\x1b[35m"
					<< '>' << recvStr
					<< "\x1b[39m" << '\n';
		}

	public:
		CsaClient() {
		}

		bool execute();
	};
}

#endif // CSACLIENT_H_
