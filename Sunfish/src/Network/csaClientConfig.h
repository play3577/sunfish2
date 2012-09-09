/*
 * csaClientConfig.h
 *
 *  Created on: 2012/09/08
 *      Author: ryosuke
 */

#ifndef CSACLIENTCONFIG_H_
#define CSACLIENTCONFIG_H_

#include <string>

namespace Network {
	class CsaClientConfig {
	private:
		static const int LINE_BUFFER_SIZE = 1024;
		std::string host;
		int port;
		std::string user;
		std::string pass;

		int depth;
		int limit;
		int repeat;

		int keepalive;
		int keepidle;
		int keepintvl;
		int keepcnt;

		bool readLine(const char* line);

	public:
		CsaClientConfig(const char* filename = NULL) {
			if (filename != NULL) {
				read(filename);
			}
		}

		bool read(const char* filename);

		const std::string& getHost() const {
			return host;
		}

		int getPort() const {
			return port;
		}

		const std::string& getUser() const {
			return user;
		}

		const std::string& getPass() const {
			return pass;
		}

		int getDepth() const {
			return depth;
		}

		int getLimit() const {
			return limit;
		}

		int getRepeat() const {
			return repeat;
		}

		int getKeepalive() const {
			return keepalive;
		}

		int getKeepidle() const {
			return keepidle;
		}

		int getKeepintvl() const {
			return keepintvl;
		}

		int getKeepcnt() const {
			return keepcnt;
		}
	};
}

#endif // CSACLIENTCONFIG_H_