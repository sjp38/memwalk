#!/bin/bash

USAGE="$0 memsizes..."

for mem in "$@"
do
	./memwalk $mem 64 10
done
