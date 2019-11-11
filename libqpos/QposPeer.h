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
/** @file QposPeer.h
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 */

#pragma once

#include <mutex>
#include <array>
#include <set>
#include <memory>
#include <utility>

#include <libdevcore/RLP.h>
#include <libdevcore/Guards.h>
#include <libdevcore/SHA3.h>

#include <libp2p/Common.h>
#include <libp2p/Host.h>
#include <libp2p/Session.h>
#include <libp2p/Capability.h>
#include <libp2p/CapabilityHost.h>


#include <libethereum/EthereumCapability.h>
#include <libethereum/CommonNet.h>

#include <chrono>

#include "QposHost.h"
#include "Common.h"
#include "QposSealEngine.h"

namespace dev
{

using namespace p2p;

/*
using namespace p2p::Session;
using namespace p2p::HostCapabilityFace;
using namespace p2p::HostCapability;
using namespace p2p::Capability;
using namespace p2p::CapDesc;
*/

namespace eth
{



class QposSealEngine;
class QposHost;

class QposPeer: public CapabilityHostFace
{
	friend class QposHost;

public:
	//QposPeer(std::shared_ptr<p2p::SessionFace> _s, p2p::HostCapabilityFace* _h, unsigned _i, p2p::CapDesc const& _cap, uint16_t _capID);
	QposPeer(std::shared_ptr<p2p::SessionFace> _s, p2p::CapabilityHostFace* _h, unsigned _i, p2p::CapDesc const& _cap);

	virtual ~QposPeer();
	QposHost* host() const;

	bool sendBytes(unsigned _t, const bytes &_b);
	virtual std::string name() const { return "Qpos"; }
	virtual unsigned version() const { return c_protocolVersion; }
	virtual unsigned messageCount() { return QposMsgCount; }

	virtual char const* packetTypeToString(unsigned _packetType) { return ethPacketTypeToString(static_cast<EthSubprotocolPacketType>(_packetType)); };
    /// Time interval to run the background work loop at
    virtual std::chrono::milliseconds backgroundWorkInterval() { std::chrono::milliseconds interval{1000}; return interval; };
    /// Called by the Host when new peer is connected.
    /// Guaranteed to be called first before any interpretCapabilityPacket for this peer.
    virtual void onConnect(NodeID const& _nodeID, u256 const& _peerCapabilityVersion){(void)_nodeID; (void)_peerCapabilityVersion; };
    /// Called by the Host when the messaege is received from the peer
    /// @returns true if the message was interpreted, false if the message had not supported type.
    virtual bool interpretCapabilityPacket(NodeID const& _nodeID, unsigned _id, RLP const&);
    /// Called by the Host when the peer is disconnected.
    /// Guaranteed to be called last after any interpretCapabilityPacket for this peer.
    virtual void onDisconnect(NodeID const& _nodeID){(void)_nodeID;};
    /// Background work loop - called by the host every backgroundWorkInterval()
    virtual void doBackgroundWork(){};

	RLPStream& prep(RLPStream& _s, unsigned _id, unsigned _args = 0){ return _s.appendRaw(bytes(1, _id)).appendList(_args); };
	void sealAndSend(RLPStream& _s){(void)_s;};
	NodeID id();
protected:
	bytes toBytes(RLP const& _r);
private:
	//virtual bool interpret(unsigned _id, RLP const&) override;
	//bool interpretCapabilityPacket(NodeID const& _peerID, unsigned _id, RLP const& _r) override;

	NodeID m_id;
	QposSealEngine* m_leader = 0;
	QposHost* m_host = 0;
};

}
}
