#!/bin/sh

# Vino Crescini
# 01 Jun 2003

dir=img
fcount=`ls -1 ${dir} | wc -l`
index=$((`date +%s`%${fcount}))
filename=`ls -1 ${dir} | head -n ${index} | tail -n 1`

echo "Content-type: image/${filename##*.}"
echo ""

cat ${dir}/${filename}
