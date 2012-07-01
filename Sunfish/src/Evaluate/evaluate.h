/*
 * evaluate.h
 *
 *  Created on: 2012/07/01
 *      Author: ryosuke
 */

#ifndef EVALUATE_H_
#define EVALUATE_H_

#include "param.h"

namespace Evaluate {
	class Evaluate {
	private:
		const Param& param;
		Value baseValue;

	public:
		Evaluate(const Param& param, Value baseValue = Value(0)) :
				param(param), baseValue(baseValue) {
		}

		Evaluate(const Param& param, const Shogi::Position& pos) :
				param(param) {
			init(pos);
		}

		void init(const Shogi::Position& pos);

		void addBaseValue(const Shogi::Piece piece) {
			baseValue += param.getPiece(piece);
		}

		void subBaseValue(const Shogi::Piece piece) {
			baseValue -= param.getPiece(piece);
		}

		void addBaseValueEx(const Shogi::Piece piece) {
			baseValue += param.getPieceExchange(piece);
		}

		void subBaseValueEx(const Shogi::Piece piece) {
			baseValue -= param.getPieceExchange(piece);
		}

		void addBaseValuePr(const Shogi::Piece piece) {
			baseValue += param.getPiecePromote(piece);
		}

		void subBaseValuePr(const Shogi::Piece piece) {
			baseValue -= param.getPiecePromote(piece);
		}

		Value getBaseValue() const {
			return baseValue;
		}

		void setBaseValue(const Value& baseValue) {
			this->baseValue = baseValue;
		}

		Value getValue() const {
			return baseValue + Value(0); // TODO
		}
	};
}

#endif // EVALUATE_H_
