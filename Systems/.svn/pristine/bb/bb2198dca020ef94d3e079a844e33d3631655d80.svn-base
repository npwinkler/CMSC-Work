#!/usr/bin/env bash
#
# Integrity check for cs154-2014 Project 3
#
# Changing anything here defeats the purpose of the integrity check,
# and the integrity check is done to help ensure that all students
# can run the autograder, and see what the instructors will see
# when they grade the student's work.
#

X="!!!"
if [[ ! -x /bin/uname ]]; then
  echo "$X This doesn't seem to be a CSIL Linux machine:"
  echo "$X /bin/uname doesn't exist."
  echo "$X CSIL Linux is the only supported platform for cs154."
  exit 1
fi
unm="/bin/uname -srmpo"
utry=$($unm 2>&1 >/dev/null)
if [[ "$utry" ]]; then
  echo "$X This doesn't seem to be a CSIL Linux machine:"
  echo "$X \"$unm\" does not work as expected."
  echo "$X CSIL Linux is the only supported platform for cs154."
  exit 1
fi
# some variation in release number exists within CSIL
uwant="Linux 3.?.?-??-generic x86_64 x86_64 GNU/Linux"
ugot=$($unm)
if [[ ! $ugot == $uwant ]]; then
  echo "$X This doesn't seem to be a CSIL Linux machine:"
  echo "$X uname said:   \"$ugot\""
  echo "$X but expected: \"$uwant\""
  echo "$X CSIL Linux is the only supported platform for cs154."
  exit 1
fi

ret=0
function ckc {
  if [[ ! -e $3 ]]; then
    echo "$X File $3 has been removed; try restoring with:"
    echo "  svn update $3"
    ret=1
    return
  fi
  ckr=($(cksum $3))
  if [[ ${ckr[0]} != $1 || ${ckr[1]} != $2 ]]; then
    echo "$X cksum $3 (${ckr[0]} ${ckr[1]}) != expected $1 $2"
    echo "$X To restore (as long as you haven't commited changes to $3):"
    echo "  rm -f $3"
    echo "  svn update $3"
    ret=1
  fi
}

# These values reflects the state of the files in the initial
# distribution, and none of these files should be changed.
ckc 4164929706 1186 README
ckc 754854602 2125 cachelab.c
ckc 1727004604 1030 cachelab.h
ckc 3146275062 26371 csim-ref
ckc 2954222233 5612 driver.py
ckc 1895651630 1177 Makefile
ckc 4243262528 17598 test-csim
ckc 3140584004 8127 test-trans.c
ckc 2482096016 2685 tracegen.c

if [[ $ret == 1 ]]; then
  echo "$X Only csim.c and trans.c should be modified for Project 3."
fi

# let make know if there was an error
exit $ret
