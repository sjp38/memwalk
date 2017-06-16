#!/bin/bash

USAGE="$0 <memsizes...>"

if [ $# -lt 1 ];
then
	echo "Usage: $USAGE"
	exit 1
fi

for mem in "$@"
do
	./memwalk $mem 64 10
done
