#!/bin/bash

LABELTXT=labels.txt

[ -d "$1" ] && cd $1

cp /dev/null $LABELTXT
i=0
for d in `ls -d */` ; do
  mv $d $i
  echo "$i `basename $d`" >> $LABELTXT
  i=$((i+1))
done

