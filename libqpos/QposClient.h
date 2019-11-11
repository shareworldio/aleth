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

#pragma once

#include <libethereum/Client.h>

namespace dev
{
namespace eth
{

class Qpos;

DEV_SIMPLE_EXCEPTION(NotQposSealEngine);
DEV_SIMPLE_EXCEPTION(ChainParamsNotQpos);
DEV_SIMPLE_EXCEPTION(InitQposFailed);

class QposClient: public Client
{
public:
	/// Trivial forwarding constructor.
	QposClient(
	    ChainParams const& _params,
	    int _networkID,
	    p2p::Host& _host,
	    std::shared_ptr<GasPricer> _gpForAdoption,
	    boost::filesystem::path const& _dbPath,
	    boost::filesystem::path const& _snapshotPath,
	    bool importAnyNode = false,
	    WithExisting _forceAction = WithExisting::Trust,
	    TransactionQueue::Limits const& _l = TransactionQueue::Limits {1024, 1024}
	);

	virtual ~QposClient();

	void startSealing() override;
	void stopSealing() override;
	//bool isMining() const override { return isWorking(); }
	bool wouldSeal() const override;
	
	Qpos* raft() const;
protected:
	void init(ChainParams const& _params, p2p::Host *_host);
	void rejigSealing() override;
	void reportBlocks(h256s const& _blocks) override;
	bool submitSealed(bytes const& _block, bool _isOurs);

	BlockHeader  m_last_commited_block;
	bool m_noEmptyBlock = true;
	bool m_importAnyNode;
};

QposClient& asQposClient(Interface& _c);
QposClient* asQposClient(Interface* _c);

}
}
