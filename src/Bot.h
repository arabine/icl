/*=============================================================================
 * TarotClub - Bot.h
 *=============================================================================
 * Bot class player. Uses a Script Engine to execute IA scripts
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

#ifndef BOT_H
#define BOT_H

#include "NetHelper.h"
#include "JSEngine.h"
#include "Log.h"

/*****************************************************************************/
class Bot
{

public:
    Bot();
    virtual ~Bot();

    void SetTimeBeforeSend(std::uint16_t t);
    void SetIdentity(const std::string &nickname, std::vector<helper::Reply> &out);
    void SetAiScript(const std::string &path);
    void ConnectToHost(const std::string &hostName, std::uint16_t port);
    void SetTableToJoin(std::uint32_t table) { mTableToJoin = table; }
    std::uint32_t GetUuid() { return mClient.mUuid; }
    void JoinTable(std::uint32_t tableId);

private:
    helper::BasicClient mClient;
    std::uint16_t  mTimeBeforeSend;
    JSEngine mBotEngine;
    std::uint32_t mTableToJoin;
    std::string mScriptPath;

    bool InitializeScriptContext();
};

#endif // BOT_H

//=============================================================================
// End of file Bot.h
//=============================================================================
