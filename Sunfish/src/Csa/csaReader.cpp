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
	bool CsaReader::parseLineBoard(const char* line, unsigned rank, Shogi::Position& pos) {
		if (strlen(line) < 3 * Shogi::Square::FILE_NUM) {
			return false;
		}
		for (int file = Shogi::Square::FILE_NUM; file >= 1; file-- ) {
			Shogi::Piece piece = Shogi::Piece::parseCsa(line);
			pos.setBoard(Shogi::Square(file, rank), piece);
			line += 3;
		}
		return true;
	}

	bool CsaReader::parseLineHand(const char* line, Shogi::Position& pos, bool black) {
		while (strlen(line) >= 4) {
			unsigned file = line[0] - '0';
			unsigned rank = line[1] - '0';
			Shogi::Piece piece = Shogi::Piece::parseCsa(line+2);
			if (piece != Shogi::Piece::EMPTY) {
				if (Shogi::Square::isInside(file, rank)) {
					pos.setBoard(Shogi::Square(file, rank), piece);
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

	bool CsaReader::parseLine(const char* line, Shogi::Position& pos) {
		switch (line[0]) {
		case CHAR_POS: // 'P'
			if (line[1] >= '1' && line[1] <= '9') { // P1, P2, ..., P9
				return parseLineBoard(line+2, line[1]-'0', pos);
			} else if (line[1] == CHAR_BLK) { // P+
				return parseLineHand(line+2, pos, true);
			} else if (line[1] == CHAR_WHT) { // P-
				return parseLineHand(line+2, pos, false);
			}
			return false;
		case CHAR_BLK: // '+'
			pos.setBlackTurn();
			return true;
		case CHAR_WHT: // '-'
			pos.setWhiteTurn();
			return true;
		default:
			return false;
		}
	}

	bool CsaReader::read(const char* filename, Shogi::Position& pos) {
		char line[LINE_BUFFER_SIZE];
		std::ifstream fin(filename);
		if (!fin) { return false; }
		pos.initNoUpdate();
		while (true) {
			fin.getline(line, sizeof(line));
			if (fin.fail()) { return false; }
			if (fin.eof()) { break; }
			if (!parseLine(line, pos)) {
				//return false;
			}
		}
		pos.update();
		fin.close();
		return true;
	}

	bool CsaReader::read(const char* filename, Record::Record& record) {
		return true;
	}
}
