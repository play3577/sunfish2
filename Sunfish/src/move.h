/*
 * move.h
 *
 *  Created on: 2012/05/10
 *      Author: ryosuke
 */

#ifndef MOVE_H_
#define MOVE_H_

class Move {
private:
	struct {
		unsigned from : 8;
		unsigned to : 8;
		unsigned nari : 1;
		unsigned dai : 1;
		unsigned koma : 4;
	} m;

public:
	Move() {
	}

	Move(const Move& move) {
		m = move.m;
	}

	unsigned getFrom() const {
		return m.from;
	}

	void setFrom(unsigned from)
	{
		m.from = from;
	}

	unsigned getKoma() const {
		return this->m.koma;
	}

	void setKoma(unsigned koma)
	{
		m.koma = koma;
	}

	unsigned getTo() const {
		return m.to;
	}

	void setTo(unsigned to)
	{
		m.to = to;
	}
};

#endif /* MOVE_H_ */
