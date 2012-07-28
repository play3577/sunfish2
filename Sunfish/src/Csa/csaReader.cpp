/*
 * csaReader.cpp
 * 
 *  Created on: 2012/06/09
 *      Author: Kubo Ryosuke
 */

#include "csaReader.h"
#include "../Shogi/square.h"
#include "../Tools/debug.h"
#include <cstring>
#include <fstream>
#include <iostream>

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
		case CHAR_COM: // '\''
		case CHAR_NON: // '\0'
			return LINE_EMPTY;
		default:
			return LINE_ERROR;
		}
	}

	CsaReader::LineStat CsaReader::parseLine(const char* line, Record::Record& pos) {
		switch (line[0]) {
		case CHAR_BLK: // '+'
			return LINE_MOVE;
		case CHAR_WHT: // '-'
			return LINE_MOVE;
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
			if (in.fail()) { return false; }
			if (in.eof()) { break; }
			LineStat stat = parseLine(line, pos);
			if (stat == LINE_TURN) {
				break;
			} else if (stat == LINE_ERROR) {
				return false;
			}
		}
		pos.update();
		return true;
	}

	bool CsaReader::read(const char* filename, Position& pos) {
		std::ifstream fin(filename);
		if (!fin) { return false; }
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
		if (!fin) { return false; }

		if (!read(fin, pos)) {
			fin.close();
			return false;
		}
		record.init(pos);

		fin.close();
		return true;
	}
}
