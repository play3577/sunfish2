/*
 * feature.h
 *
 *  Created on: 2012/07/07
 *      Author: ryosuke
 */

#ifndef FEATURE_H_
#define FEATURE_H_

#include <cassert>
#include "estimate.h"
#include "../Shogi/position.h"

namespace Evaluates {
	static const int KING_ERROR  = 100 * 24;
	static const int PIECE_ERROR = 100 *  8;

	class Feature {
	private:
		static const int kppBlackHand[8];
		static const int kppWhiteHand[8];

		static Value getKKP(const Param& p, const Kings& kings,
				const Shogi::Square& square,
				const Shogi::Piece& piece) {
			int squareIndex = square.getShortIndex();
			int pieceIndex = piece.getInteger();
			return Value(piece.isBlack() ?
				p.getKKP(kings, blackPiece[pieceIndex], squareIndex, true) :
				p.getKKP(kings, whitePiece[pieceIndex], 80 - squareIndex, false));
		}

		static void addKKP(Gradient& g, const Kings& kings,
				const Shogi::Square& square,
				const Shogi::Piece& piece, double inc) {
			int squareIndex = square.getShortIndex();
			int pieceIndex = piece.getInteger();
			if (piece.isBlack()) {
				g.addKKP(kings, blackPiece[pieceIndex], squareIndex, true, ValueF(inc));
			} else {
				g.addKKP(kings, whitePiece[pieceIndex], 80 - squareIndex, false, ValueF(inc));
			}
		}

		static int getKPPIndex(const Shogi::Square& square,
				const Shogi::Piece& piece, bool black) {
			int squareIndex = square.getShortIndex();
			int pieceIndex = piece.getInteger();
			if (black) {
				return blackPiece[pieceIndex] + KPP_KNUM * squareIndex;
			} else {
				return whitePiece[pieceIndex] + KPP_KNUM * (80 - squareIndex);
			}
		}

		template<bool get, bool cum>
		static Value extract(const Shogi::Position& pos,
				const Param* pp, Gradient* pg, double inc);

		static Value getDiffKPP(const Param& p, int king,
				const int changeList[], int changeNum,
				const int list[], int num);

		static Value getDiffKPP(const Param& p, int king,
				int x, int y) {
			if (x < y) {
				return Value(p.getKPP(king, y, x));
			} else {
				return Value(p.getKPP(king, x, y));
			}
		}

		template <bool diff>
		static int listHand(int list0[], int list1[], int num,
				const Shogi::Position& pos,
				Shogi::Piece change = Shogi::Piece::EMPTY);

		Feature();

	public:
		static Value getValue(const Shogi::Position& pos,
				const Param* pparam) {
			return extract<true, false>(pos, pparam, NULL, 0.0);
		}

		static void incValue(const Shogi::Position& pos,
				Gradient* pg, double d) {
			extract<false, true>(pos, NULL, pg, d);
		}

		static Value getDiff(const Shogi::Position& pos,
				const Param& p, Shogi::Square from,
				Shogi::Square to, Shogi::Piece fromBefore,
				Shogi::Piece toBefore, Shogi::Piece toAfter);

		static Estimate estimate(const Shogi::Position& pos,
				const Param* pparam, const Shogi::Move& move,
				int scale) {
			Value value0(0);
			Value value1(0);
			Value error(0);
			Shogi::Piece cap = pos.getBoard(move.getTo());
			if (!cap.isEmpty()) {
				value0 -= pparam->getPieceExchange(cap);
			}
			if (move.getPiece().isKing()) {
				error = KING_ERROR;
			} else {
				Kings kings(pos);
				if (!move.isHand()) {
					value1 -= pparam->getKKP(kings,
						move.getPiece(), move.getFrom());
				}
				Shogi::Piece piece = move.isPromotion() ?
					move.getPiece().getPromoted() : move.getPiece();
				value1 += pparam->getKKP(kings, piece, move.getTo());
				error = PIECE_ERROR;
			}
			return Estimate(value0 + value1 / scale, error);
		}
	};
}

#endif // FEATURE_H_
