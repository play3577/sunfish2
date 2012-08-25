/*
 * connection.h
 *
 *  Created on: 2012/08/25
 *      Author: ryosuke
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <queue>
#include <string>

#ifdef WIN32
#	include <process.h>
#	include <mmsystem.h>
#	include <csignal>
#	include <ctime>
#else
#	include <unistd.h>
#	include <csignal>
#	include <dirent.h>
#	include <strings.h>
#	include <sched.h>
#	include <sys/types.h>
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <netinet/tcp.h>
#	include <netdb.h>
#	include <errno.h>
#endif

namespace Network {
	class Connection {
	private:
		std::string host;
		int port;
#ifdef WIN32
		SOCKET sock;
#else
		int sock;
#endif
		std::queue<std::string> received;
		int keepalive;
		int keepidle;
		int keepintvl;
		int keepcnt;

		void initKeepAlive() {
			keepalive = 0;
			keepidle = 7200;
			keepintvl = 75;
			keepcnt = 3;
		}

	public:
		Connection() {
		}

		Connection(const std::string& host, int port)
				: host(host), port(port) {
		}

		void setHost(const std::string& host) {
			this->host = host;
		}

		void setHost(int port) {
			this->port = port;
		}

		bool connect();

		void disconnect();

		bool receive();

		std::string getReceivedString() {
			std::string str = received.front();
			received.pop();
			return str;
		}

		bool send(const std::string& str);
	};
}

#endif // CONNECTION_H_
