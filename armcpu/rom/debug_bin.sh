#!/bin/bash -e
# $File: run_loader.sh
# $Date: Tue Nov 26 09:50:30 2013 +0800
# $Author: jiakai <jia.kai66@gmail.com>

MEMTRANS=../../utils/memtrans/controller.py

./my_gemrom.sh $1

$MEMTRANS ram write 0 $1.bin
$MEMTRANS jmp 0
