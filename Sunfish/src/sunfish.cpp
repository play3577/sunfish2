/*
 * sunfish.cpp
 *
 *  Created on: 2012/05/08
 *      Author: ryosuke
 */

#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include "Cui/controller.h"
#include "Shogi/position.h"
#include "sunfish.h"
#include "Network/csaClient.h"
#include "Log/logger.h"
#include "Test/shogiTest.h"
#include "Learn/learn.h"
#include "Evaluates/paramAnalyzer.h"

using boost::program_options::options_description;
using boost::program_options::variables_map;
using boost::program_options::value;
using boost::program_options::store;
using boost::program_options::parse_command_line;

#ifndef NDEBUG
bool test();
#endif //NDEBUG

int main(int argc, char* argv[]) {
	std::cout << SUNFISH_NAME << ' ';
	std::cout << SUNFISH_VERSION << '\n';
	std::cout << SUNFISH_COPYRIGHT << '\n';

	// log
	Log::error.addStream(std::cerr, "\x1b[31m", "\x1b[39m");
	Log::warning.addStream(std::cerr, "\x1b[33m", "\x1b[39m");
	Log::message.addStream(std::cerr);
	Log::send.addStream(std::cerr, "\x1b[34m", "\x1b[39m");
	Log::receive.addStream(std::cerr, "\x1b[35m", "\x1b[39m");
#ifndef NDEBUG
	Log::debug.addStream(std::cerr, "\x1b[36m", "\x1b[39m");
	Log::test.addStream(std::cerr, "\x1b[32m", "\x1b[39m");
#endif

	// hash
	unsigned hashSize = Shogi::PositionHash::getRequireSize();
	Util::Hash hash(hashSize);
	Shogi::PositionHash posHash(hash);
	Shogi::Position::setPositionHash(&posHash);

	// program options
	options_description opt("Option");
	opt.add_options()
			("help,h", "show help")
#ifndef NDEBUG
			("test", "unit test")
#endif //NDEBUG
#ifndef NLEARN
			("learn", "learning")
			("analyze", "analyzing for 'evdata'")
#endif //NLEARN
			("network,n", "CSA client moode")
			("auto-black,b", "search will be begun automatically on black turn.")
			("auto-white,w", "search will be begun automatically on white turn.")
			("depth,d", value<int>(), "search depth")
			("limit,l", value<int>(), "search limit(sec)")
			("parallel,p", value<int>(), "number of worker threads")
			("file,f", value<std::string>(), "CSA file name to read")
			;
	variables_map argmap;
	try {
		store(parse_command_line(argc, argv, opt), argmap);
	} catch (std::exception& e) {
		std::cerr << "ERROR :" << e.what() << std::endl;
		std::cerr << opt << std::endl;
		return 1;
	}

	if (argmap.count("help")) {
		// ** コマンドヘルプの表示
		std::cerr << opt << std::endl;
		return 0;
#ifndef NDEBUG
	} else if (argmap.count("test")) {
		// ** ユニットテスト
		return test() ? 0 : 1;
#endif //NDEBUG
#ifndef NLEARN
	} else if (argmap.count("learn")) {
		// ** 機械学習
		using namespace Learns;

		// log
		std::ofstream fout("learn.log", std::ios::out | std::ios::app);
		if (fout) {
			Log::error.addStream(fout);
			Log::warning.addStream(fout);
			Log::message.addStream(fout);
#ifndef NDEBUG
			//Log::debug.addStream(fout);
#endif
		}

		Learn learn;
		learn.execute();
		return 0;
	} else if (argmap.count("analyze")) {
		using namespace Evaluates;
		Param* pparam = new Param("evdata");
		ParamAnalyzer analyzer(*pparam);
		std::cout << analyzer.analyze();
		delete pparam;
		return 0;
#endif //NLEARN
	} else if (argmap.count("network")) {
		// ** CSA Client の起動

		// log
		std::ofstream fout("network.log", std::ios::out | std::ios::app);
		if (fout) {
			Log::error.addStream(fout);
			Log::warning.addStream(fout);
			Log::message.addStream(fout);
			Log::send.addStream(fout);
			Log::receive.addStream(fout);
#ifndef NDEBUG
			Log::debug.addStream(fout);
#endif
		}

		Network::CsaClient csaClient;
		csaClient.execute();
		fout.close(); // close a log file.
		return 0;
	}

	// ** CLI の起動
	Cui::Controller controller;
	if (argmap.count("auto-black")) {
		controller.setAutoBlack(true);
	}
	if (argmap.count("auto-white")) {
		controller.setAutoWhite(true);
	}
	if (argmap.count("depth")) {
		controller.setDepth(argmap["depth"].as<int>());
	}
	if (argmap.count("limit")) {
		controller.setLimit(argmap["limit"].as<int>());
	}
	if (argmap.count("parallel")) {
		controller.setWorker(argmap["parallel"].as<int>());
	}
	if (argmap.count("file")) {
		controller.setFilename(argmap["file"].as<std::string>().c_str());
	}
	controller.play();

	return 0;
}

#ifndef NDEBUG
bool test() {
	using namespace Tests;

	std::ofstream fout("test.log", std::ios::out | std::ios::app);
	if (fout) {
		Log::error.addStream(fout);
		Log::warning.addStream(fout);
		Log::message.addStream(fout);
		Log::send.addStream(fout);
		Log::receive.addStream(fout);
		Log::debug.addStream(fout);
		Log::test.addStream(fout);
	}

	if (!ShogiTest().test()) {
		return false;
	}

	return true;
}
#endif //NDEBUG
