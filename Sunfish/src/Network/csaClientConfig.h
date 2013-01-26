/*
 * csaClientConfig.h
 *
 *  Created on: 2012/09/08
 *      Author: ryosuke
 */

#ifndef CSACLIENTCONFIG_H_
#define CSACLIENTCONFIG_H_

#include <string>
#include <typeinfo>

namespace Network {
	enum ItemType {
		BOOL,
		INTEGER,
		STRING,
	};

	struct ConfigItem {
		const char* name;
		ItemType type;
		void* data;
		ConfigItem() {
		}
		ConfigItem(const char* name,
				ItemType type, void* data) {
			this->name = name;
			this->type = type;
			this->data = data;
		}
	};

	class CsaClientConfig {
	private:
		static const int LINE_BUFFER_SIZE = 1024;

		ConfigItem items[14];

		std::string host;
		int port;
		std::string user;
		std::string pass;

		int depth;
		int limit;
		int repeat;
		bool enemy;

		int keepalive;
		int keepidle;
		int keepintvl;
		int keepcnt;

		bool floodgate;

		std::string kifu;

		bool readLine(const char* line);

	public:
		CsaClientConfig(const char* filename = NULL);
		
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

		bool getEnemy() const {
			return enemy;
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

		bool getFloodgate() const {
			return floodgate;
		}

		std::string getKifu() const {
			if (kifu.length() > 0 && kifu[kifu.length()-1] == '/') {
				return kifu;
			} else {
				return kifu + '/';
			}
		}
	};
}

#endif // CSACLIENTCONFIG_H_
