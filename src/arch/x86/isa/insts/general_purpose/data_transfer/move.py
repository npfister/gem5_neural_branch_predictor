# Copyright (c) 2007 The Hewlett-Packard Development Company
# All rights reserved.
#
# Redistribution and use of this software in source and binary forms,
# with or without modification, are permitted provided that the
# following conditions are met:
#
# The software must be used only for Non-Commercial Use which means any
# use which is NOT directed to receiving any direct monetary
# compensation for, or commercial advantage from such use.  Illustrative
# examples of non-commercial use are academic research, personal study,
# teaching, education and corporate research & development.
# Illustrative examples of commercial use are distributing products for
# commercial advantage and providing services using the software for
# commercial advantage.
#
# If you wish to use this software or functionality therein that may be
# covered by patents for commercial use, please contact:
#     Director of Intellectual Property Licensing
#     Office of Strategy and Technology
#     Hewlett-Packard Company
#     1501 Page Mill Road
#     Palo Alto, California  94304
#
# Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.  Redistributions
# in binary form must reproduce the above copyright notice, this list of
# conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.  Neither the name of
# the COPYRIGHT HOLDER(s), HEWLETT-PACKARD COMPANY, nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.  No right of
# sublicense is granted herewith.  Derivatives of the software and
# output created using the software may be prepared, but only for
# Non-Commercial Uses.  Derivatives of the software may be shared with
# others provided: (i) the others agree to abide by the list of
# conditions herein which includes the Non-Commercial Use restrictions;
# and (ii) such Derivatives of the software include the above copyright
# notice to acknowledge the contribution from this software where
# applicable, this list of conditions and the disclaimer below.
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
# Authors: Gabe Black

microcode = '''

#
# Regular moves
#

def macroop MOV_R_R {
    mov reg, reg, regm
};

def macroop MOV_M_R {
    st reg, seg, sib, disp
};

def macroop MOV_P_R {
    rdip t7
    st reg, seg, riprel, disp
};

def macroop MOV_R_M {
    ld reg, seg, sib, disp
};

def macroop MOV_R_P {
    rdip t7
    ld reg, seg, riprel, disp
};

def macroop MOV_R_I {
    limm reg, imm
};

def macroop MOV_M_I {
    limm t1, imm
    st t1, seg, sib, disp
};

def macroop MOV_P_I {
    rdip t7
    limm t1, imm
    st t1, seg, riprel, disp
};

#
# Sign extending moves
#

def macroop MOVSXD_R_R {
    sext reg, regm, 32
};

def macroop MOVSXD_R_M {
    ld t1, seg, sib, disp, dataSize=4
    sext reg, t1, 32
};

def macroop MOVSXD_R_P {
    rdip t7
    ld t1, seg, riprel, disp, dataSize=4
    sext reg, t1, 32
};

def macroop MOVSX_B_R_R {
    sext reg, regm, 8
};

def macroop MOVSX_B_R_M {
    ld reg, seg, sib, disp, dataSize=1
    sext reg, reg, 8
};

def macroop MOVSX_B_R_P {
    rdip t7
    ld reg, seg, riprel, disp, dataSize=1
    sext reg, reg, 8
};

def macroop MOVSX_W_R_R {
    sext reg, regm, 16
};

def macroop MOVSX_W_R_M {
    ld reg, seg, sib, disp, dataSize=2
    sext reg, reg, 16
};

def macroop MOVSX_W_R_P {
    rdip t7
    ld reg, seg, riprel, disp, dataSize=2
    sext reg, reg, 16
};

#
# Zero extending moves
#

def macroop MOVZX_B_R_R {
    zext reg, regm, 8
};

def macroop MOVZX_B_R_M {
    ld t1, seg, sib, disp, dataSize=1
    zext reg, t1, 8
};

def macroop MOVZX_B_R_P {
    rdip t7
    ld t1, seg, riprel, disp, dataSize=1
    zext reg, t1, 8
};

def macroop MOVZX_W_R_R {
    zext reg, regm, 16
};

def macroop MOVZX_W_R_M {
    ld t1, seg, sib, disp, dataSize=2
    zext reg, t1, 16
};

def macroop MOVZX_W_R_P {
    rdip t7
    ld t1, seg, riprel, disp, dataSize=2
    zext reg, t1, 16
};
'''
#let {{
#    class MOVD(Inst):
#	"GenFault ${new UnimpInstFault}"
#    class MOVNTI(Inst):
#	"GenFault ${new UnimpInstFault}"
#}};