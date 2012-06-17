/*
 * change.cpp
 *
 *  Created on: 2012/06/16
 *      Author: ryosuke
 */

#ifndef CHANGE_H_
#define CHANGE_H_

#include "square.h"
#include "piece.h"

namespace Shogi {
	class Change {
	private:
		struct {
			unsigned type : 2;
			unsigned handPiece : 3;
			unsigned handNum : 5;
			//unsigned handEnemyPiece : 3;
			//unsigned handEnemyNum : 5;
			unsigned fromSquare : 8;
			unsigned fromPiece : 5;
			unsigned toSquare : 8;
			unsigned toPiece : 5;
		} c;

	public:
		enum {
			NULL_MOVE = 0,
			DROP,
			NO_CAPTURE,
			CAPTURE,
		};

		Change() {
		}

		Change(const Change& change) {
			c = change.c;
		}

		unsigned getType() {
			return c.type;
		}

		void setType(unsigned type) {
			c.type = type;
		}

		Piece getHandPiece() const {
			return Piece(c.handPiece);
		}

		void setHandPiece(const Piece& piece) {
			c.handPiece = piece.getTurnedBlack().getInteger();
		}

		unsigned getHandNum() const {
			return c.handNum;
		}

		void setHandNum(int num) {
			c.handNum = num;
		}

/*
		Piece getHandEnemyPiece() const {
			return Piece(c.handEnemyPiece);
		}

		void setHandEnemyPiece(const Piece& piece) {
			c.handEnemyPiece = piece.getTurnedBlack().getInteger();
		}

		unsigned getHandEnemyNum() const {
			return c.handEnemyNum;
		}

		void setHandEnemyNum(int num) {
			c.handEnemyNum = num;
		}
*/

		Square getFromSquare() const {
			return Square(c.fromSquare);
		}

		void setFromSquare(const Square& square) {
			c.fromSquare = square.getIndex();
		}

		Piece getFromPiece() const {
			return Piece(c.fromPiece);
		}

		void setFromPiece(const Piece& piece) {
			c.fromPiece = piece.getInteger();
		}

		Square getToSquare() const {
			return Square(c.toSquare);
		}

		void setToSquare(const Square& square) {
			c.toSquare = square.getIndex();
		}

		Piece getToPiece() const {
			return Piece(c.toPiece);
		}

		void setToPiece(const Piece& piece) {
			c.toPiece = piece.getInteger();
		}
	};
}

#endif // CHANGE_H_
