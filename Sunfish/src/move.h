/*
 * move.h
 *
 *  Created on: 2012/05/10
 *      Author: ryosuke
 */

#ifndef MOVE_H_
#define MOVE_H_

namespace Shogi {
	class Move {
	private:
		struct {
			unsigned from : 8;
			unsigned to : 8;
			unsigned promote : 1;
			unsigned hand : 1;
			unsigned piece : 4;
		} m;

	public:
		Move() {
		}

		Move(const Move& move) {
			m = move.m;
		}

		Move(unsigned from, unsigned to, bool promote,
				bool hand, unsigned piece) {
			m.from = from;
			m.to = to;
			m.promote = promote;
			m.hand = hand;
			m.piece = piece;
		}

		unsigned getFrom() const {
			return m.from;
		}

		void setFrom(unsigned from)
		{
			m.from = from;
		}
		unsigned getTo() const {
			return m.to;
		}

		void setTo(unsigned to)
		{
			m.to = to;
		}

		bool isHand() {
			return m.hand;
		}

		void setHand(bool hand) {
			m.hand = hand;
		}

		unsigned getPiece() const {
			return this->m.piece;
		}

		void setPiece(unsigned piece)
		{
			m.piece = piece;
		}

		Move& operator=(const Move& move) {
			m = move.m;
			return *this;
		}

		std::string toString() const;
	};
}

#endif /* MOVE_H_ */
