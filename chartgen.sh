#!/bin/bash

LBOX=/home/$USER/lazybox

DATFILE="./out/profile_rawdat"

SZ_MEMS_LST=(`grep "Walk" $DATFILE | awk '{print $2}'`)

APS=(`grep "accesses per second" ./out/profile_rawdat | awk '{print $1}'`)

OUT="aps\n"
for IDX in ${!SZ_MEMS_LST[@]}
do
	OUT+="${SZ_MEMS_LST[$IDX]} ${APS[$IDX]}\n"
done

echo -e $OUT | sed 's/,//g' | $LBOX/gnuplot/plot_stdin.sh
mv plot.pdf out/aps.pdf

IPCS=(`grep "insn per cycle" $DATFILE | awk '{print $4}'`)
OUT="ips\n"
for IDX in ${!SZ_MEMS_LST[@]}
do
	OUT+="${SZ_MEMS_LST[$IDX]} ${IPCS[$IDX]}\n"
done
echo -e $OUT | sed 's/,//g' | $LBOX/gnuplot/plot_stdin.sh
mv plot.pdf out/ips.pdf
