
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
 * $Id: NodePersistentTable.C 1.3 04/08/16 14:12:33-05:00 beckmann@c2-143.cs.wisc.edu $
 *
 */

#include "mem/ruby/system/NodePersistentTable.hh"
#include "mem/ruby/common/Set.hh"
#include "mem/gems_common/Map.hh"
#include "mem/ruby/common/Address.hh"
#include "mem/ruby/slicc_interface/AbstractChip.hh"
#include "mem/gems_common/util.hh"

// randomize so that handoffs are not locality-aware
// int persistent_randomize[] = {0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15};
int persistent_randomize[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};


class NodePersistentTableEntry {
public:
  Set m_starving;
  Set m_marked;
  Set m_request_to_write;
};

NodePersistentTable::NodePersistentTable(AbstractChip* chip_ptr, int version)
{
  m_chip_ptr = chip_ptr;
  m_map_ptr = new Map<Address, NodePersistentTableEntry>;
  m_version = version;
}

NodePersistentTable::~NodePersistentTable()
{
  delete m_map_ptr;
  m_map_ptr = NULL;
  m_chip_ptr = NULL;
}

void NodePersistentTable::persistentRequestLock(const Address& address, NodeID llocker, AccessType type)
{

  // if (locker == m_chip_ptr->getID()  )
  // cout << "Chip " << m_chip_ptr->getID() << ": " << llocker << " requesting lock for " << address << endl;

  NodeID locker = (NodeID) persistent_randomize[llocker];

  assert(address == line_address(address));
  if (!m_map_ptr->exist(address)) {
    // Allocate if not present
    NodePersistentTableEntry entry;
    entry.m_starving.add(locker);
    if (type == AccessType_Write) {
      entry.m_request_to_write.add(locker);
    }
    m_map_ptr->add(address, entry);
  } else {
    NodePersistentTableEntry& entry = m_map_ptr->lookup(address);
    assert(!(entry.m_starving.isElement(locker))); // Make sure we're not already in the locked set

    entry.m_starving.add(locker);
    if (type == AccessType_Write) {
      entry.m_request_to_write.add(locker);
    }
    assert(entry.m_marked.isSubset(entry.m_starving));
  }
}

void NodePersistentTable::persistentRequestUnlock(const Address& address, NodeID uunlocker)
{
  // if (unlocker == m_chip_ptr->getID() )
  // cout << "Chip " << m_chip_ptr->getID() << ": " << uunlocker << " requesting unlock for " << address << endl;

  NodeID unlocker = (NodeID) persistent_randomize[uunlocker];

  assert(address == line_address(address));
  assert(m_map_ptr->exist(address));
  NodePersistentTableEntry& entry = m_map_ptr->lookup(address);
  assert(entry.m_starving.isElement(unlocker)); // Make sure we're in the locked set
  assert(entry.m_marked.isSubset(entry.m_starving));
  entry.m_starving.remove(unlocker);
  entry.m_marked.remove(unlocker);
  entry.m_request_to_write.remove(unlocker);
  assert(entry.m_marked.isSubset(entry.m_starving));

  // Deallocate if empty
  if (entry.m_starving.isEmpty()) {
    assert(entry.m_marked.isEmpty());
    m_map_ptr->erase(address);
  }
}

bool NodePersistentTable::okToIssueStarving(const Address& address) const
{
  assert(address == line_address(address));
  if (!m_map_ptr->exist(address)) {
    return true; // No entry present
  } else if (m_map_ptr->lookup(address).m_starving.isElement(m_chip_ptr->getID())) {
    return false; // We can't issue another lockdown until are previous unlock has occurred
  } else {
    return (m_map_ptr->lookup(address).m_marked.isEmpty());
  }
}

NodeID NodePersistentTable::findSmallest(const Address& address) const
{
  assert(address == line_address(address));
  assert(m_map_ptr->exist(address));
  const NodePersistentTableEntry& entry = m_map_ptr->lookup(address);
  // cout << "Node " <<  m_chip_ptr->getID() << " returning " << persistent_randomize[entry.m_starving.smallestElement()] << " for findSmallest(" << address << ")" << endl;
  return (NodeID) persistent_randomize[entry.m_starving.smallestElement()];
}

AccessType NodePersistentTable::typeOfSmallest(const Address& address) const
{
  assert(address == line_address(address));
  assert(m_map_ptr->exist(address));
  const NodePersistentTableEntry& entry = m_map_ptr->lookup(address);
  if (entry.m_request_to_write.isElement(entry.m_starving.smallestElement())) {
    return AccessType_Write;
  } else {
    return AccessType_Read;
  }
}

void NodePersistentTable::markEntries(const Address& address)
{
  assert(address == line_address(address));
  if (m_map_ptr->exist(address)) {
    NodePersistentTableEntry& entry = m_map_ptr->lookup(address);
    assert(entry.m_marked.isEmpty());  // None should be marked
    entry.m_marked = entry.m_starving; // Mark all the nodes currently in the table
  }
}

bool NodePersistentTable::isLocked(const Address& address) const
{
  assert(address == line_address(address));
  // If an entry is present, it must be locked
  return (m_map_ptr->exist(address));
}

int NodePersistentTable::countStarvingForAddress(const Address& address) const
{
  if (m_map_ptr->exist(address)) {
    NodePersistentTableEntry& entry = m_map_ptr->lookup(address);
    return (entry.m_starving.count());
  }
  else {
    return 0;
  }
}

int NodePersistentTable::countReadStarvingForAddress(const Address& address) const
{
  if (m_map_ptr->exist(address)) {
    NodePersistentTableEntry& entry = m_map_ptr->lookup(address);
    return (entry.m_starving.count() - entry.m_request_to_write.count());
  }
  else {
    return 0;
  }
}

