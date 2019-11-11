/*
	This file is part of cpp-ethereum.

	cpp-ethereum is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	cpp-ethereum is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with cpp-ethereum.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @file QposHost.cpp
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 */
 
#include "QposPeer.h"
#include "QposHost.h"

#include <libdevcore/CommonIO.h>
#include <libdevcore/Log.h>
#include <libp2p/All.h>

using namespace std;
using namespace dev;
using namespace dev::p2p;
using namespace dev::eth;

QposHost::QposHost(QposSealEngine* leader)
{
	m_leader = leader;
}

QposHost::~QposHost()
{
}

char const* QposHost::packetTypeToString(unsigned _packetType) const
{
	switch (_packetType)
    {
    case qposBlockVote:
        return "qposBlockVote";
    case qposBlockVoteAck:
        return "qposBlockVoteAck";
    case qposVote:
        return "qposVote";
    case qposVoteAck:
        return "qposVoteAck";
    case qposHeart:
        return "qposHeart";
    case qposBroadBlock:
        return "qposBroadBlock";
    case QposTestPacket:
        return "QposTestPacket";
    default:
        return "UnknownQposPacket";
    }
}

bool QposHost::interpretCapabilityPacket(NodeID const& _nodeID, unsigned _id, RLP const&)
{
	(void)_nodeID;
	(void)_id;
	return true;
}

void QposHost::onConnect(NodeID const& _nodeID, u256 const& _peerCapabilityVersion)
{
	(void)_nodeID;
	(void)_peerCapabilityVersion;
}

void QposHost::onDisconnect(NodeID const& _nodeID)
{
	(void)_nodeID;
}

void QposHost::doBackgroundWork()
{

}


