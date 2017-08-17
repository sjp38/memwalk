#!/bin/bash

mkdir -p out

for sz in "$@"
do
	perf stat -d ./memwalk $sz 64 10 &>> out/profile_rawdat
done
