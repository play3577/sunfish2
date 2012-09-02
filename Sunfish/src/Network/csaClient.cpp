/*
 * csaClient.cpp
 *
 *  Created on: 2012/08/25
 *      Author: ryosuke
 */

#include "csaClient.h"
#include "../Csa/csaReader.h"

#include <boost/bind.hpp>

namespace Network {
	using namespace Shogi;
	using namespace Csa;

	const CsaClient::ReceiveFlagSet CsaClient::flagSets[RECV_NUM] = {
		{ boost::regex("^LOGIN:.* OK$"), RECV_LOGIN_OK, NULL },
		{ boost::regex("^LOGIN:incorect$"), RECV_LOGIN_INC, NULL },
		{ boost::regex("^LOGOUT:completed$"), RECV_LOGOUT, NULL },
		{ boost::regex("^%.*"), RECV_MOVE, recvMove },
		{ boost::regex("^\\+.*"), RECV_MOVE, recvMove },
		{ boost::regex("^-.*"), RECV_MOVE, recvMove },
		{ boost::regex("^BEGIN Game_Summary$"), RECV_SUMMARY, st_recvGameSummary },
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
		send("LOGIN user pass");
		unsigned result = waitReceive(RECV_LOGIN_MSK);
		if (result & RECV_LOGIN_OK) {
			return true;
		}
		return false;
	}

	bool CsaClient::logout() {
		send("LOGOUT");
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

	void CsaClient::recvGameSummary() {
		while (con.receive()) {
			recvStr = con.getReceivedString();
			printReceivedString();
			if (recvStr == "BEGIN Time") {
				recvTime();
			} else if (recvStr == "BEGIN Position") {
				recvPosition();
			} else if (recvStr == "END Game_Summary") {
				return;
			}
		}
	}

	void CsaClient::recvTime() {
		while (con.receive()) {
			recvStr = con.getReceivedString();
			printReceivedString();
			if (recvStr == "END Time") {
				return;
			}
		}
	}

	/*
	"^Protocol_Version:.*"
	"^Protocol_Mode:.*"
	"^Format:.*"
	"^Declaration:.*"
	"^Game_ID:.*"
	"^Name\\+:.*"
	"^Name-:.*"
	"^Your_Turn:.*"
	"^Rematch_On_Draw:.*"
	"^To_Move:.*"
	*/

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

	/*
	^Time_Unit:.*"
	"^Least_Time_Per_Move:.*"
	"^Total_Timey:.*"
	"^Byoyomi:.*"
	*/
}
