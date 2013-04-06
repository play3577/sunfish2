#!/bin/bash
#
# 通信対局モード起動スクリプト
#

# TODO: 相対パス除去

LOGFILE=network.log

if [ -e ${LOGFILE} ]; then
	mv ${LOGFILE} ${LOGFILE}.`date "+%Y%m%d-%H%M%S"`
fi

./sunfish -n&
