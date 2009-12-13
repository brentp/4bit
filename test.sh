#!/bin/bash

SEQS[0]=CCCC
SEQS[1]=ACAC
SEQS[2]=NNNN
SEQS[3]=N-XA
SEQS[4]=AAAAAAAAAAAA
SEQS[5]=CACACAGG-Xc
SEQS[6]=actgACTGxXnN-

PASS=0
FAIL=0
for seq in ${SEQS[@]}
do
    OUT=`echo $seq | ./4bit-cli encode | ./4bit-cli decode`
    if [ $seq != $OUT ]
    then
        echo "BAD!! input:$seq output:$OUT"
        FAIL=$(( $FAIL+1 ))
    else
        echo "OK: $OUT"
        PASS=$(( $PASS+1 ))
    fi
done
TOT=$(( $PASS + $FAIL ))
echo ""
echo "$PASS / $TOT tests PASS"
if [ $FAIL != 0 ]
then
    echo "$FAIL tests FAIL"
    exit 1
else
    exit 0
fi
