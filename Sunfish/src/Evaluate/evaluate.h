/*
 * evaluate.h
 *
 *  Created on: 2012/07/01
 *      Author: ryosuke
 */

#ifndef EVALUATE_H_
#define EVALUATE_H_

#include "evaluateTable.h"
#include "feature.h"

namespace Evaluate {
	class Evaluate {
	private:
		const Param& param;
		Value baseValue;
		EvaluateTable table;

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


		Value getAdditionalValue(const Shogi::Position& pos) {
			Util::uint64 hash = pos.getHash();
			Value value;
			if (!table.get(hash, value)) {
				value = Feature::getValue<Value, ValueS, ValueS>(pos, &param);
				table.set(hash, value);
			}
			return value;
		}

		Value getValue(const Shogi::Position& pos) {
			return baseValue + getAdditionalValue(pos);
		}

		const Param& getParam() const {
			return param;
		}

		Estimate<Value> estimate(const Shogi::Position& pos, const Shogi::Move move) const {
			return Feature::estimate<Value, ValueS, ValueS>(pos, &param, move);
		}
	};
}

#endif // EVALUATE_H_
