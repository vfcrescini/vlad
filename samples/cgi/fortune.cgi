#!/bin/sh

# Vino Crescini
# 01 Jun 2003

indent=0

echo_indent()
{
   i=0
   while test ${i} -lt ${1}; do
    echo -n -e " "
    i=$((${i}+1))
  done 
}

eval ${QUERY_STRING}

echo "Content-type: text/html"
echo ""
echo ""

/usr/games/fortune startrek science linux |

while read line; do
  echo_indent $((${indent}+2))
  echo "${line}"
done
