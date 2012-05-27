/*
 * debug.cpp
 *
 *  Created on: 2012/05/12
 *      Author: ryosuke
 */


#ifndef NDEBUG

#include <boost/thread.hpp>
#include <ctime>
#include <fstream>
#include <cstdarg>
#include "debug.h"

namespace Tools{
	void Debug::Print( const char* str, ... ){
		std::ofstream file;
		char buf[1024];
		time_t t;
		struct tm* tmDate;
		char strDate[64];
		boost::xtime xt;
		boost::xtime_get(&xt, boost::TIME_UTC);

		va_list argx;
		va_start( argx, str );

		// ファイルオープン
		file.open( "debug.txt", std::ios::out | std::ios::app );
		if( file.is_open() ){
			// 時刻
			t = time( NULL );
			tmDate = localtime( &t );
			strftime( strDate, sizeof(strDate), "%F %H:%M:%S ", tmDate );

			// 出力文字列
			vsprintf( buf, str, argx );

			// 出力
			file << strDate << buf;
			file.close();
		}

		va_end( argx );
	}
}

#endif // NDEBUG
