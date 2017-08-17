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


TITLES=("ipc" "l1-dcache-loads" "l1-dcache-load-misses" "LLC-loads" "LLC-load-misses")
KEYWORDS=("insn per cycle" "L1-dcache-loads" "L1-dcache-load-misses" "LLC-loads" "LLC-load-misses")
for IDX in ${!TITLES[@]}
do
	VALUES=`grep "${KEYWORDS[$IDX]}" $DATFILE | awk '{print $4}'`
	echo $VALUES
	plot "${TITLES[$IDX]}" "$SZ_MEMS" "$VALUES"
done
