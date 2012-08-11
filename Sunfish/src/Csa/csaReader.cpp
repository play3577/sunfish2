/*
 * csaReader.cpp
 * 
 *  Created on: 2012/06/09
 *      Author: Kubo Ryosuke
 */

#include "csaReader.h"
#include "../Shogi/square.h"
#include "../Tools/debug.h"
#include "../Log/logger.h"
#include <cstring>
#include <fstream>

namespace Csa {
	using namespace Shogi;

	bool CsaReader::parseLineBoard(const char* line, unsigned rank, Position& pos) {
		if (strlen(line) < 3 * Square::FILE_NUM) {
			return false;
		}
		for (int file = Square::FILE_NUM; file >= 1; file-- ) {
			Piece piece = Piece::parseCsa(line);
			pos.setBoard(Square(file, rank), piece);
			line += 3;
		}
		return true;
	}

	bool CsaReader::parseLineHand(const char* line, Position& pos, bool black) {
		while (strlen(line) >= 4) {
			unsigned file = line[0] - '0';
			unsigned rank = line[1] - '0';
			Piece piece = Piece::parseCsa(line+2);
			if (piece != Piece::EMPTY) {
				if (Square::isInside(file, rank)) {
					pos.setBoard(Square(file, rank), piece);
				} else if (file == 0 && rank == 0) {
					if (black) { pos.incBlackHand(piece); }
					else       { pos.incWhiteHand(piece); }
				} else {
					return false;
				}
			} else {
				return false;
			}
			line += 4;
		}
		return true;
	}

	bool CsaReader::parseLineMove(const char* line, Record::Record& record) {
		/*
		if (strlen(line) < 6) {
			return false;
		}
		Square from(line[0]-'0', line[1]-'0');
		Square to(line[2]-'0', line[3]-'0');
		Piece piece = Piece::parseCsa(&line[4]);
		if (!from.valid() || !to.valid() || piece.isEmpty()) {
			return false;
		}
		return record.move(Move(from, to, promote, hand, piece));
		*/
		Move move;
		return record.getPosition().inputMoveCsa(line, move) && record.move(move);
	}

	CsaReader::LineStat CsaReader::parseLine(const char* line, Position& pos) {
		switch (line[0]) {
		case CHAR_POS: // 'P'
			if (line[1] >= '1' && line[1] <= '9') { // P1, P2, ..., P9
				return parseLineBoard(line+2, line[1]-'0', pos) ? LINE_BOARD : LINE_ERROR;
			} else if (line[1] == CHAR_BLK) { // P+
				return parseLineHand(line+2, pos, true) ? LINE_HAND : LINE_ERROR;
			} else if (line[1] == CHAR_WHT) { // P-
				return parseLineHand(line+2, pos, false) ? LINE_HAND : LINE_ERROR;
			}
			return LINE_ERROR;
		case CHAR_BLK: // '+'
			return pos.setBlackTurn() ? LINE_TURN : LINE_ERROR;
		case CHAR_WHT: // '-'
			return pos.setWhiteTurn() ? LINE_TURN : LINE_ERROR;
		case CHAR_VAR: // 'V'
			return LINE_VAR;
		case CHAR_PLR: // 'N'
		case CHAR_INF: // '$'
			return LINE_INFO;
		case CHAR_COM: // '\''
		case CHAR_NON: // '\0'
			return LINE_EMPTY;
		default:
			return LINE_ERROR;
		}
	}

	CsaReader::LineStat CsaReader::parseLine(const char* line, Record::Record& record) {
		switch (line[0]) {
		case CHAR_BLK: // '+'
		case CHAR_WHT: // '-'
			return parseLineMove(line, record) ? LINE_MOVE : LINE_ERROR;
		case CHAR_TIM: // 'T'
			return LINE_TIME;
		case CHAR_STA: // '%'
			return LINE_STAT;
		case CHAR_COM: // '\''
		case CHAR_NON: // '\0'
			return LINE_EMPTY;
		default:
			return LINE_ERROR;
		}
	}

	bool CsaReader::read(std::istream& in, Position& pos) {
		char line[LINE_BUFFER_SIZE];
		pos.initNoUpdate();
		while (true) {
			in.getline(line, sizeof(line));
			if (in.eof()) { break; }
			if (in.fail()) {
				Log::Logger::out() << "ERROR: unknown i/o error :\"" << line << "\"\n";
				return false;
			}
			LineStat stat = parseLine(line, pos);
			if (stat == LINE_TURN) {
				break;
			} else if (stat == LINE_ERROR) {
				Log::Logger::out() << "ERROR: CSA syntax :\"" << line << "\"\n";
				return false;
			}
		}
		pos.update();
		return true;
	}

	bool CsaReader::read(const char* filename, Position& pos) {
		std::ifstream fin(filename);
		if (!fin) {
			Log::Logger::out() << "ERROR: can't open a file :\"" << "\"\n";
			return false;
		}
		if (!read(fin, pos)) {
			fin.close();
			return false;
		}
		fin.close();
		return true;
	}

	bool CsaReader::read(const char* filename, Record::Record& record) {
		Position pos;
		std::ifstream fin(filename);
		if (!fin) {
			Log::Logger::out() << "ERROR: can't open a file :\"" << "\"\n";
			return false;
		}

		// 局面の読み込み
		if (!read(fin, pos)) {
			fin.close();
			return false;
		}
		record.init(pos);

		// 指し手の読み込み
		while (true) {
			char line[LINE_BUFFER_SIZE];
			fin.getline(line, sizeof(line));
			if (fin.eof()) { break; }
			if (fin.fail()) {
				Log::Logger::out() << "ERROR: unknown i/o error :\"" << line << "\"\n";
				return false;
			}
			LineStat stat = parseLine(line, record);
			if (stat == LINE_ERROR) {
				Log::Logger::out() << "ERROR: CSA syntax :\"" << line << "\"\n";
				fin.close();
				return false;
			}
		}

		fin.close();
		return true;
	}
}
