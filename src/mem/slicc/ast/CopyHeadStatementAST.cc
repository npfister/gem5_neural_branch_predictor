
/*
 * Copyright (c) 1999-2008 Mark D. Hill and David A. Wood
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
 */

/*
 * $Id$
 *
 */

#include "mem/slicc/ast/CopyHeadStatementAST.hh"
#include "mem/slicc/symbols/SymbolTable.hh"
#include "mem/slicc/ast/VarExprAST.hh"
#include "mem/gems_common/util.hh"

CopyHeadStatementAST::CopyHeadStatementAST(VarExprAST* in_queue_ptr,
                                           VarExprAST* out_queue_ptr,
                                           PairListAST* pairs_ptr)
  : StatementAST(pairs_ptr->getPairs())
{
  m_in_queue_ptr = in_queue_ptr;
  m_out_queue_ptr = out_queue_ptr;
}

CopyHeadStatementAST::~CopyHeadStatementAST()
{
  delete m_in_queue_ptr;
  delete m_out_queue_ptr;
}

void CopyHeadStatementAST::generate(string& code, Type* return_type_ptr) const
{
  m_in_queue_ptr->assertType("InPort");
  m_out_queue_ptr->assertType("OutPort");

  code += indent_str();
  code += m_out_queue_ptr->getVar()->getCode() + ".enqueue(" + m_in_queue_ptr->getVar()->getCode() + ".getMsgPtrCopy()";

  if (getPairs().exist("latency")) {
    code += ", " + getPairs().lookup("latency");
  } else {
    code += ", COPY_HEAD_LATENCY";
  }

  code += ");\n";
}

void CopyHeadStatementAST::findResources(Map<Var*, string>& resource_list) const
{
  Var* var_ptr = m_out_queue_ptr->getVar();
  int res_count = 0;
  if (resource_list.exist(var_ptr)) {
    res_count = atoi((resource_list.lookup(var_ptr)).c_str());
  }
  resource_list.add(var_ptr, int_to_string(res_count+1));
}

void CopyHeadStatementAST::print(ostream& out) const
{
  out << "[CopyHeadStatementAst: " << *m_in_queue_ptr << " " << *m_out_queue_ptr << "]";
}