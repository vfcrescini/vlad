#!/bin/sh

# Vino Crescini
# 13 Oct 2002

echo "Content-type: text/html"
echo ""
eval $QUERY_STRING

if [ -z "$PARTITION" ] || [ "`ls /dev/$PARTITION 2> /dev/null | wc -l`" -ne 1 ]; then
  exit 0
fi

MOUNTPOINT="`mount | grep $PARTITION | cut -d \" \" -f 3`"
FREE="`df -h | grep $PARTITION | cut -c34-38`"
SIZE="`df -h | grep $PARTITION | cut -c21-26`"
PERCENT="`df -h | grep $PARTITION | cut -c40-44`"

echo -ne "/dev/$PARTITION mounted on $MOUNTPOINT has $FREE of $SIZE available ( $PERCENT usage )."
