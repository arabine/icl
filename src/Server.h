/*=============================================================================
 * TarotClub - Server.h
 *=============================================================================
 * Server, game modes, database and statistics management
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * TarotClub is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TarotClub is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TarotClub.  If not, see <http://www.gnu.org/licenses/>.
 *
 *=============================================================================
 */

#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include "Value.h"
#include "Lobby.h"
#include "Protocol.h"
#include "JsonValue.h"
#include "BotManager.h"
#include "TcpServer.h"
#include <mutex>
#include <thread>
#include <chrono>

/*****************************************************************************/
class Server : private TcpServer::IEvent, private Lobby::IPacketNotifier
{
public:

    Server();

    void Start(const ServerOptions &options, const TournamentOptions &tournamentOpt);
    void Stop();

private:

    // From TcpServer interface
    virtual void NewConnection(const Peer &peer);
    virtual void ReadData(const Peer &peer, const ByteArray &data);
    virtual void ClientClosed(const Peer &peer);
    virtual void ServerTerminated(CloseType type);

    // From Lobby::IPacketNotifier
    virtual void Send(const ByteArray &data, std::list<std::uint32_t> peers);


    // A TarotClub server contains:
    // 1. A lobby, to manage chat between players and playing tables
    Lobby mLobby;
    // 2. A way to add opponents
    BotManager mBotManager;
    // 3. Network stuff
    int             mGamePort;
    TcpServer       mTcpServer;
    std::map<std::uint32_t, Peer> mPeers; // pair of uuid, Peer info


    void SendDataToPlayer(const ByteArray &data);
    std::uint32_t GetUuid(const Peer &peer);
    bool IsValid(std::uint32_t uuid, const Peer &peer);
};


#endif // SERVER_H

//=============================================================================
// End of file Server.h
//=============================================================================
