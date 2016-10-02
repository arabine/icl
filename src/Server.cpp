/*=============================================================================
 * TarotClub - Server.cpp
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

#include <cctype>
#include <iostream>
#include <string>
#include <sstream>
#include "Util.h"
#include "Server.h"
#include "System.h"


/*****************************************************************************/
Server::Server()
    : mServer(mLobby)
    , mStopRequested(false)
    , mGamePort(ServerConfig::DEFAULT_GAME_TCP_PORT)
{

}
/*****************************************************************************/
void Server::Start(const ServerOptions &opt, const TournamentOptions &tournamentOpt)
{
    (void) tournamentOpt;

    // Init lobby
    mLobby.Initialize(opt.name, opt.tables);
    mLobby.Register(this);

    // Initialize all the tables, starting with the TCP port indicated
    mGamePort = opt.game_tcp_port;
    mTcpServer.Start(opt.lobby_max_conn, opt.localHostOnly, opt.game_tcp_port, 4270); // FIXME: make the WS port configurable

    // Init server
    mGamePort = options.game_tcp_port;
}
/*****************************************************************************/
void Server::Stop()
{
    // Properly stop the threads
    mBotManager.Close();
    mServer.Stop();
    mServer.WaitForEnd();
}
/*****************************************************************************/
void Server::NewConnection(const Peer &peer)
{
    std::uint32_t uuid = mLobby.AddUser(mTcpServer.GetPeerName(peer.socket));
    mPeers[uuid] = peer;
}
/*****************************************************************************/

/**
 * @brief Server::ReadData
 *
 * This callback is executed within the Tcp context. Here we are receiving data
 * from peers.
 */
void Server::ReadData(const Peer &peer, const std::string &data)
{
    Protocol proto;

    if (proto.Parse(data))
    {
        // If it is a valid player (already connected, send the packet to the lobby
        if (IsValid(proto.GetSourceUuid(), peer))
        {
            mLobby.Decode(proto.GetSourceUuid(), proto.GetDestUuid(), proto.GetArg());
        }
        else
        {
            TLogNetwork("Invalid packet received");
        }
    }
}
/*****************************************************************************/
void Server::ClientClosed(const Peer &peer)
{
    std::uint32_t uuid = GetUuid(peer);

    mLobby.RemoveUser(uuid);
}
/*****************************************************************************/
void Server::ServerTerminated(TcpServer::IEvent::CloseType type)
{
    (void) type;
    TLogError("Server terminated (internal error)");
}
/*****************************************************************************/
void Server::SendData(const JsonValue &data, std::uint32_t src_uuid, std::uint32_t dest_uuid)
{
    std::vector<std::uint32_t> peers; // list of users to send the data

    if (mTableIds.IsTaken(dest_uuid))
    {
        // If the player is connected to a table, send data to the table players only
        peers = mUsers.GetTablePlayers(dest_uuid);
    }
    else if (dest_uuid == Protocol::LOBBY_UID)
    {
        // Send data to all the connected users
        peers = mUsers.GetTablePlayers(Protocol::LOBBY_UID);
    }
    else if (mUsers.IsHere(dest_uuid))
    {
        // Only send the data to one connected client
        peers.push_back(dest_uuid);
    }


    // Send the data to a(all) peer(s)
    for (std::uint32_t i = 0U; i < peers.size(); i++)
    {
        std::uint32_t uuid = peers[i];
        if (mPeers.count(uuid) > 0)
        {
            TcpSocket::Send(Protocol::Build(0U, src_uuid, dest_uuid, "tarot", data), mPeers.at(uuid));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20U));
    }
}
/*****************************************************************************/
void Server::Stop()
{
    mInitialized = false;
    CloseClients();
    mTcpServer.Stop();
    mLobby.RemoveAllUsers();
}
/*****************************************************************************/
void Server::CloseClients()
{
    for (std::map<std::uint32_t, Peer>::iterator iter = mPeers.begin(); iter != mPeers.end(); ++iter)
    {
        TcpSocket::Close(iter->second);
    }
}
/*****************************************************************************/
bool Server::IsValid(std::uint32_t uuid, const Peer &peer)
{
    bool valid = false;

    if (mPeers.count(uuid) > 0U)
    {
        if (mPeers[uuid] == peer)
        {
            valid = true;
        }
    }
    return valid;
}
/*****************************************************************************/
std::uint32_t Server::GetUuid(const Peer &peer)
{
    std::uint32_t uuid = Protocol::INVALID_UID;
    for (std::map<std::uint32_t, Peer>::iterator iter = mPeers.begin(); iter != mPeers.end(); ++iter)
    {
        if (iter->second == peer)
        {
            uuid = iter->first;
        }
    }
    return uuid;
}



//=============================================================================
// End of file Server.cpp
//=============================================================================
