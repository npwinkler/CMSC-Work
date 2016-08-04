#!/usr/bin/env bash
set -o errexit
set -o nounset

TEST=REGISTER
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

DB=db-test.txt
CIN=in.txt
COUT=out.txt
LOG=log.txt

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

PORT=$[ 5000 + ($RANDOM % 3000)]
UU=UU_$(printf %06u $[ $RANDOM % 1000000 ])
PAUSE=3

DB=db-test.txt; dieifthere $DB; junk $DB
CIN=in.txt; dieifthere $CIN; junk $CIN
COUT=out.txt; dieifthere $COUT; junk $COUT
LOG=log.txt; dieifthere $LOG; junk $LOG

echo "0 users:" > $DB

./ims -p $PORT -d $DB -i $PAUSE &> $LOG &
IMSPID=$!
sleep 1

touch $CIN
echo "register $UU" >> $CIN
echo "sleep 1"  >> $CIN

echo "vvvvvvvvvvvvvvvvvvvvv txtimc input:"
cat $CIN
echo "^^^^^^^^^^^^^^^^^^^^^"

./txtimc -s localhost -p $PORT < $CIN &> $COUT

echo "=== sleep $[$PAUSE+2] (waiting for $DB to be re-written)"
sleep $[$PAUSE+2]

echo "vvvvvvvvvvvvvvvvvvvvv txtimc output:"
cat $COUT
echo "^^^^^^^^^^^^^^^^^^^^^"

echo "vvvvvvvvvvvvvvvvvvvvv final $DB"
cat $DB
echo "^^^^^^^^^^^^^^^^^^^^^"

gotack=$(grep "ACK REGISTER $UU" $COUT | wc -l)
indb=$(grep $UU $DB | wc -l)

echo "P5IMS TEST $TEST: ACK $gotack"
echo "P5IMS TEST $TEST: INDB $indb"
