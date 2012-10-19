

# Copyright (c) 2012 Purdue University                
# All rights reserved.                                                         
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met: redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer;               
# redistributions in binary form must reproduce the above copyright            
# notice, this list of conditions and the following disclaimer in the          
# documentation and/or other materials provided with the distribution;
# neither the name of the copyright holders nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS         
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT         
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT         
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
#            
# Authors: Malek Musleh   

### The following file was referenced from the following site: 
### http://www.m5sim.org/SPEC_CPU2006_benchmarks
###
### and subsequent changes were made


import os
import optparse
import sys

import m5
from m5.objects import *

m5.util.addToPath('../common')

bench_dir='/home/min/a/ece565/benchspec/CPU2006/'

### Note: That some benchmarks require this variable to be modified to match
### your home environment. This variable only applies for certain benchmarks
### that require read/write to an output directory
output_dir= '/home/min/a/username/outputs/spec2k6/'

#400.perlbench
perlbench = LiveProcess()
perlbench_dir = '400.perlbench/'
perlbench.executable =  bench_dir+perlbench_dir+'/exe/perlbench'
perlbench.cmd = [perlbench.executable] + ['-I./lib', 'attrs.pl']
perlbench.output = 'attrs.out'

#401.bzip2
bzip2 = LiveProcess()
bzip2_dir = '401.bzip2/'
bzip2.executable =  bench_dir+bzip2_dir+'/exe/bzip2'
data=bench_dir+bzip2_dir+'data/ref/input/input.source'
bzip2.cmd = [bzip2.executable] + [data, '1']
bzip2.output = 'input.source.out'

#403.gcc
gcc = LiveProcess()
gcc_dir = '403.gcc/'
gcc.executable =  bench_dir+gcc_dir+'/exe/gcc'
data=bench_dir+'/data/ref/input/166.i'
output=output_dir+'/gcc/166.s'
gcc.cmd = [gcc.executable] + [data]+['-o',output] + ['-quiet'] + ['-funroll-loops'] + ['-fforce-mem'] + ['-fcse-follow-jumps'] + ['-fcse-skip-blocks'] + ['-fexpensive-optimizations'] + ['-fstrength-reduce'] + ['-fpeephole']  + ['-fschedule-insns'] + ['-finline-functions'] + ['-fschedule-insns2']

#410.bwaves
bwaves = LiveProcess()
bwaves.executable =  bench_dir+'/exe/bwaves'
#bwaves.data = bwaves.data
bwaves.cmd = [bwaves.executable]

#416.gamess
gamess=LiveProcess()
gamess_dir='416.gamess/'
gamess.executable =  bench_dir+gamess_dir+'/exe/gamess'
gamess.cmd = [gamess.executable]
gamess.input='exam29.config'
gamess.output='exam29.output'

#429.mcf
mcf = LiveProcess()
mcf_dir = '429.mcf/'
mcf.executable = bench_dir+mcf_dir+'/exe/mcf'
data=bench_dir+mcf_dir+'/data/ref/input/inp.in'
mcf.cmd = [mcf.executable] + [data]
mcf.output = 'inp.out'

#433.milc
milc=LiveProcess()
milc_dir='433.milc/'
milc.executable = bench_dir+milc_dir+'/exe/milc'
stdin=bench_dir+milc_dir+'/data/ref/input/su3imp.in'
milc.cmd = [milc.executable]
milc.input=stdin
milc.output='su3imp.out'

#434.zeusmp
zeusmp=LiveProcess()
zeusmp_dir='434.zeusmp/'
zeusmp.executable = bench_dir+zeusmp_dir+'/exe/zeusmp'
zeusmp.cmd = [zeusmp.executable]
zeusmp.output = 'zeusmp.stdout'

#435.gromacs
gromacs = LiveProcess()
gromacs_dir='435.gromacs/'
gromacs.executable = bench_dir+gromacs_dir+gromacs_dir+'/exe/gromacs'
data=bench_dir+gromacs_dir+'/data/ref/input/gromacs.tpr'
gromacs.cmd = [gromacs.executable] + ['-silent','-deffnm',data,'-nice','0']

#436.cactusADM
cactusADM = LiveProcess()
cactusADM_dir = '436.cactusADM/'
cactusADM.executable =  bench_dir+cactusADM_dir+'/exe/cactusADM'
data=bench_dir+cactusADM_dir+'/data/ref/input/benchADM.par'
cactusADM.cmd = [cactusADM.executable] + [data]
cactusADM.output = 'benchADM.out'

#437.leslie3d
leslie3d=LiveProcess()
leslie3d_dir= '437.leslie3d/'
leslie3d.executable = bench_dir+leslie3d_dir+'/exe/leslie3d'
stdin=bench_dir+leslie3d_dir+'/data/ref/input/leslie3d.in'
leslie3d.cmd = [leslie3d.executable]
leslie3d.input=stdin
leslie3d.output='leslie3d.stdout'

#444.namd
namd = LiveProcess()
namd_dir='444.namd/'
namd.executable =  bench_dir+namd_dir+'/exe/namd'
input=bench_dir+namd_dir+'/data/all/input/namd.input'
namd.cmd = [namd.executable] + ['--input',input,'--iterations','1','--output','namd.out']
namd.output='namd.stdout'

#445.gobmk
gobmk=LiveProcess()
gobmk_dir = '445.gobmk/'
gobmk.executable = bench_dir+gobmk_dir+'/exe/gobmk'
stdin=bench_dir+gobmk_dir+'/data/ref/input/13x13.tst'
gobmk.cmd = [gobmk.executable]+['--quiet','--mode','gtp']
gobmk.input=stdin
gobmk.output='capture.out'

#447.dealII
dealII=LiveProcess()
dealII_dir = '447.dealII/'
dealII.executable = bench_dir+dealII_dir+'/exe/dealII'
dealII.cmd = [gobmk.executable]+['8']
dealII.output='log'

#450.soplex
soplex=LiveProcess()
soplex_dir = '450.soplex/'
soplex.executable = bench_dir+soplex_dir+'/exe/soplex'
data=bench_dir+soplex_dir+'/data/ref/input/ref.mps'
soplex.cmd = [soplex.executable]+['-m10000',data]
soplex.output = 'test.out'

#453.povray
povray=LiveProcess()
povray_dir = '453.povray/'
povray.executable = bench_dir+povray_dir+'/exe/povray'
data=bench_dir+povray_dir+'/data/ref/input/SPEC-benchmark-ref.ini'
povray.cmd = [povray.executable]+[data]
povray.output = 'SPEC-benchmark-ref.stdout'

#454.calculix
calculix=LiveProcess()
calculix_dir='454.calculix/'
calculix.executable = bench_dir+calculix_dir+'/exe/calculix'
data='/data/ref/input/hyperviscoplastic.inp'
calculix.cmd = [calculix.executable]+['-i',data]
calculix.output = 'beampic.log'

#456.hmmer
hmmer=LiveProcess()
hmmr_dir = '456.hmmr/'
hmmer.executable = bench_dir+hmmr_dir+'/exe/hmmer'
data=bench_dir+hmmr_dir+'/data/ref/input/nph3.hmm'
hmmer.cmd = [hmmer.executable]+['--fixed', '0', '--mean', '325', '--num', '5000', '--sd', '200', '--seed', '0', data]
hmmer.output = 'bombesin.out'

#458.sjeng
sjeng=LiveProcess()
sjeng_dir = '458.sjeng/'
sjeng.executable =  bench_dir+sjeng_dir+'/exe/sjeng'
data=bench_dir+sjeng_dir+'/data/ref/input/ref.txt'
sjeng.cmd = [sjeng.executable]+[data]
sjeng.output = 'ref.out'

#459.GemsFDTD
GemsFDTD=LiveProcess()
GemsFDTD_dir = '459.GemsFDTD/'
GemsFDTD.executable =  bench_dir+GemsFDTD_dir+'/exe/GemsFDTD'
GemsFDTD.cmd = [GemsFDTD.executable]
GemsFDTD.output = 'ref.log'

#462.libquantum
libquantum=LiveProcess()
libquantum_dir ='462.libquantum/'
libquantum.executable = bench_dir+libquantum_dir+'/exe/libquantum'
libquantum.cmd = [libquantum.executable],'33','5'
libquantum.output = 'ref.out'

#464.h264ref
h264ref=LiveProcess()
h264_dir = '464.h264ref/'
h264ref.executable = bench_dir+h264_dir+'/exe/h264'
data=bench_dir+h264_dir+'/data/ref/input/foreman_ref_encoder_baseline.cfg'
h264ref.cmd = [h264ref.executable]+['-d',data]
h264ref.output = 'foreman_ref_encoder_baseline.out'

#470.lbm
lbm=LiveProcess()
lbm_dir='470.lbm/'
lbm.executable = bench_dir+lbm_dir+'/exe/lbm'
data=bench_dir+lbm_dir+'/data/ref/input/100_100_130_ldc.of'
lbm.cmd = [lbm.executable]+['20', 'reference.dat', '0', '1' ,data]
lbm.output = 'lbm.out'

#471.omnetpp
omnetpp=LiveProcess()
omnetpp_dir = '471.omnetpp/'
omnetpp.executable =  bench_dir+omnetpp_dir+'/exe/omnetpp'
data=bench_dir+omnetpp_dir+'/data/ref/input/omnetpp.ini'
omnetpp.cmd = [omnetpp.executable]+[data]
omnetpp.output = 'omnetpp.log'

#473.astar
astar=LiveProcess()
astar_dir='473.astar'
astar.executable = bench_dir+astar_dir+'/exe/astar'
data=bench_dir+astar_dir+'/data/ref/input/rivers.cfg'
astar.cmd = [astar.executable]+[data]
astar.output = 'lake.out'

#481.wrf
wrf=LiveProcess()
wrf_dir = '481.wrf'
wrf.executable = bench_dir+wrf_dir+'/exe/wrf'
data = bench_dir+wrf_dir+'/data/ref/input/namelist.input'
wrf.cmd = [wrf.executable]+[data]
wrf.output = 'rsl.out.0000'

#482.sphinx
sphinx3=LiveProcess()
sphinx3_dir = '482.sphinx/'
sphinx3.executable =  bench_dir+sphinx3_dir+'/exe/sphinx'
sphinx3.cmd = [sphinx3.executable]+['ctlfile', '.', 'args.an4']
sphinx3.output = 'an4.out'

#483.xalancbmk
xalancbmk=LiveProcess()
xalanch_dir = '483.xalancbmk/'
xalancbmk.executable =  bench_dir+xalanch_dir+'/exe/Xalan'
data = bench_dir + xalanch_dir + '/data/ref/input/'
xalancbmk.cmd = [xalancbmk.executable]+['-v',data+'t5.xml',data+'xalanc.xsl']
xalancbmk.output = 'ref.out'

#998.specrand
specrand_i=LiveProcess()
specrand_i_dir = '998.specrand/'
specrand_i.executable = bench_dir+specrand_i_dir+'/exe/specrand_i'
specrand_i.cmd = [specrand_i.executable] + ['324342','24239']
specrand_i.output = 'rand.24239.out'

#999.specrand
specrand_f=LiveProcess()
specrand_f_dir = '999.specrand/'
specrand_f.executable = bench_dir+specrand_f_dir+'/exe/specrand_f'
specrand_f.cmd = [specrand_f.executable] + ['324342','24239']
specrand_f.output = 'rand.24239.out'
