/*=============================================================================
 * TarotClub - Table.h
 *=============================================================================
 * The table manages the Tarot thread server
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
#ifndef TABLE_H
#define TABLE_H

#include <map>
#include "TcpServer.h"
#include "Controller.h"
#include "Bot.h"
#include "UniqueId.h"

/*****************************************************************************/
class Table : public TcpServer::IEvent
{

public:
    Table();

    // Helpers
    void LoadConfiguration();
    void CreateGame(Game::Mode gameMode, int nbPlayers, const Game::Shuffle &shuffle);
    void StartDeal();
    void Initialize();
    void Stop();
    void ConnectBots();

    // Getters
    ServerOptions &GetOptions();

    // Setters
    void SaveConfiguration(const ServerOptions &opt);

    // TcpServer class events
    virtual void NewConnection(int socket);
    virtual void ReadData(const std::string &data);
    virtual void ClientClosed(int socket);

private:
    class ControllerListener : public Observer<Controller::Signal>
    {
    public:
        ControllerListener(Table &i_table)
            : mTable(i_table)
        {

        }

        void Update(const Controller::Signal &info)
        {
            if (info.type == Controller::SIG_SEND_DATA)
            {
                mTable.SendToSocket(info.data);
            }
            else if (info.type == Controller::SIG_GAME_FULL)
            {
                if (mTable.mAutoStart)
                {
                    mTable.StartDeal();
                }
            }
        }

    private:
        Table &mTable;
    };

    friend class ControllerListener;

    void SendToSocket(const ByteArray &packet);
    void StopServer();
    void CloseClients();

    ControllerListener mControllerListener;
    int             mTcpPort;
    Controller      mController;
    ServerConfig    serverConfig;
    Bot             mBots[3];
    UniqueId        mIdManager;
    TcpServer       mTcpServer;
    bool            mAutoStart;

    // Pair of UUID and socket
    std::map<std::uint32_t, std::int32_t> mUsers;
    std::mutex mUsersMutex;
};

#endif // TABLE_H

//=============================================================================
// End of file Table.h
//=============================================================================
