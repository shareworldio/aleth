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
/** @file QposHost.h
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
#include <libdevcore/Worker.h>
#include <libdevcore/Guards.h>
#include <libdevcore/SHA3.h>

#include <libp2p/Common.h>
#include <libp2p/Host.h>
#include <libp2p/Session.h>
#include <libp2p/Capability.h>
#include <libp2p/CapabilityHost.h>


#include <libethereum/EthereumCapability.h>
#include <libethereum/CommonNet.h>


#include "QposPeer.h"
#include "QposSealEngine.h"

namespace dev
{
namespace eth
{

class QposSealEngine;

class QposHost : public p2p::CapabilityFace
{
	friend class QposPeer;

public:
	QposHost(QposSealEngine* leader);
	virtual ~QposHost();

	std::string name() const override { return "qpos";};
    /// Subprotocol version, used in negotiating common capabilities with the peers.
    unsigned version() const override { return 1; };
    /// Combination of name and version
    CapDesc descriptor() const override { return {name(), version()}; };
    /// Number of messages supported by the subprotocol version.
    unsigned messageCount() const override { return PacketCount; };
    /// Convert supplied packet type to string - used for logging purposes
    char const* packetTypeToString(unsigned _packetType) const override;
    /// Time interval to run the background work loop at
    std::chrono::milliseconds backgroundWorkInterval() const override { return std::chrono::milliseconds{1000}; };
    /// Called by the Host when new peer is connected.
    /// Guaranteed to be called first before any interpretCapabilityPacket for this peer.
    void onConnect(NodeID const& _nodeID, u256 const& _peerCapabilityVersion) override;
    /// Called by the Host when the messaege is received from the peer
    /// @returns true if the message was interpreted, false if the message had not supported type.
    bool interpretCapabilityPacket(NodeID const& _nodeID, unsigned _id, RLP const&) override;
    /// Called by the Host when the peer is disconnected.
    /// Guaranteed to be called last after any interpretCapabilityPacket for this peer.
    void onDisconnect(NodeID const& _nodeID) override;
    /// Background work loop - called by the host every backgroundWorkInterval()
    void doBackgroundWork() override;

	QposSealEngine* Leader() {return m_leader;}
protected:
	

private:
	QposSealEngine* m_leader = 0;
};

}
}
