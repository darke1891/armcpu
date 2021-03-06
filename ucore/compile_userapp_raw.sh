#!/bin/bash -e
# $File: compile_userapp_raw.sh
# $Date: Sat Dec 21 21:04:43 2013 +0800
# $Author: jiakai <jia.kai66@gmail.com>

GCCPREFIX=mips-sde-elf-

LD=${GCCPREFIX}ld
CC=${GCCPREFIX}gcc
[ -z "$OPTFLAG" ]  && OPTFLAG=-O0
CFLAGS="-mips1 $OPTFLAG $CFLAGS -fno-builtin -nostdlib  -nostdinc -G0 -Wformat -EL -Wall -Werror"

src=$1
out=$2
obj=${out}.o



if [ -d $1 ]
then
    dir=$1
    src=$(ls $1/*.c)
    out=a.out
    obj=

    set -x
    for file in $src; do
        $CC -c $file -DUSER_PROG $CFLAGS -o $file.o
        obj=$obj" "$file.o
    done
    $LD -S -T $(dirname $0)/user/libs/user.ld $obj $LDFLAGS -o $dir/$out
    exit
fi

if [ -z "$out" ]
then
	echo "usage: $0 <.c source file> <output file>"
	exit
fi

set -x
$CC -c $src -DUSER_PROG $CFLAGS -o $obj
$LD -S -T $(dirname $0)/user/libs/user.ld $obj $LDFLAGS -o $out
rm -f $obj
