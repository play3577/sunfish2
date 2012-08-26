/*
 * csaClient.cpp
 *
 *  Created on: 2012/08/25
 *      Author: ryosuke
 */

#include "csaClient.h"

#include <boost/bind.hpp>

namespace Network {
	const CsaClient::ReceiveFlagSet CsaClient::flagSets[RECV_NUM] = {
		{ boost::regex("^LOGIN:.* OK$"), RECV_LOGIN_OK },
		{ boost::regex("^LOGIN:incorect$"), RECV_LOGIN_INC },
		{ boost::regex("^LOGOUT:completed$"), RECV_LOGOUT },
	};

	bool CsaClient::execute() {
		init();
		con.setHost("localhost");
		con.setPort(4081);
		con.connect();
		boost::thread receiverThread(boost::bind(&CsaClient::receiver, this));
		if (!login()) {
			std::cout << "login failed!\n";
			interrupt = true;
			receiverThread.join();
			return false;
		}
		std::cout << "login ok!!\n";
		logout();
		interrupt = true;
		receiverThread.join();
		return true;
	}

	bool CsaClient::login() {
		con.send("LOGIN user pass");
		unsigned result = waitReceive(RECV_LOGIN_MSK);
		if (result & RECV_LOGIN_OK) {
			return true;
		}
		return false;
	}

	bool CsaClient::logout() {
		con.send("LOGOUT");
#if 0
		unsigned result = waitReceive(RECV_LOGOUT);
		if (result & RECV_LOGOUT) {
			return true;
		}
		return false;
#else
		// floodgateがLOGOUT:completedを送信してくれない。
		return true;
#endif
	}

	unsigned CsaClient::waitReceive(unsigned flags) {
		while (true) {
			unsigned masked = recvFlags & flags;
			if (masked) {
				recvFlags &= ~masked;
				return masked;
			}
			sleep(20);
		}
	}

	void CsaClient::receiver() {
		while (!interrupt && con.receive()) {
			std::string rec = con.getReceivedString();
			std::cout << "\x1b[35m";
			std::cout << '>' << rec << std::endl;
			std::cout << "\x1b[39m";
			for (int i = 0; i < RECV_NUM; i++) {
				if (boost::regex_match(rec, flagSets[i].regex)) {
					recvFlags |= flagSets[i].flag;
				}
			}
		}
	}
}
