#!/bin/sh

# type screen >/dev/null 2>&1 || echo "screen must be installed" && exit

. $(dirname $(readlink -f $0))/setebhost
program=jsroot_totmonitor

#_______________________________________________________________________________
# if [ -z $ROOTSYS ]; then
#     echo "ROOTSYS must be set"
#     exit 1
# fi
# thisroot_sh=$ROOTSYS/bin/thisroot.sh

#_______________________________________________________________________________
top_dir=$(dirname $(readlink -f $0))/..
server=$top_dir/bin/$program

conf=/param/conf/analyzer.conf
if [ -z "$1" ]; then
    data=${ebhost}:8901
else
    data=$top_dir/data/run$1.dat.gz
fi

# if [ -z "$1" ]; then
#     conf=/param/conf/analyzer.conf
# else
#     conf=$1
# fi
# if [ -z "$2" ]; then
#     data=eb0:8901
# else
#     data=$2
# fi

#_______________________________________________________________________________
pid=$(pgrep -fo "$server $conf $data")
if [ ! -z $pid ]; then
    echo "http_server is already running ($pid)"
    exit 1
fi
#_______________________________________________________________________________
# screen -AmdS K18OnlineServer \
#     sh -c ". $thisroot_sh && while true; do $server $conf $data; done"
screen -AmdS K18OnlineServerTOTMonitor \
    sh -c "while true; do $server $conf $data; done"
