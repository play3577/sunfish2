/*
 * csaClient.h
 *
 *  Created on: 2012/08/25
 *      Author: ryosuke
 */

#ifndef CSACLIENT_H_
#define CSACLIENT_H_

#include "connection.h"
#define BOOST_THREAD_USE_LIB
#include <boost/thread.hpp>
#define BOOST_REGEX_USE_LIB
#include <boost/regex.hpp>

namespace Network {
	class CsaClient {
	private:
		enum RECV_FLAG {
			RECV_NULL      = 0x0000,

			RECV_LOGIN_OK  = 0x0001,
			RECV_LOGIN_INC = 0x0002,
			RECV_LOGIN_MSK = RECV_LOGIN_OK | RECV_LOGIN_INC,

			RECV_LOGOUT    = 0x0004,

			RECV_ERROR     = 0x8000,

			RECV_NUM       = 3,
		};
		unsigned recvFlags;

		struct ReceiveFlagSet {
			boost::regex regex;
			RECV_FLAG flag;
		};
		static const ReceiveFlagSet flagSets[RECV_NUM];

		Connection con;

		bool interrupt;

		void receiver();

		bool login();

		bool logout();

		void init() {
			interrupt = false;
			recvFlags = RECV_NULL;
		}

		unsigned waitReceive(unsigned flags);

		void sleep(unsigned msec) {
			boost::thread::sleep(boost::get_system_time()
				+ boost::posix_time::milliseconds(msec));
		}

	public:
		CsaClient() {
		}

		bool execute();
	};
}

#endif // CSACLIENT_H_
