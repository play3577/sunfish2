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
	extern const int sym[];

	enum{
		// King Piece Piece
		KPP_KNUM    = 18,
	
		KPP_BAN = 0,
		KPP_DAI_SFU = KPP_BAN     + 81 * KPP_KNUM,
		KPP_DAI_SKY = KPP_DAI_SFU + 19,
		KPP_DAI_SKE = KPP_DAI_SKY + 5,
		KPP_DAI_SGI = KPP_DAI_SKE + 5,
		KPP_DAI_SKI = KPP_DAI_SGI + 5,
		KPP_DAI_SKA = KPP_DAI_SKI + 5,
		KPP_DAI_SHI = KPP_DAI_SKA + 3,
	
		KPP_DAI_GFU = KPP_DAI_SHI + 3,
		KPP_DAI_GKY = KPP_DAI_GFU + 19,
		KPP_DAI_GKE = KPP_DAI_GKY + 5,
		KPP_DAI_GGI = KPP_DAI_GKE + 5,
		KPP_DAI_GKI = KPP_DAI_GGI + 5,
		KPP_DAI_GKA = KPP_DAI_GKI + 5,
		KPP_DAI_GHI = KPP_DAI_GKA + 3,
	
		KPP_ALL     = KPP_DAI_GHI + 3,
	
		KPP_MAX     = KPP_ALL*(KPP_ALL+1)/2,
	
		// King King Piece
		KKP_KNUM    = 9,
	
		KKP_BAN     = 0,
	
		KKP_DAI_FU  = KKP_BAN    + 81 * KKP_KNUM,
		KKP_DAI_KY  = KKP_DAI_FU + 19,
		KKP_DAI_KE  = KKP_DAI_KY + 5,
		KKP_DAI_GI  = KKP_DAI_KE + 5,
		KKP_DAI_KI  = KKP_DAI_GI + 5,
		KKP_DAI_KA  = KKP_DAI_KI + 5,
		KKP_DAI_HI  = KKP_DAI_KA + 3,
	
		KKP_ALL     = KKP_DAI_HI + 3,
	};

	template <class T, class U>
	class TempParam {
	private:
		typedef TempParam<T, U> Type;

		static const double EV_SCALE = 32.0;

		T piece[Shogi::Piece::DRAGON+1];
		T pieceEx[Shogi::Piece::DRAGON+1];
		T piecePr[Shogi::Piece::DRAGON+1];

		// King Piece Piece
		U kpp[81][KPP_MAX];

		// King King Piece
		U kkp[81][81][KKP_ALL];

		template <bool copy>
		void symmetry() {
			int x0, y0, z0;
			int x1, y1, z1;

			for( x0 = 0 ; x0 < 81 ; x0++ ){
				x1 = sym[x0]; // 左右反転
				if( x0 > x1 ) { continue; }

				// King Piece Piece
				for( y0 = 0 ; y0 < KPP_ALL ; y0++ ){
					if( y0 < KPP_DAI_SFU ){
						// 盤上は左右反転
						y1 = sym[y0/KPP_KNUM] * KPP_KNUM + ( y0 % KPP_KNUM );
						if( x0 == x1 && y0 > y1 ) { continue; }
					}
					else{ y1 = y0; } // 駒台はそのまま
					for( z0 = 0 ; z0 <= y0 ; z0++ ){
						if( z0 < KPP_DAI_SFU ){
							// 盤上は左右反転
							z1 = sym[z0/KPP_KNUM] * KPP_KNUM + ( z0 % KPP_KNUM );
							if( x0 == x1 && y0 == y1 && z0 >= z1 ) {
								continue;
							}
						}
						else{ z1 = z0; } // 駒台はそのまま
						if( y1 >= z1 ) {
							int i0 = y0*(y0+1)/2+z0;
							int i1 = y1*(y1+1)/2+z1;
							if (copy) {
								kpp[x0][i0] = kpp[x1][i1];
							} else {
								kpp[x0][i0] = kpp[x1][i1] = kpp[x0][i0] + kpp[x1][i1];
							}
						} else {
							int i0 = y0*(y0+1)/2+z0;
							int i1 = z1*(z1+1)/2+y1;
							if (copy) {
								kpp[x0][i0] = kpp[x1][i1];
							} else {
								kpp[x0][i0] = kpp[x1][i1] = kpp[x0][i0] + kpp[x1][i1];
							}
						}
					}
				}

				// King King Piece
				for( y0 = 0 ; y0 < 81 ; y0++ ){
					if( y0 == x0 ) { continue; }
					y1 = sym[y0];
					if( x0 == x1 && y0 > y1 ) { continue; }
					for( z0 = 0 ; z0 < KKP_ALL ; z0++ ){
						if( z0 < KKP_DAI_FU ){
							// 盤上は左右反転
							z1 = sym[z0/KKP_KNUM] * KKP_KNUM + ( z0 % KKP_KNUM );
							if( x0 == x1 && y0 == y1 && z0 >= z1 ) {
								continue;
							}
						}
						else{
							// 駒台はそのまま
							if( x0 == x1 && y0 == y1 ) {
								continue;
							}
							z1 = z0;
						}
						if (copy) {
							kkp[x0][y0][z0] = kkp[x1][y1][z1];
						} else {
							kkp[x0][y0][z0] = kkp[x1][y1][z1] = kkp[x0][y0][z0] + kkp[x1][y1][z1];
						}
					}
				}
			}
		}

	public:
		TempParam() {
			init();
		}

		virtual ~TempParam() {
		}

		void init() {
			memset(this, 0, sizeof(*this));
			//cumulate(); // 全要素0なら不要なはず。
		}

		bool read(const char* filename);

		bool write(const char* filename);

		void cumulate();

		void decumulate();

		void symCopy() {
			symmetry<true>();
		}

		void symAdd(){
			symmetry<true>();
		}

		void setPiece(const Shogi::Piece& p, T value) {
			Shogi::Piece bp = p.getTurnedBlack();
			piece[bp] = value;
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

		void setKPP(int kingIndex, int pieceIndex, U value) {
			kpp[kingIndex][pieceIndex] = value;
		}

		void setKPP(int index, U value) {
			kpp[0][index] = value;
		}

		U getKPP(int kingIndex, int pieceIndex) const {
			return kpp[kingIndex][pieceIndex];
		}

		U getKPP(int index) const {
			return kpp[0][index];
		}

		void getKKP(int kingIndex1, int kingIndex2, int pieceIndex, U value) {
			kkp[kingIndex1][kingIndex2][pieceIndex] = value;
		}

		void getKKP(int index, U value) {
			kkp[0][0][index] = value;
		}

		U getKKP(int kingIndex1, int kingIndex2, int pieceIndex) const {
			return kkp[kingIndex1][kingIndex2][pieceIndex];
		}

		U getKKP(int index) const {
			return kkp[0][0][index];
		}

		Type& operator+=(Type& param) {
			for (Shogi::Piece p = Shogi::Piece::PAWN; p <= Shogi::Piece::DRAGON; p.toNext()) {
				piece[p] += param.piece[p];
			}

			for(int i = 0 ; i < 81 * KPP_MAX ; i++) {
				kpp[0][i] += param.kpp[0][i];
			}

			for(int i = 0 ; i < 81 * 81 * KKP_ALL ; i++) {
				kkp[0][0][i] += param.kkp[0][0][i];
			}

			return (*this);
		}
	};

	typedef TempParam<Value, ValueS> Param;
	typedef TempParam<ValueF, ValueF> Gradient;
};

#endif // PARAM_H_
