
#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <cstdint>
#include <iostream>

#include "tst_tarot_protocol.h"
#include "Lobby.h"

TarotProtocol::TarotProtocol()
{
}


void Dump(const std::vector<helper::Reply> &reply)
{
    for (std::uint32_t i = 0U; i < reply.size(); i++)
    {
        std::cout << "------------------------------------" << std::endl;
        std::cout << "Destination: " << (int)reply[i].dest  << std::endl;
        std::cout << reply[i].data.ToString(0U)  << std::endl;
    }
}


void TarotProtocol::TestPlayerJoinAndQuit()
{
    std::vector<std::string> tables;
    tables.push_back("test");
    Lobby lobby;

    lobby.Initialize("test", tables);

    std::vector<helper::Reply> reply;

    std::uint32_t src = Protocol::INVALID_UID;
    std::uint32_t dest = Protocol::LOBBY_UID;

    JsonObject json;

    src = lobby.AddUser(reply);
    Dump(reply);
    reply.clear();

    json.AddValue("cmd", "ReplyLogin");
    json.AddValue("nickname", "Casimir");

    QCOMPARE(lobby.Decode(src, dest, json.ToString(0U), reply), true);
    Dump(reply);
    reply.clear();
}
