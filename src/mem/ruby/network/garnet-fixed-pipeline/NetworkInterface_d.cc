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
 * NetworkInterface_d.C
 *
 * Niket Agarwal, Princeton University
 *
 * */

#include "mem/ruby/network/garnet-fixed-pipeline/NetworkInterface_d.hh"
#include "mem/ruby/buffers/MessageBuffer.hh"
#include "mem/ruby/network/garnet-fixed-pipeline/flitBuffer_d.hh"
#include "mem/ruby/slicc_interface/NetworkMessage.hh"

NetworkInterface_d::NetworkInterface_d(int id, int virtual_networks, GarnetNetwork_d *network_ptr)
{
        m_id = id;
        m_net_ptr = network_ptr;
        m_virtual_networks  = virtual_networks;
        m_vc_per_vnet = NetworkConfig::getVCsPerClass();
        m_num_vcs = m_vc_per_vnet*m_virtual_networks;

        m_vc_round_robin = 0;
        m_ni_buffers.setSize(m_num_vcs);
        m_ni_enqueue_time.setSize(m_num_vcs);
        inNode_ptr.setSize(m_virtual_networks);
        outNode_ptr.setSize(m_virtual_networks);
        creditQueue = new flitBuffer_d();

        for(int i =0; i < m_num_vcs; i++)
        {
                m_ni_buffers[i] = new flitBuffer_d(); // instantiating the NI flit buffers
                m_ni_enqueue_time[i] = INFINITE_;
        }
         m_vc_allocator.setSize(m_virtual_networks); // 1 allocator per virtual net
         for(int i = 0; i < m_virtual_networks; i++)
         {
                 m_vc_allocator[i] = 0;
         }

         for(int i = 0; i < m_num_vcs; i++)
         {
                m_out_vc_state.insertAtBottom(new OutVcState_d(i));
                m_out_vc_state[i]->setState(IDLE_, g_eventQueue_ptr->getTime());
         }
}

NetworkInterface_d::~NetworkInterface_d()
{
        m_out_vc_state.deletePointers();
        m_ni_buffers.deletePointers();
        delete creditQueue;
        delete outSrcQueue;
}

void NetworkInterface_d::addInPort(NetworkLink_d *in_link, CreditLink_d *credit_link)
{
        inNetLink = in_link;
        in_link->setLinkConsumer(this);
        m_ni_credit_link = credit_link;
        credit_link->setSourceQueue(creditQueue);
}

void NetworkInterface_d::addOutPort(NetworkLink_d *out_link, CreditLink_d *credit_link)
{
        m_credit_link = credit_link;
        credit_link->setLinkConsumer(this);

        outNetLink = out_link;
        outSrcQueue = new flitBuffer_d();
        out_link->setSourceQueue(outSrcQueue);
}

void NetworkInterface_d::addNode(Vector<MessageBuffer*>& in,  Vector<MessageBuffer*>& out)
{
        ASSERT(in.size() == m_virtual_networks);
        inNode_ptr = in;
        outNode_ptr = out;
        for (int j = 0; j < m_virtual_networks; j++)
        {
                inNode_ptr[j]->setConsumer(this);  // So that protocol injects messages into the NI
        }
}

bool NetworkInterface_d::flitisizeMessage(MsgPtr msg_ptr, int vnet)
{
        NetworkMessage *net_msg_ptr = dynamic_cast<NetworkMessage*>(msg_ptr.ref());
        NetDest net_msg_dest = net_msg_ptr->getInternalDestination();
        Vector<NodeID> dest_nodes = net_msg_dest.getAllDest(); // gets all the destinations associated with this message.

        int num_flits = (int) ceil((double) MessageSizeType_to_int(net_msg_ptr->getMessageSize())/NetworkConfig::getFlitSize() ); // Number of flits is dependent on the link bandwidth available. This is expressed in terms of bytes/cycle or the flit size

        for(int ctr = 0; ctr < dest_nodes.size(); ctr++) // loop because we will be converting all multicast messages into unicast messages
        {
                int vc = calculateVC(vnet); // this will return a free output virtual channel
                if(vc == -1)
                {
                        return false ;
                }
                MsgPtr new_msg_ptr = *(msg_ptr.ref());
                NodeID destID = dest_nodes[ctr];

                NetworkMessage *new_net_msg_ptr = dynamic_cast<NetworkMessage*>(new_msg_ptr.ref());
                if(dest_nodes.size() > 1)
                {
                        NetDest personal_dest;
                        for(int m = 0; m < (int) MachineType_NUM; m++)
                        {
                                if((destID >= MachineType_base_number((MachineType) m)) && destID < MachineType_base_number((MachineType) (m+1)))
                                {
                                        // calculating the NetDest associated with this destination ID
                                        personal_dest.clear();
                                        personal_dest.add((MachineID) {(MachineType) m, (destID - MachineType_base_number((MachineType) m))});
                                        new_net_msg_ptr->getInternalDestination() = personal_dest;
                                        break;
                                }
                        }
                        net_msg_dest.removeNetDest(personal_dest);
                        net_msg_ptr->getInternalDestination().removeNetDest(personal_dest); // removing the destination from the original message to reflect that a message with this particular destination has been flitisized and an output vc is acquired
                }
                for(int i = 0; i < num_flits; i++)
                {
                        m_net_ptr->increment_injected_flits();
                        flit_d *fl = new flit_d(i, vc, vnet, num_flits, new_msg_ptr);
                        fl->set_delay(g_eventQueue_ptr->getTime() - (msg_ptr.ref())->getTime());
                        m_ni_buffers[vc]->insert(fl);
                }
                m_ni_enqueue_time[vc] = g_eventQueue_ptr->getTime();
                m_out_vc_state[vc]->setState(ACTIVE_, g_eventQueue_ptr->getTime());
        }
        return true ;
}

// Looking for a free output vc
int NetworkInterface_d::calculateVC(int vnet)
{
        for(int i = 0; i < m_vc_per_vnet; i++)
        {
                int delta = m_vc_allocator[vnet];
                m_vc_allocator[vnet]++;
                if(m_vc_allocator[vnet] == m_vc_per_vnet)
                        m_vc_allocator[vnet] = 0;

                if(m_out_vc_state[(vnet*m_vc_per_vnet) + delta]->isInState(IDLE_, g_eventQueue_ptr->getTime()))
                {
                        return ((vnet*m_vc_per_vnet) + delta);
                }
        }
        return -1;
}

/*
 * The NI wakeup checks whether there are any ready messages in the protocol buffer. If yes,
 * it picks that up, flitisizes it into a number of flits and puts it into an output buffer
 * and schedules the output link. On a wakeup it also checks whether there are flits in the
 * input link. If yes, it picks them up and if the flit is a tail, the NI inserts the
 * corresponding message into the protocol buffer. It also checks for credits being sent
 * by the downstream router.
 */

void NetworkInterface_d::wakeup()
{
        DEBUG_EXPR(NETWORK_COMP, MedPrio, m_id);
        DEBUG_MSG(NETWORK_COMP, MedPrio, "NI WOKE UP");
        DEBUG_EXPR(NETWORK_COMP, MedPrio, g_eventQueue_ptr->getTime());

        MsgPtr msg_ptr;

        //Checking for messages coming from the protocol
        for (int vnet = 0; vnet < m_virtual_networks; vnet++) // can pick up a message/cycle for each virtual net
        {
                while(inNode_ptr[vnet]->isReady()) // Is there a message waiting
                {
                        msg_ptr = inNode_ptr[vnet]->peekMsgPtr();
                        if(flitisizeMessage(msg_ptr, vnet))
                        {
                                inNode_ptr[vnet]->pop();
                        }
                        else
                        {
                                break;
                        }
                }
        }

        scheduleOutputLink();
        checkReschedule();

/*********** Picking messages destined for this NI **********/

        if(inNetLink->isReady())
        {
                flit_d *t_flit = inNetLink->consumeLink();
                bool free_signal = false;
                if(t_flit->get_type() == TAIL_ || t_flit->get_type() == HEAD_TAIL_)
                {
                        free_signal = true;
                        if(!NetworkConfig::isNetworkTesting()) // When we are doing network only testing, the messages do not have to be buffered into the message buffers
                        {
                                outNode_ptr[t_flit->get_vnet()]->enqueue(t_flit->get_msg_ptr(), 1); // enqueueing for protocol buffer. This is not required when doing network only testing
                        }
                }
                flit_d *credit_flit = new flit_d(t_flit->get_vc(), free_signal); // Simply send a credit back since we are not buddering this flit in the NI
                creditQueue->insert(credit_flit);
                g_eventQueue_ptr->scheduleEvent(m_ni_credit_link, 1);

                m_net_ptr->increment_recieved_flits();
                int network_delay = g_eventQueue_ptr->getTime() - t_flit->get_enqueue_time();
                int queueing_delay = t_flit->get_delay();
                m_net_ptr->increment_network_latency(network_delay);
                m_net_ptr->increment_queueing_latency(queueing_delay);
                delete t_flit;
        }

         /****************** Checking for credit link *******/

         if(m_credit_link->isReady())
         {
                flit_d *t_flit = m_credit_link->consumeLink();
                m_out_vc_state[t_flit->get_vc()]->increment_credit();
                if(t_flit->is_free_signal())
                {
                        m_out_vc_state[t_flit->get_vc()]->setState(IDLE_, g_eventQueue_ptr->getTime());
                }
                delete t_flit;
         }
}

// This function look at the NI buffers and if some buffer has flits which are ready to traverse the link in the next cycle and also the downstream output vc associated with this flit has buffers left, the link is scheduled for the next cycle

void NetworkInterface_d::scheduleOutputLink()
{
        int vc = m_vc_round_robin;
        m_vc_round_robin++;
        if(m_vc_round_robin == m_num_vcs)
                m_vc_round_robin = 0;

        for(int i = 0; i < m_num_vcs; i++)
        {
                vc++;
                if(vc == m_num_vcs)
                        vc = 0;
                if(m_ni_buffers[vc]->isReady() && m_out_vc_state[vc]->has_credits())  // models buffer backpressure
                {
                        bool is_candidate_vc = true;
                        int t_vnet = get_vnet(vc);
                        int vc_base = t_vnet * m_vc_per_vnet;

                        if(m_net_ptr->isVNetOrdered(t_vnet))
                        {
                                for (int vc_offset = 0; vc_offset < m_vc_per_vnet; vc_offset++)
                                {
                                        int t_vc = vc_base + vc_offset;
                                        if(m_ni_buffers[t_vc]->isReady())
                                        {
                                                if(m_ni_enqueue_time[t_vc] < m_ni_enqueue_time[vc])
                                                {
                                                        is_candidate_vc = false;
                                                        break;
                                                }
                                        }
                                }
                        }
                        if(!is_candidate_vc)
                                continue;

                        m_out_vc_state[vc]->decrement_credit();
                        flit_d *t_flit = m_ni_buffers[vc]->getTopFlit();        // Just removing the flit
                        t_flit->set_time(g_eventQueue_ptr->getTime() + 1);
                        outSrcQueue->insert(t_flit);
                        g_eventQueue_ptr->scheduleEvent(outNetLink, 1); // schedule the out link

                        if(t_flit->get_type() == TAIL_ || t_flit->get_type() == HEAD_TAIL_)
                        {
                                m_ni_enqueue_time[vc] = INFINITE_;
                        }
                        return;
                }
        }
}

int NetworkInterface_d::get_vnet(int vc)
{
        for(int i = 0; i < NUMBER_OF_VIRTUAL_NETWORKS; i++)
        {
                if(vc >= (i*m_vc_per_vnet) && vc < ((i+1)*m_vc_per_vnet))
                {
                        return i;
                }
        }
        ERROR_MSG("Could not determine vc");
        return -1;
}

void NetworkInterface_d::checkReschedule()
{
        for(int vnet = 0; vnet < m_virtual_networks; vnet++)
        {
                if(inNode_ptr[vnet]->isReady()) // Is there a message waiting
                {
                        g_eventQueue_ptr->scheduleEvent(this, 1);
                        return;
                }
        }
        for(int vc = 0; vc < m_num_vcs; vc++)
        {
                if(m_ni_buffers[vc]->isReadyForNext())
                {
                        g_eventQueue_ptr->scheduleEvent(this, 1);
                        return;
                }
        }
}

void NetworkInterface_d::printConfig(ostream& out) const
{
        out << "[Network Interface " << m_id << "] - ";
        out << "[inLink " << inNetLink->get_id() << "] - ";
        out << "[outLink " << outNetLink->get_id() << "]" << endl;
}

void NetworkInterface_d::print(ostream& out) const
{
        out << "[Network Interface]";
}