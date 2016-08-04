#!/usr/bin/env bash
set -o errexit
set -o nounset

TEST=DB
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
THOME=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
CNDB=$THOME/cndb
if [[ ! -x $CNDB ]]; then
  echo "P5IMS ERROR $TEST: database canonical-izer $CNDB not found" >&2
  exit 1
fi
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

DB1=db-test1.txt; dieifthere $DB1; junk $DB1
DB2=db-test2.txt; dieifthere $DB2; junk $DB2
DB3=db-test3.txt; dieifthere $DB3; junk $DB3
DB4=db-test4.txt; dieifthere $DB4; junk $DB4
LOG=log.txt; dieifthere $LOG; junk $LOG

PORT=$[ 5000 + ($RANDOM % 3000)]
UA=UU_$(printf %06u $[ $RANDOM % 1000000 ])
UB=UU_$(printf %06u $[ $RANDOM % 1000000 ])
UC=UU_$(printf %06u $[ $RANDOM % 1000000 ])
UD=UU_$(printf %06u $[ $RANDOM % 1000000 ])
UE=UU_$(printf %06u $[ $RANDOM % 1000000 ])
UF=UU_$(printf %06u $[ $RANDOM % 1000000 ])
UG=UU_$(printf %06u $[ $RANDOM % 1000000 ])
UH=UU_$(printf %06u $[ $RANDOM % 1000000 ])
PAUSE=3

touch $DB1
cat > $DB1 <<endofusers
8 users:
$UA
- $UB
- $UC
.
$UB
- $UA
- $UC
- $UD requested
.
$UC
- $UA
- $UB
- $UD pending
- $UE requested
- $UF
.
$UD
- $UB pending
- $UC requested
- $UE requested
.
$UE
- $UC pending
- $UD pending
- $UG requested
.
$UF
- $UC
.
$UG
- $UE pending
.
$UH
.
endofusers

$CNDB < $DB1 > $DB2

echo "vvvvvvvvvvvvvvvvvvvvv $DB2 before starting ims"
cat $DB2
echo "^^^^^^^^^^^^^^^^^^^^^"

./ims -p $PORT -d $DB2 -i $PAUSE &> $LOG &
IMSPID=$!

echo "=== sleep 1"
sleep 1

echo "=== rm -f $DB2"
rm -f $DB2

echo "=== sleep $[$PAUSE+2] (waiting for $DB2 to be re-written)"
sleep $[$PAUSE+2]

$CNDB < $DB2 > $DB3
$CNDB < $DB1 > $DB4

echo "vvvvvvvvvvvvvvvvvvvvv new $DB2, made canonical ($DB3)"
cat $DB3
echo "^^^^^^^^^^^^^^^^^^^^^"

echo "vvvvvvvvvvvvvvvvvvvvv difference between canonical(input) and canonical(output)"
diff $DB4 $DB3
echo "^^^^^^^^^^^^^^^^^^^^^"

! diff $DB4 $DB3 > /dev/null
okay=$?

echo "P5IMS TEST $TEST: $okay"
