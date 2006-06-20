#!/bin/sh
# This file is part of PolicyUpdater.
#
# Copyright (C) 2003, 2004, 2005, 2006 University of Western Sydney
# by Vino Fernando Crescini <jcrescin@scm.uws.edu.au>
#
# PolicyUpdater is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# PolicyUpdater is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with PolicyUpdater; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

samples=12
runs=10

sampledir=../samples
runsdir=runs

# check stuff
if ! [ -d ${sampledir} -a -d ${runsdir} ]; then
  exit -1
fi

# for each sample i
for i in `seq -w -s " " ${samples}`; do
  echo -ne "$i "

  # find average for compute statements
  csum=0
  for j in `seq -w -s " " ${runs}`; do
    eval "ctmp${j}=\`cat ${runsdir}/${i}-${j} | head -n 1 | tail -n 1\`"
    csum="${csum} + \$ctmp${j}"
  done
  tmp="`eval echo ${csum}`"
  tmp=`echo -ne "scale=6\n(${tmp}) / ${runs}.0\n" | bc`
  echo -ne "${tmp} "

  # find total number of queries for this sample
  queries=`grep "^.*query.*;" ${sampledir}/test-${i}.ops | wc -l`

  # find total average query time
  tqsum=0
  for j in `seq -s " " ${queries}`; do
    qsum=0
    for k in `seq -w -s " " ${runs}`; do
      eval "qtmp${k}=\`cat ${runsdir}/${i}-${k} | head -n $[${j} + 1] | tail -n 1\`"
      qsum="${qsum} + \$qtmp${k}"
    done
    tmp="`eval echo ${qsum}`"
    tmp=`echo -ne "scale=6\n(${tmp}) / ${runs}.0\n" | bc`
    tqsum="${tqsum} + \$tmp"
  done
  tmp="`eval echo ${tqsum}`"
  tmp=`echo -ne "scale=6\n${tmp}\n" | bc`
  echo "${tmp}"
done

exit 0
