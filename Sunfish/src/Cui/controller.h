/*
 * controller.h
 *
 *  Created on: 2012/07/01
 *      Author: ryosuke
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "../Shogi/position.h"
#include "../Evaluate/param.h"
#include "../Evaluate/initializer.h"

namespace Cui {
	class Controller {
	private:
		enum Command {
			UNKNOWN = 0,
			PREV,
			NEXT,
			MOVES,
			SEARCH,
			CAPTURES,
			NOCAPTURES,
		};

		Command inputCommand(const char* str);

		Evaluate::Param* pparam;

	public:
		Controller() {
			pparam = new Evaluate::Param();
			Evaluate::Initializer::apply(*pparam);
			pparam->read("evdata");
		}

		virtual ~Controller() {
			delete pparam;
		}

		void showLegalMoves(Shogi::Position pos);

		void showCaptures(Shogi::Position pos);

		void showNoCaptures(Shogi::Position pos);

		bool play();
	};
}

#endif // CONTROLLER_H_
