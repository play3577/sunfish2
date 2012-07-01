/*
 * param.h
 *
 *  Created on: 2012/07/01
 *      Author: ryosuke
 */

#ifndef PARAM_H_
#define PARAM_H_

#include "../Shogi/position.h"
#include "value.h"

namespace Evaluate {
	template <class T, class U>
	class TempParam {
	private:
		T piece[Shogi::Piece::DRAGON+1];
		T pieceEx[Shogi::Piece::DRAGON+1];
		T piecePr[Shogi::Piece::DRAGON+1];

	public:
		TempParam() {
			init();
		}

		virtual ~TempParam() {
		}

		void init() {
			memset(this, 0, sizeof(*this));
		}

		void setPiece(const Shogi::Piece& p, T value) {
			Shogi::Piece bp = p.getTurnedBlack();
			piece[bp] = value;
			Shogi::Piece up = bp.getUnPromoted();
			Shogi::Piece pp = bp.getPromoted();
			pieceEx[up] = piece[up] * 2;
			pieceEx[pp] = piece[up] + piece[pp];
			piecePr[up] = piecePr[pp] = piece[pp] - piece[up];
		}

		T getPiece(const Shogi::Piece& p) const {
			if (!p.isWhite()) {
				return  piece[p];
			} else {
				return -piece[p.getTurnedBlack()];
			}
		}

		T getPieceExchange(const Shogi::Piece& p) const {
			if (!p.isWhite()) {
				return  pieceEx[p];
			} else {
				return -pieceEx[p.getTurnedBlack()];
			}
		}

		T getPiecePromote(const Shogi::Piece& p) const {
			if (!p.isWhite()) {
				return  piecePr[p];
			} else {
				return -piecePr[p.getTurnedBlack()];
			}
		}
	};

	typedef TempParam<Value, ValueS> Param;
	typedef TempParam<ValueF, ValueF> Gradient;
};

#endif // PARAM_H_
