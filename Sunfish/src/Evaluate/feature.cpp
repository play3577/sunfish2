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

	const int Feature::blackPiece[] = {
		 0,
//		歩  香  桂  銀  金  角  飛  王
		 0,  1,  2,  3,  4,  5,  6,  0,
		 4,  4,  4,  4,  0,  7,  8,  0,
//		歩  香  桂  銀  金  角  飛  王
		 9, 10, 11, 12, 13, 14, 15,  0,
		13, 13, 13, 13,  0, 16, 17,
	};

	const int Feature::whitePiece[] = {
		 0,
//		歩  香  桂  銀  金  角  飛  王
		 9, 10, 11, 12, 13, 14, 15,  0,
		13, 13, 13, 13,  0, 16, 17,  0,
//		歩  香  桂  銀  金  角  飛  王
		 0,  1,  2,  3,  4,  5,  6,  0,
		 4,  4,  4,  4,  0,  7,  8,
	};

	template<class X, class T, class U, bool get, bool cum>
	void Feature::Extract(const Shogi::Position& pos,
			const TempParam<T, U>* iparam, const X* ivalue,
			TempParam<T, U>* oparam, X* ovalue) {
		int num = 0;
		int list0[64];
		int list1[64];
		int list2[64];

		int bking = pos.getBKing().getShortIndex();
		int wking = pos.getWKing().getShortIndex();
		int bking2 = 80 - wking;
		int wking2 = 80 - bking;

		for (Square square = Square::TOP; square.valid(); square.next()) {
			Piece piece = pos.getBoard(square);
			if (!piece.isEmpty()) {
				if (piece.isBlack()) {
					if (piece != Piece::BKING) {
						int squareIndex = square.getShortIndex();
						int pieceIndex = piece.getInteger();
						list0[num] = blackPiece[pieceIndex] + KPP_KNUM * squareIndex;
						list2[num] = whitePiece[pieceIndex] + KPP_KNUM * (80-squareIndex);
						num++;
						if (get) {
							*ovalue += Value(iparam->getKKP(bking, wking, blackPiece[pieceIndex]+KKP_KNUM*squareIndex));
						}
						if (cum) {
						}
					}
				} else {
					if (piece != Piece::WKING) {
						int squareIndex = square.getShortIndex();
						int pieceIndex = piece.getInteger();
						list0[num] = blackPiece[pieceIndex] + KPP_KNUM * squareIndex;
						list2[num] = whitePiece[pieceIndex] + KPP_KNUM * (80-squareIndex);
						num++;
						if (get) {
							*ovalue -= Value(iparam->getKKP(bking2, wking2, whitePiece[pieceIndex]+KKP_KNUM*(80-squareIndex)));
						}
						if (cum) {
						}
					}
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
			int x0 = list0[i];
			int x1 = list1[i];
			for(int j = 0; j <= i; j++) {
				int y0 = list0[j];
				int y1 = list1[j];
				if (get) {
					*ovalue += Value(iparam->getKPP(bking, x0*(x0+1)/2+y0));
					*ovalue -= Value(iparam->getKPP(bking2, x1*(x1+1)/2+y1));
				}
				if (cum) {
				}
			}
		}
	}
	template void Feature::Extract<Value, ValueS, ValueS, true, false>
			(const Shogi::Position& pos,
			const Param* iparam, const Value* ivalue,
			Param* oparam, Value* ovalue);
}
