# Copyright (c) 2012 Purdue University                                            
# All rights reserved.                                                            
#                                                                                 
# Redistribution and use in source and binary forms, with o without               
# modification, are permitted provided that the following coditions are           
# met: redistributions of source code must retain the above cpyright              
# notice, this list of conditions and the following disclaimer                    
# redistributions in binary form must reproduce the above copyrght                
# notice, this list of conditions and the following disclaimer i the              
# documentation and/or other materials provided with the distribuion;             
# neither the name of the copyright holders nor the names of its                  
# contributors may be used to endorse or promote products derived fom             
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
from m5.defines import buildEnv
from m5.objects import *
from m5.util import addToPath, fatal

addToPath('../common')
addToPath('../ruby')
addToPath('../topologies')

import Options
import Ruby
import Simulation
import CacheConfig
from Caches import *
from cpu2000 import *
import spec2k6

from Ruby import *

# Get paths we might need.  It's expected this file is in m5/configs/example.
config_path = os.path.dirname(os.path.abspath(__file__))
print config_path
config_root = os.path.dirname(config_path)
print config_root
m5_root = os.path.dirname(config_root)
print m5_root

execfile(os.path.join(config_root, "ruby", "Ruby.py"))

parser = optparse.OptionParser()
Options.addCommonOptions(parser)
Options.addSEOptions(parser)
Ruby.define_options(parser)

# Benchmark options

parser.add_option("-b", "--benchmark", default="",
                 help="The benchmark to be loaded.")

(options, args) = parser.parse_args()

if args:
    print "Error: script doesn't take any positional arguments"
    sys.exit(1)

if options.benchmark == 'perlbench':
   process = spec2k6.perlbench
elif options.benchmark == 'bzip2':
   process = spec2k6.bzip2
elif options.benchmark == 'gcc':
   process = spec2k6.gcc
elif options.benchmark == 'bwaves':
   process = spec2k6.bwaves
elif options.benchmark == 'gamess':
   process = spec2k6.gamess
elif options.benchmark == 'mcf':
   process = spec2k6.mcf
elif options.benchmark == 'milc':
   process = spec2k6.milc
elif options.benchmark == 'zeusmp':
   process = spec2k6.zeusmp
elif options.benchmark == 'gromacs':
   process = spec2k6.gromacs
elif options.benchmark == 'cactusADM':
   process = spec2k6.cactusADM
elif options.benchmark == 'leslie3d':
   process = spec2k6.leslie3d
elif options.benchmark == 'namd':
   process = spec2k6.namd
elif options.benchmark == 'gobmk':
   process = spec2k6.gobmk;
elif options.benchmark == 'dealII':
   process = spec2k6.dealII
elif options.benchmark == 'soplex':
   process = spec2k6.soplex
elif options.benchmark == 'povray':
   process = spec2k6.povray
elif options.benchmark == 'calculix':
   process = spec2k6.calculix
elif options.benchmark == 'hmmer':
   process = spec2k6.hmmer
elif options.benchmark == 'sjeng':
   process = spec2k6.sjeng
elif options.benchmark == 'GemsFDTD':
   process = spec2k6.GemsFDTD
elif options.benchmark == 'libquantum':
   process = spec2k6.libquantum
elif options.benchmark == 'h264ref':
   process = spec2k6.h264ref
elif options.benchmark == 'tonto':
   process = spec2k6.tonto
elif options.benchmark == 'lbm':
   process = spec2k6.lbm
elif options.benchmark == 'omnetpp':
   process = spec2k6.omnetpp
elif options.benchmark == 'astar':
   process = spec2k6.astar
elif options.benchmark == 'wrf':
   process = spec2k6.wrf
elif options.benchmark == 'sphinx3':
   process = spec2k6.sphinx3
elif options.benchmark == 'xalancbmk':
   process = spec2k6.xalancbmk
elif options.benchmark == 'specrand_i':
   process = spec2k6.specrand_i
elif options.benchmark == 'specrand_f':
   process = spec2k6.specrand_f

multiprocesses = []
numThreads = 1

(CPUClass, test_mem_mode, FutureClass) = Simulation.setCPUClass(options)
CPUClass.clock = '1.0GHz'
CPUClass.numThreads = numThreads

multiprocesses.append(process)

np = options.num_cpus
system = System(cpu = [CPUClass(cpu_id=i) for i in xrange(np)],
                physmem = SimpleMemory(range=AddrRange("512MB")),
                membus = CoherentBus(), mem_mode = test_mem_mode)

for i in xrange(np):
    if options.smt:
        system.cpu[i].workload = multiprocesses
    elif len(multiprocesses) == 1:
        system.cpu[i].workload = multiprocesses[0]
    else:
        system.cpu[i].workload = multiprocesses[i]

options.use_map = True
Ruby.create_system(options, system)
assert(options.num_cpus == len(system.ruby._cpu_ruby_ports))

for i in xrange(np):
    ruby_port = system.ruby._cpu_ruby_ports[i]

    # Create the interrupt controller and connect its ports to Ruby
    system.cpu[i].createInterruptController()
    # Connect the cpu's cache ports to Ruby
    system.cpu[i].icache_port = ruby_port.slave
    system.cpu[i].dcache_port = ruby_port.slave
    if buildEnv['TARGET_ISA'] == 'x86':
        system.cpu[i].interrupts.pio = ruby_port.master
        system.cpu[i].interrupts.int_master = ruby_port.slave
        system.cpu[i].interrupts.int_slave = ruby_port.master

        system.cpu[i].itb.walker.port = ruby_port.slave
        system.cpu[i].dtb.walker.port = ruby_port.slave


root = Root(full_system = False, system = system)
Simulation.run(options, root, system, FutureClass)
