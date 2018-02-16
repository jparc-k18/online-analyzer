#!/bin/sh

ana_dir=$(dirname $(readlink -f $0))/..
src_dir=$ana_dir/src
src_dir=`cd $src_dir && echo $PWD`

function copy()
{
    if [ ! -f $1 ]; then
	echo "cannod find $1"
	exit
    fi

    if [ -f $2 ]; then
	diff $1 $2
    else
	cp -i $1 $2
    fi
}

copy $src_dir/common.mk.org $src_dir/common.mk

for d in `find $src_dir -maxdepth 1 -type d`
do
    copy $d/Makefile.org $d/Makefile
done
