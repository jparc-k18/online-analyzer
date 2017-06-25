#!/bin/sh

#_______________________________________________________________________________
pid=$(pgrep -fo http_server)
if [ ! -z $pid ]; then
    echo "http_server is already running ($pid)"
    exit 1
fi

#_______________________________________________________________________________
if [ -z $ROOTSYS ]; then
    echo "ROOTSYS must be set"
    exit 1
fi
thisroot_sh=$ROOTSYS/bin/thisroot.sh

#_______________________________________________________________________________
top_dir=$(dirname $(readlink -f $0))/..
server=$top_dir/bin/http_server

if [ -z $1 ]; then
    conf=/param/conf/analyzer.conf
else
    conf=$1
fi

if [ -z $2 ]; then
    data=eb0:8901
else
    data=$2
fi

#_______________________________________________________________________________
screen -AmdS K18OnlineServer \
    sh -c ". $thisroot_sh && while true; do $server $conf $data; done"
