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
		{ boost::regex("^LOGIN:.* OK$"), RECV_LOGIN_OK, NULL },
		{ boost::regex("^LOGIN:incorect$"), RECV_LOGIN_INC, NULL },
		{ boost::regex("^LOGOUT:completed$"), RECV_LOGOUT, NULL },
		{ boost::regex("^%.*"), RECV_MOVE, recvMove },
		{ boost::regex("^\\+.*"), RECV_MOVE, recvMove },
		{ boost::regex("^-.*"), RECV_MOVE, recvMove },
		{ boost::regex("^BEGIN Game_Summary$"), RECV_SUMMARY, recvGameSummary },
		/*
		{ boost::regex("^Protocol_Version:.*"), RECV_NULL, NULL },
		{ boost::regex("^Protocol_Mode:.*"), RECV_NULL, NULL },
		{ boost::regex("^Format:.*"), RECV_NULL, NULL },
		{ boost::regex("^Declaration:.*"), RECV_NULL, NULL },
		{ boost::regex("^Game_ID:.*"), RECV_NULL, NULL },
		{ boost::regex("^Name\\+:.*"), RECV_NULL, NULL },
		{ boost::regex("^Name-:.*"), RECV_NULL, NULL },
		{ boost::regex("^Your_Turn:.*"), RECV_NULL, NULL },
		{ boost::regex("^Rematch_On_Draw:.*"), RECV_NULL, NULL },
		{ boost::regex("^To_Move:.*"), RECV_NULL, NULL },
		{ boost::regex("^BEGIN Time$"), RECV_NULL, NULL },
		{ boost::regex("^Time_Unit:.*"), RECV_NULL, NULL },
		{ boost::regex("^Least_Time_Per_Move:.*"), RECV_NULL, NULL },
		{ boost::regex("^Total_Timey:.*"), RECV_NULL, NULL },
		{ boost::regex("^Byoyomi:.*"), RECV_NULL, NULL },
		{ boost::regex("^End Time$"), RECV_NULL, NULL },
		{ boost::regex("^BEGIN Position$"), RECV_NULL, NULL },
		*/
		{ boost::regex("^START:.*"), RECV_START, NULL },
		{ boost::regex("^REJECT:.* BY .*"), RECV_REJECT, NULL },
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
		init();
		con.setHost("localhost");
		con.setPort(4081);
		con.connect();
		boost::thread receiverThread(boost::bind(&CsaClient::receiver, this));
		if (!login()) {
			std::cout << "login failed!\n";
			goto lab_end;
		}
		std::cout << "login ok!!\n";

		if (!waitGameSummary()) {
			goto lab_end;
		}

lab_logout:
		logout();

lab_end:
		con.disconnect();
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
			if (recvFlags & RECV_LOGOUT) {
				return 0U;
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
					recvFlags |= flagSets[i].flag;
				}
			}
		}
	}

	void CsaClient::recvGameSummary(CsaClient* p) {
		while (p->con.receive()) {
			p->recvStr = p->con.getReceivedString();
			if (p->recvStr == "END Game_Summary") {
			}
		}
	}
}
