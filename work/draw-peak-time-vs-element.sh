#!/bin/bash

RUN=${1:?'Need a run number.'}

DIR_RESULT=/data4/e1039_data/online/intime/run_$(printf '%06d' $RUN)
TXT_IN=hodo_in_time.txt
DIR_OUT=. # time_vs_ele.png
echo "RUN = $RUN"
echo "DIR_RESULT = $DIR_RESULT"

if [ ! -e $DIR_RESULT/$TXT_IN ] ; then
    echo "Cannot find the text file.  Abort."
    exit 1
fi

root.exe -b <<EOF
gSystem->Load("libOnlMon.so");
ExtractHodoInTime::DrawPeakTimeVsElement("$DIR_RESULT/$TXT_IN", "$DIR_OUT");
EOF
