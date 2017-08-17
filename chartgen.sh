#!/bin/bash

LBOX=/home/$USER/lazybox

DATFILE="./out/profile_rawdat"

SZ_MEMS=`grep "Walk" $DATFILE | awk '{print $2}'`

function plot() {
	TITLE=$1
	OUT="$TITLE\n"

	SLIST=($2)
	VALS=($3)

	for IDX in ${!SLIST[@]}
	do
		OUT+="${SLIST[$IDX]} ${VALS[$IDX]}\n"
	done
	echo -e $OUT
	echo -e $OUT | sed 's/,//g' | $LBOX/gnuplot/plot_stdin.sh
	mv plot.pdf out/$TITLE.pdf
}

APS=`grep "accesses per second" ./out/profile_rawdat | awk '{print $1}'`
plot "aps" "$SZ_MEMS" "$APS"

IPCS=`grep "insn per cycle" $DATFILE | awk '{print $4}'`
plot "ipc" "$SZ_MEMS" "$IPCS"

L1LOADS=`grep "L1-dcache-loads" $DATFILE | awk '{print $4}'`
plot "l1-dcache-loads" "$SZ_MEMS" "$L1LOADS"

L1MISSES=`grep "L1-dcache-load-misses" $DATFILE | awk '{print $4}'`
plot "l1-dcache-load-misses" "$SZ_MEMS" "$L1MISSES"

LLCLOADS=`grep "LLC-loads" $DATFILE | awk '{print $4}'`
plot "LLC-loads" "$SZ_MEMS" "$LLCLOADS"

LLCMISSES=`grep "LLC-load-misses" $DATFILE | awk '{print $4}'`
plot "LLC-load-misses" "$SZ_MEMS" "$LLCMISSES"
