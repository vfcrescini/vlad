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

samples=17
runs=10

distdir=../dist
sampledir=../samples
outdir=runs
prog=${distdir}/bin/vlad

# check stuff
if ! [ -d ${distdir} -a -d ${sampledir} -a -x ${prog} ]; then
  exit -1
fi

# if outdir aleady exists, exit, otherwise create
if [ -d ${outdir} -o -r ${outdir} ]; then
  exit -1
fi
mkdir -p ${outdir}

# runs
for i in `seq -w -s " " 1 ${samples}`; do
    # skip certain samples
    sample=`echo "${i} + 0" | bc`
    if [ ${sample} -eq 13 ] || [ ${sample} -eq 17 ]; then
      continue
    fi
  for j in `seq -w -s " " 1 ${runs}`; do
    if ! ${prog} -t -e ${sampledir}/test-${i}.vl{p,a} 2> ${outdir}/${i}-${j} > /dev/null; then
      exit -1
    fi
  done
done

exit 0
