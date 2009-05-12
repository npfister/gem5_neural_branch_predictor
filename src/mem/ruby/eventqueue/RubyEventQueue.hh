
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
 * Description: The EventQueue class implements an event queue which
 * can be trigger events, allowing our simulation to be event driven.
 *
 * Currently, the only event we support is a Consumer being signaled
 * by calling the consumer's wakeup() routine.  Adding the event to
 * the queue does not require a virtual function call, though calling
 * wakeup() is a virtual function call.
 *
 * The method triggerEvents() is called with a global time.  All
 * events which are before or at this time are triggered in timestamp
 * order.  No ordering is enforced for events scheduled to occur at
 * the same time.  Events scheduled to wakeup the same consumer at the
 * same time are combined into a single event.
 *
 * The method scheduleConsumerWakeup() is called with a global time
 * and a consumer pointer.  The event queue will call the wakeup()
 * method of the consumer at the appropriate time.
 *
 * This implementation of EventQueue uses a dynamically sized array
 * managed as a heap.  The algorithms used has O(lg n) for insert and
 * O(lg n) for extract minimum element. (Based on chapter 7 of Cormen,
 * Leiserson, and Rivest.)  The array is dynamically sized and is
 * automatically doubled in size when necessary.
 *
 */

#ifndef RUBYEVENTQUEUE_H
#define RUBYEVENTQUEUE_H

#include "config/no_vector_bounds_checks.hh"
#include "mem/ruby/common/Global.hh"
#include "mem/gems_common/Vector.hh"

class Consumer;
template <class TYPE> class PrioHeap;
class RubyEventQueueNode;

class RubyEventQueue {
public:
  // Constructors
  RubyEventQueue();

  // Destructor
  ~RubyEventQueue();

  // Public Methods

  Time getTime() const { return m_globalTime; }
  void scheduleEvent(Consumer* consumer, Time timeDelta) { scheduleEventAbsolute(consumer, timeDelta + m_globalTime); }
  void scheduleEventAbsolute(Consumer* consumer, Time timeAbs);
  void triggerEvents(Time t); // called to handle all events <= time t
  void triggerAllEvents();
  void print(ostream& out) const;
  bool isEmpty() const;

  Time getTimeOfLastRecovery() {return m_timeOfLastRecovery;}
  void setTimeOfLastRecovery(Time t) {m_timeOfLastRecovery = t;}

  // Private Methods
private:
  // Private copy constructor and assignment operator
  void init();
  RubyEventQueue(const RubyEventQueue& obj);
  RubyEventQueue& operator=(const RubyEventQueue& obj);

  // Data Members (m_ prefix)
  PrioHeap<RubyEventQueueNode>* m_prio_heap_ptr;
  Time m_globalTime;
  Time m_timeOfLastRecovery;
};

// Output operator declaration
inline extern
ostream& operator<<(ostream& out, const RubyEventQueue& obj);

// ******************* Definitions *******************

// Output operator definition
inline extern
ostream& operator<<(ostream& out, const RubyEventQueue& obj)
{
  obj.print(out);
  out << flush;
  return out;
}

#endif //EVENTQUEUE_H