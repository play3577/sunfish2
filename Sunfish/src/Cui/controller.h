/*
 * controller.h
 *
 *  Created on: 2012/07/01
 *      Author: ryosuke
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

namespace Cui {
	class Controller {
	private:
		enum Command {
			UNKNOWN = 0,
			PREV,
			NEXT,
		};

		Command inputCommand(const char* str);

	public:
		bool play();
	};
}

#endif // CONTROLLER_H_
