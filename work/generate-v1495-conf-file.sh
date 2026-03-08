#!/bin/bash

RUN=${1:?'Need a run number.'}

DIR_IN=/data4/e1039_data/online/intime_v1495/run_$(printf '%06d' $RUN)
DIR_OUT=conf_run_$(printf '%06d' $RUN)
WIDTH=19.00
echo "RUN     = $RUN"
echo "DIR_IN  = $DIR_IN"
echo "DIR_OUT = $DIR_OUT"

FILE_IN=$DIR_IN/v1495_in_time.txt
if [ ! -e $FILE_IN  ] ; then
    echo "Cannot find the input file: $FILE_IN.  Abort."
    exit 1
fi

FILE_HODO_OUT=$DIR_OUT/hodo.tsv
FILE_L2_OUT=$DIR_OUT/L2.tsv # RF, STOP, etc
rm -rf   $DIR_OUT
mkdir -p $DIR_OUT
while read DET_NAME DET_ID ELE_ID TIME STATUS ; do
    TIME2=$(printf '%.2f' $TIME)
    LINE=
    if [ $STATUS = 0 ] ; then
	LINE="$DET_NAME\t$ELE_ID\t${TIME2}\t$WIDTH"
    elif [ $STATUS = 1 ] ; then
	LINE="$DET_NAME\t$ELE_ID\t${TIME2}?\t$WIDTH"
    else 
	LINE="$DET_NAME\t$ELE_ID\t${TIME2}??\t$WIDTH"
    fi
    if [ ${DET_NAME:0:1} = 'H' ] ; then
	echo -e "$LINE" >>$FILE_HODO_OUT
    else
	echo -e "$LINE" >>$FILE_L2_OUT
    fi

done <$FILE_IN
