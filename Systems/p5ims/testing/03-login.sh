#!/usr/bin/env bash
set -o errexit
set -o nounset

TEST=LOGIN
IMSPID=0
JUNK=""
function junk {
  JUNK="$JUNK $@"
}
function cleanup {
  rm -rf $JUNK
  if [[ $IMSPID > 0 ]]; then
    kill -9 $IMSPID &> /dev/null
  fi
}
trap cleanup err exit int term
function dieifthere {
  if [[ -e $1 ]]; then
    echo "P5IMS ERROR $TEST: $1 exists already; \"rm $1\" to proceed with testing" >&2
    exit 1
 fi
}

# get to where ims and txtimc should be
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && cd .. && pwd )
echo "=== cd $DIR"
cd $DIR

if [[ ! -x ims ]]; then
  echo "P5IMS ERROR $TEST: don't see ims executable in $PWD" >&2
  exit 1
fi
if [[ ! -x txtimc ]]; then
  echo "P5IMS ERROR $TEST: don't see txtimc executable in $PWD" >&2
  exit 1
fi
CKSA=($(cksum txtimc))
if [[ 1724304442 != ${CKSA[0]} ]]; then
  echo "P5IMS ERROR $TEST: cksum txtimc ${CKSA[0]} != 1724304442; this isn't the txtimc that will be used for grading" >&2
  exit 1
fi

DB=db-test1.txt; dieifthere $DB; junk $DB
CIN1=in1.txt; dieifthere $CIN1; junk $CIN1
CIN2=in2.txt; dieifthere $CIN2; junk $CIN2
CIN3=in3.txt; dieifthere $CIN3; junk $CIN3
COUT1=out1.txt; dieifthere $COUT1; junk $COUT1
COUT2=out2.txt; dieifthere $COUT2; junk $COUT2
COUT3=out3.txt; dieifthere $COUT3; junk $COUT3
LOG=log.txt; dieifthere $LOG; junk $LOG

touch $CIN1
UU1=UU_$(printf %06u $[ $RANDOM % 1000000 ])
echo "register $UU1" >> $CIN1
echo "sleep 1"  >> $CIN1
echo "login $UU1" >> $CIN1
echo "sleep 1"  >> $CIN1

touch $CIN2
UU2=UU_$(printf %06u $[ $RANDOM % 1000000 ])
echo "register $UU2" >> $CIN2
echo "sleep 1"  >> $CIN2
echo "login $UU2" >> $CIN2
echo "sleep 1"  >> $CIN2

touch $CIN3
UU3=UU_$(printf %06u $[ $RANDOM % 1000000 ])
echo "register $UU3" >> $CIN3
echo "sleep 1"  >> $CIN3
echo "login $UU3" >> $CIN3
echo "sleep 1"  >> $CIN3


PORT=$[ 5000 + ($RANDOM % 3000)]
PAUSE=3

junk $DB
echo "0 users:" > $DB

./ims -p $PORT -d $DB -i $PAUSE &> $LOG &
IMSPID=$!
sleep 1


echo "vvvvvvvvvvvvvvvvvvvvv txtimc inputs:"
cat $CIN1
echo "====================="
cat $CIN2
echo "====================="
cat $CIN3
echo "^^^^^^^^^^^^^^^^^^^^^"

./txtimc -s localhost -p $PORT < $CIN1 &> $COUT1 &
./txtimc -s localhost -p $PORT < $CIN2 &> $COUT2 &
./txtimc -s localhost -p $PORT < $CIN3 &> $COUT3 &

echo "=== sleep $[$PAUSE+2] (waiting for $DB to be re-written)"
sleep $[$PAUSE+2]

echo "vvvvvvvvvvvvvvvvvvvvv txtimc output:"
cat $COUT1
echo "====================="
cat $COUT2
echo "====================="
cat $COUT3
echo "^^^^^^^^^^^^^^^^^^^^^"

echo "vvvvvvvvvvvvvvvvvvvvv final $DB"
cat $DB
echo "^^^^^^^^^^^^^^^^^^^^^"

gotack1=$(grep "ACK LOGIN $UU1" $COUT1 | wc -l)
gotack2=$(grep "ACK LOGIN $UU2" $COUT2 | wc -l)
gotack3=$(grep "ACK LOGIN $UU3" $COUT3 | wc -l)

echo "P5IMS TEST $TEST: ACK1 $gotack1"
echo "P5IMS TEST $TEST: ACK2 $gotack2"
echo "P5IMS TEST $TEST: ACK3 $gotack3"
