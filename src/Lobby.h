/*=============================================================================
 * TarotClub - Lobby.h
 *=============================================================================
 * Central meeting point of a server to chat and join game tables
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

#ifndef LOBBY_H
#define LOBBY_H

#include "Protocol.h"
#include "PlayingTable.h"
#include "Users.h"
#include "Observer.h"

/*****************************************************************************/
class Lobby
{

public:
    class IPacketNotifier
    {
    public:
        virtual void Send(const std::string &data, std::uint32_t src_uuid, std::uint32_t dest_uuid, std::vector<std::uint32_t> peers) = 0;
    };

    Lobby();
    ~Lobby();

    void Initialize(const std::string &name, const std::vector<std::string> &tables);
    void Register(IPacketNotifier * notifier);
    std::string GetName() { return mName; }
    bool Decode(uint32_t src_uuid, uint32_t dest_uuid, std::string &arg);

    // Users management
    std::uint32_t GetNumberOfPlayers();
    std::uint32_t AddUser(const std::string &ip);
    void RemoveUser(std::uint32_t uuid);
    void RemoveAllUsers();

    // Tables management
    std::uint32_t CreateTable(const std::string &tableName, bool adminMode = true, const Tarot::Game &game = Tarot::Game());
    bool DestroyTable(std::uint32_t id);

private:
    std::vector<IPacketNotifier *> mNotifiers;
    bool mInitialized;
    std::vector<PlayingTable *> mTables;
    UniqueId    mTableIds;
    Users       mUsers;
    std::string mName;

    std::string GetTableName(const std::uint32_t tableId);
    void RemovePlayerFromTable(std::uint32_t uuid, std::uint32_t tableId);
    void SendData(const JsonValue &data, uint32_t src_uuid, uint32_t dest_uuid);
    void SendPlayerList(const std::vector<uint32_t> &players, const std::string &event);
};

#endif // LOBBY_H
