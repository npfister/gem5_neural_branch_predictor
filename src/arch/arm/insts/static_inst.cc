/* Copyright (c) 2007-2008 The Florida State University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Stephen Hines
 */

#include "arch/arm/insts/static_inst.hh"
#include "base/condcodes.hh"

namespace ArmISA
{
// Shift Rm by an immediate value
int32_t
ArmStaticInst::shift_rm_imm(uint32_t base, uint32_t shamt,
                            uint32_t type, uint32_t cfval) const
{
    assert(shamt < 32);
    ArmShiftType shiftType;
    shiftType = (ArmShiftType)type;

    switch (shiftType)
    {
      case LSL:
        return base << shamt;
      case LSR:
        if (shamt == 0)
            return 0;
        else
            return base >> shamt;
      case ASR:
        if (shamt == 0)
            return (int32_t)base >> 31;
        else
            return (int32_t)base >> shamt;
      case ROR:
        if (shamt == 0)
            return (cfval << 31) | (base >> 1); // RRX
        else
            return (base << (32 - shamt)) | (base >> shamt);
      default:
        fprintf(stderr, "Unhandled shift type\n");
        exit(1);
        break;
    }
    return 0;
}

// Shift Rm by Rs
int32_t
ArmStaticInst::shift_rm_rs(uint32_t base, uint32_t shamt,
                           uint32_t type, uint32_t cfval) const
{
    enum ArmShiftType shiftType;
    shiftType = (enum ArmShiftType) type;

    switch (shiftType)
    {
      case LSL:
        if (shamt >= 32)
            return 0;
        else
            return base << shamt;
      case LSR:
        if (shamt >= 32)
            return 0;
        else
            return base >> shamt;
      case ASR:
        if (shamt >= 32)
            return (int32_t)base >> 31;
        else
            return (int32_t)base >> shamt;
      case ROR:
        shamt = shamt & 0x1f;
        if (shamt == 0)
            return base;
        else
            return (base << (32 - shamt)) | (base >> shamt);
      default:
        fprintf(stderr, "Unhandled shift type\n");
        exit(1);
        break;
    }
    return 0;
}


// Generate C for a shift by immediate
bool
ArmStaticInst::shift_carry_imm(uint32_t base, uint32_t shamt,
                               uint32_t type, uint32_t cfval) const
{
    enum ArmShiftType shiftType;
    shiftType = (enum ArmShiftType) type;

    switch (shiftType)
    {
      case LSL:
        if (shamt == 0)
            return cfval;
        else
            return (base >> (32 - shamt)) & 1;
      case LSR:
        if (shamt == 0)
            return (base >> 31);
        else
            return (base >> (shamt - 1)) & 1;
      case ASR:
        if (shamt == 0)
            return (base >> 31);
        else
            return (base >> (shamt - 1)) & 1;
      case ROR:
        shamt = shamt & 0x1f;
        if (shamt == 0)
            return (base & 1); // RRX
        else
            return (base >> (shamt - 1)) & 1;
      default:
        fprintf(stderr, "Unhandled shift type\n");
        exit(1);
        break;
    }
    return 0;
}


// Generate C for a shift by Rs
bool
ArmStaticInst::shift_carry_rs(uint32_t base, uint32_t shamt,
                              uint32_t type, uint32_t cfval) const
{
    enum ArmShiftType shiftType;
    shiftType = (enum ArmShiftType) type;

    if (shamt == 0)
        return cfval;

    switch (shiftType)
    {
      case LSL:
        if (shamt > 32)
            return 0;
        else
            return (base >> (32 - shamt)) & 1;
      case LSR:
        if (shamt > 32)
            return 0;
        else
            return (base >> (shamt - 1)) & 1;
      case ASR:
        if (shamt > 32)
            shamt = 32;
        return (base >> (shamt - 1)) & 1;
      case ROR:
        shamt = shamt & 0x1f;
        if (shamt == 0)
            shamt = 32;
        return (base >> (shamt - 1)) & 1;
      default:
        fprintf(stderr, "Unhandled shift type\n");
        exit(1);
        break;
    }
    return 0;
}


// Generate the appropriate carry bit for an addition operation
bool
ArmStaticInst::arm_add_carry(int32_t result, int32_t lhs, int32_t rhs) const
{
    return findCarry(32, result, lhs, rhs);
}

// Generate the appropriate carry bit for a subtraction operation
bool
ArmStaticInst::arm_sub_carry(int32_t result, int32_t lhs, int32_t rhs) const
{
    return findCarry(32, result, lhs, ~rhs);
}

bool
ArmStaticInst::arm_add_overflow(int32_t result, int32_t lhs, int32_t rhs) const
{
    return findOverflow(32, result, lhs, rhs);
}

bool
ArmStaticInst::arm_sub_overflow(int32_t result, int32_t lhs, int32_t rhs) const
{
    return findOverflow(32, result, lhs, ~rhs);
}

void
ArmStaticInst::printReg(std::ostream &os, int reg) const
{
    if (reg < FP_Base_DepTag) {
        ccprintf(os, "r%d", reg);
    }
    else {
        ccprintf(os, "f%d", reg - FP_Base_DepTag);
    }
}

std::string
ArmStaticInst::generateDisassembly(Addr pc,
                                   const SymbolTable *symtab) const
{
    std::stringstream ss;

    ccprintf(ss, "%-10s ", mnemonic);

    return ss.str();
}
}