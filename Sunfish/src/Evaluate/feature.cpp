/*
 * feature.cpp
 *
 *  Created on: 2012/07/07
 *      Author: ryosuke
 */

#include "feature.h"
#include "../Tools/debug.h"

namespace Evaluate {
	using namespace Shogi;

	template<class X, class T, class U, bool get, bool cum>
	void Feature::extract(const Shogi::Position& pos,
			const TempParam<T, U>* iparam, const X* ivalue,
			TempParam<T, U>* oparam, X* ovalue) {
		int num = 0;
		int list0[64];
		int list1[64];
		int list2[64];

		Kings kings(pos);

		for (Square square = Square::TOP; square.valid(); square.next()) {
			Piece piece = pos.getBoard(square);
			if (!piece.isEmpty()) {
				if (!piece.isKing()) {
					int squareIndex = square.getShortIndex();
					int squareIndexR = 80 - squareIndex;
					int pieceIndex = piece.getInteger();
					int blackIndex = blackPiece[pieceIndex];
					int whiteIndex = whitePiece[pieceIndex];
					if (get) {
						*ovalue += piece.isBlack() ?
							iparam->getKKP(kings, blackIndex, squareIndex, true) :
							iparam->getKKP(kings, whiteIndex, squareIndexR, false);
					}
					if (cum) {
					}
					list0[num] = blackIndex + KPP_KNUM * squareIndex;
					list2[num] = whiteIndex + KPP_KNUM * squareIndexR;
					num++;
				}
			}
		}

		for (int i = num - 1; i >= 0; i--) {
			list1[num-i-1] = list2[i];
		}

		list0[num] = KPP_DAI_SFU + pos.getBlackHand(Piece::PAWN);
		list1[num] = KPP_DAI_SFU + pos.getWhiteHand(Piece::PAWN);
		num++;
		list0[num] = KPP_DAI_SKY + pos.getBlackHand(Piece::LANCE);
		list1[num] = KPP_DAI_SKY + pos.getWhiteHand(Piece::LANCE);
		num++;
		list0[num] = KPP_DAI_SKE + pos.getBlackHand(Piece::KNIGHT);
		list1[num] = KPP_DAI_SKE + pos.getWhiteHand(Piece::KNIGHT);
		num++;
		list0[num] = KPP_DAI_SGI + pos.getBlackHand(Piece::SILVER);
		list1[num] = KPP_DAI_SGI + pos.getWhiteHand(Piece::SILVER);
		num++;
		list0[num] = KPP_DAI_SKI + pos.getBlackHand(Piece::GOLD);
		list1[num] = KPP_DAI_SKI + pos.getWhiteHand(Piece::GOLD);
		num++;
		list0[num] = KPP_DAI_SKA + pos.getBlackHand(Piece::BISHOP);
		list1[num] = KPP_DAI_SKA + pos.getWhiteHand(Piece::BISHOP);
		num++;
		list0[num] = KPP_DAI_SHI + pos.getBlackHand(Piece::ROOK);
		list1[num] = KPP_DAI_SHI + pos.getWhiteHand(Piece::ROOK);
		num++;

		list0[num] = KPP_DAI_GFU + pos.getWhiteHand(Piece::PAWN);
		list1[num] = KPP_DAI_GFU + pos.getBlackHand(Piece::PAWN);
		num++;
		list0[num] = KPP_DAI_GKY + pos.getWhiteHand(Piece::LANCE);
		list1[num] = KPP_DAI_GKY + pos.getBlackHand(Piece::LANCE);
		num++;
		list0[num] = KPP_DAI_GKE + pos.getWhiteHand(Piece::KNIGHT);
		list1[num] = KPP_DAI_GKE + pos.getBlackHand(Piece::KNIGHT);
		num++;
		list0[num] = KPP_DAI_GGI + pos.getWhiteHand(Piece::SILVER);
		list1[num] = KPP_DAI_GGI + pos.getBlackHand(Piece::SILVER);
		num++;
		list0[num] = KPP_DAI_GKI + pos.getWhiteHand(Piece::GOLD);
		list1[num] = KPP_DAI_GKI + pos.getBlackHand(Piece::GOLD);
		num++;
		list0[num] = KPP_DAI_GKA + pos.getWhiteHand(Piece::BISHOP);
		list1[num] = KPP_DAI_GKA + pos.getBlackHand(Piece::BISHOP);
		num++;
		list0[num] = KPP_DAI_GHI + pos.getWhiteHand(Piece::ROOK);
		list1[num] = KPP_DAI_GHI + pos.getBlackHand(Piece::ROOK);
		num++;

		// King Piece Piece
		for (int i = 0; i < num; i++) {
			int x = list0[i];
			int y = list1[i];
			for(int j = 0; j <= i; j++) {
				if (get) {
					*ovalue += Value(iparam->getKPP(
						kings.getBlack(), x, list0[j]));
					*ovalue -= Value(iparam->getKPP(
						kings.getBlackR(), y, list1[j]));
				}
				if (cum) {
				}
			}
		}
	}
	template void Feature::extract<Value, ValueS, ValueS, true, false>
			(const Shogi::Position& pos,
			const Param* iparam, const Value* ivalue,
			Param* oparam, Value* ovalue);
}
