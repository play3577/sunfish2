/*
 * attackTable.cpp
 *
 *  Created on: 2012/09/30
 *      Author: ryosuke
 */

#include <iostream>

#include "attackTableGenerator.h"
#include "checkTableGenerator.h"

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cerr << "ERROR: require an argument.\n";
	}
	Develop::AttackTableGenerator codeGen;
	codeGen.write(argv[1]);
}
