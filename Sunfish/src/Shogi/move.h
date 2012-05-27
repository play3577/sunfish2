/*
 * move.h
 *
 *  Created on: 2012/05/10
 *      Author: ryosuke
 */

#ifndef MOVE_H_
#define MOVE_H_

#include "square.h"
#include "piece.h"

namespace Shogi {
	class Move {
	private:
		struct {
			unsigned from : 8;
			unsigned to : 8;
			unsigned promote : 1;
			unsigned hand : 1;
			unsigned piece : 5;
		} m;

	public:
		Move() {
		}

		Move(const Move& move) {
			m = move.m;
		}

		Move(const Square& from, const Square& to, bool promote,
				bool hand, const Piece& piece) {
			setFrom(from);
			setTo(to);
			setPromotion(promote);
			setHand(hand);
			setPiece(piece);
		}

		Square getFrom() const {
			return Square(m.from);
		}

		void setFrom(const Square& from)
		{
			m.from = from.getIndex();
		}
		Square getTo() const {
			return Square(m.to);
		}

		void setTo(const Square& to)
		{
			m.to = to.getIndex();
		}

		bool isHand() const {
			return m.hand;
		}

		void setHand(bool hand) {
			m.hand = hand;
		}

		bool isPromotion() const {
			return m.promote;
		}

		void setPromotion(bool promote) {
			m.promote = promote;
		}

		Piece getPiece() const {
			return Piece(m.piece);
		}

		void setPiece(const Piece& piece)
		{
			m.piece = piece.getPieceNumber();
		}

		Move& operator=(const Move& move) {
			m = move.m;
			return *this;
		}

		std::string toString() const;
	};
}

#endif /* MOVE_H_ */
