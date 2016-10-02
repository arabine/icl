
#include "NetHelper.h"
#include "Protocol.h"
#include "JsonReader.h"
#include "Log.h"

namespace helper
{

/*****************************************************************************/
bool BasicClient::TestDiscard(const Deck &discard)
{
    return Player::TestDiscard(discard, mDog, mNbPlayers);
}
/*****************************************************************************/
Contract BasicClient::CalculateBid()
{
    int total = 0;
    Contract cont;

    UpdateStatistics();

    // Set points according to the card values
    if (mStats.bigTrump == true)
    {
        total += 9;
    }
    if (mStats.fool == true)
    {
        total += 7;
    }
    if (mStats.littleTrump == true)
    {
        if (mStats.trumps == 5)
        {
            total += 5;
        }
        else if (mStats.trumps == 6 || mStats.trumps == 7)
        {
            total += 7;
        }
        else if (mStats.trumps > 7)
        {
            total += 8;
        }
    }

    // Each trump is 1 point
    // Each major trump is 1 more point
    total += mStats.trumps * 2;
    total += mStats.majorTrumps * 2;
    total += mStats.kings * 6;
    total += mStats.queens * 3;
    total += mStats.knights * 2;
    total += mStats.jacks;
    total += mStats.weddings;
    total += mStats.longSuits * 5;
    total += mStats.cuts * 5;
    total += mStats.singletons * 3;
    total += mStats.sequences * 4;

    // We can decide the bid
    if (total <= 35)
    {
        cont = Contract::PASS;
    }
    else if (total >= 36  && total <= 50)
    {
        cont = Contract::TAKE;
    }
    else if (total >= 51  && total <= 65)
    {
        cont = Contract::GUARD;
    }
    else if (total >= 66  && total <= 75)
    {
        cont = Contract::GUARD_WITHOUT;
    }
    else
    {
        cont = Contract::GUARD_AGAINST;
    }
    return cont;
}
/*****************************************************************************/
void BasicClient::UpdateStatistics()
{
    mStats.Reset();
    AnalyzeTrumps(mStats);
    AnalyzeSuits(mStats);
}
/*****************************************************************************/
Card BasicClient::Play()
{
    Card c;

    for (Deck::ConstIterator it = Begin(); it != End(); ++it)
    {
        c = (*it);
        if (IsValid(c) == true)
        {
            break;
        }
    }
    return c;
}
/*****************************************************************************/
bool BasicClient::IsValid(const Card &c)
{
    return Player::CanPlayCard(c, mCurrentTrick);
}
/*****************************************************************************/
Deck BasicClient::AutoDiscard()
{
    Deck discard = Player::AutoDiscard(mDog, mNbPlayers);
    Player::RemoveDuplicates(discard);
    return discard;
}
/*****************************************************************************/
void BasicClient::BuildChangeNickname(std::vector<helper::Reply> &out)
{
    JsonObject obj;

    obj.AddValue("cmd", "ReqChangeNickname");
    obj.AddValue("nickname", mNickName);

    out.push_back(helper::Reply(Protocol::LOBBY_UID, obj));
}
/*****************************************************************************/
BasicClient::Event BasicClient::Decode(uint32_t src_uuid, uint32_t dest_uuid, const std::string &arg, IContext &ctx, std::vector<helper::Reply> &out)
{
    BasicClient::Event event = BAD_EVENT;
    JsonReader reader;
    JsonValue json;

    if (!reader.ParseString(json, arg))
    {
        TLogNetwork("Not a JSON data");
        return JSON_ERROR;
    }

    std::string cmd = json.FindValue("cmd").GetString();

    if (cmd == "RequestLogin")
    {
        ctx.Initialize();

        mUuid = static_cast<std::uint32_t>(json.FindValue("cmd").GetInteger());

        JsonObject obj;

        obj.AddValue("cmd", "ReplyLogin");
        obj.AddValue("nickname", mNickName);

        out.push_back(helper::Reply(dest_uuid, obj));

        event = REQ_LOGIN;
    }
    else if (cmd == "AccessGranted")
    {
        JsonArray tables = json.FindValue("tables").GetArray();

        for (std::uint32_t i = 0U; i < tables.Size(); i++)
        {
           JsonObject table = tables.GetEntry(i).GetObject();
           std::uint32_t uuid = static_cast<std::uint32_t>(table.GetValue("uuid").GetInteger());
           std::string name = table.GetValue("name").GetString();
           ctx.AddTable(name, uuid);
        }

        event = ACCESS_GRANTED;
    }
    else if (cmd == "ChatMessage")
    {
        IContext::Message msg;
        msg.src = static_cast<std::uint32_t>(json.FindValue("source").GetInteger());
        msg.dst = static_cast<std::uint32_t>(json.FindValue("target").GetInteger());
        msg.msg = table.GetValue("message").GetString();

        event = MESSAGE;
    }
    else if (cmd == "PlayersList")
    {
        JsonArray players = json.FindValue("players").GetArray();

        for (std::uint32_t i = 0U; i < players.Size(); i++)
        {
            IContext::Member member;
            JsonObject player = tables.GetEntry(i).GetObject();

            std::uint32_t uuid = static_cast<std::uint32_t>(player.GetValue("uuid").GetInteger());
            member.table = static_cast<std::uint32_t>(player.GetValue("table").GetInteger());
            member.name = player.GetValue("nickname").GetString();
            std::string event = player.GetValue("event").GetString();
            ctx.UpdateMember(uuid, member, event);
        }

        event = PLAYER_LIST;


/*

        QJsonArray players = object["players"].toArray();
        mLobbyUsers.clear();
        for (int i = 0; i < players.size(); i++)
        {
           QJsonObject player = players[i].toObject();
           std::uint32_t uuid = static_cast<std::uint32_t>(player["uuid"].toInt());
           Identity identity;

           identity.avatar = player["avatar"].toString().toStdString();
           identity.nickname = player["nickname"].toString().toStdString();
           identity.gender = static_cast<std::uint8_t>(player["gender"].toInt());
           mLobbyUsers[uuid] = identity;
        }

        emit sigLobbyPlayersList();
        */
    }

#if 0
    else if (cmd == "AdminGameFull")
    {
        // FIXME: parameter "full" can be used to display specific things on the GUI
        mNet.SendPacket(Protocol::AdminNewGame(mClient.mGame, mClient.GetUuid(), mClient.mTableId));
    }
    else if (cmd == "TableJoinEvent")
    {
        mClient.mPlace = Place(object["place"].toString().toStdString());
        mClient.mTableId = static_cast<std::uint32_t>(object["table"].toInt());
        mClient.mNbPlayers = static_cast<std::uint8_t>(object["table"].toInt());

        emit sigTableJoinEvent(mClient.mTableId);
        AddBots();
        mNet.SendPacket(Protocol::ClientSyncJoinTable(mClient.GetUuid(), mClient.mTableId));
    }
    else if (cmd == "TableQuitEvent")
    {
        std::uint32_t tableId = static_cast<std::uint32_t>(object["table"].toInt());
        emit sigTableQuitEvent(tableId);
        // We have been kicked from the table OR this is a simple acknowlegment message
        // Clean canvas
        InitScreen(true);
    }
    else if (cmd == "TablePlayersList")
    {
        QJsonArray players = object["players"].toArray();
        mTablePlayers.clear();
        for (int i = 0; i < players.size(); i++)
        {
           QJsonObject player = players[i].toObject();

           std::uint32_t uuid = static_cast<std::uint32_t>(player["uuid"].toInt());
           Place place = Place(player["place"].toString().toStdString());
           mTablePlayers[place] = uuid;
        }

        emit sigTablePlayersList();
        mCanvas->SetPlayerIdentity(GetTablePlayersList(), mClient.mPlace);
    }
    else if (cmd == "NewGame")
    {
        mClient.mGame.mode = static_cast<std::uint8_t>(object["mode"].toInt());

        emit sigNewGame();
        mNet.SendPacket(Protocol::ClientSyncNewGame(mClient.GetUuid(), mClient.mTableId));
    }
    else if (cmd == "NewDeal")
    {
        mClient.SetCards(object["cards"].toString().toStdString());
        mClient.UpdateStatistics();
        emit sigNewDeal();
        mCanvas->ResetCards();
        ShowSouthCards();
        mNet.SendPacket(Protocol::ClientSyncNewDeal(mClient.GetUuid(), mClient.mTableId));
    }
    else if (cmd == "RequestBid")
    {
        Place place = Place(object["place"].toString().toStdString());
        Contract contract = Contract(object["contract"].toString().toStdString());

        mCanvas->ShowSelection(place, mClient.mPlace);
        if (place == mClient.mPlace)
        {
            if (mAutoPlay)
            {
                mNet.SendPacket(Protocol::ClientBid(Contract::PASS, false, mClient.GetUuid(), mClient.mTableId));
            }
            else
            {
                mCanvas->ShowBidsChoice(contract);
                mCanvas->SetFilter(Canvas::MENU);
            }
        }
    }
    else if (cmd == "ShowBid")
    {
        Place place = Place(object["place"].toString().toStdString());
        Contract contract = Contract(object["contract"].toString().toStdString());
        bool slam = object["slam"].toBool();

        // FIXME: show the announced slam on the screen
        Q_UNUSED(slam);

        mCanvas->ShowBid(place, contract, mClient.mPlace);
        mNet.SendPacket(Protocol::ClientSyncBid(mClient.GetUuid(), mClient.mTableId));
    }
    else if (cmd == "AllPassed")
    {
        mSequence = IDLE;
        mCanvas->SetFilter(Canvas::BLOCK_ALL);
        mCanvas->InitBoard();

        if (!mAutoPlay)
        {
            QMessageBox::information(this, trUtf8("Information"),
                                     trUtf8("All the players have passed.\n"
                                            "New deal will start."));
        }
        mNet.SendPacket(Protocol::ClientSyncAllPassed(mClient.GetUuid(), mClient.mTableId));
    }
    else if (cmd == "ShowDog")
    {
        mClient.mDog.SetCards(object["dog"].toString().toStdString());
        if (mAutoPlay)
        {
            mNet.SendPacket(Protocol::ClientSyncDog(mClient.GetUuid(), mClient.mTableId));
        }
        else
        {
            mSequence = SHOW_DOG;
            mCanvas->DrawCardsInPopup(mClient.mDog);
            mCanvas->SetFilter(Canvas::BOARD);
        }
    }
    else if (cmd == "BuildDiscard")
    {
        // We add the dog to the player's deck for discard building
        mMySavedDeck = mClient;  // locally save the legacy player's deck
        mClient.Append(mClient.mDog); // Add the dog

        mDiscard.Clear();
        mSequence = BUILD_DISCARD;
        mCanvas->SetFilter(Canvas::CARDS | Canvas::MENU);

        // Player's cards are shown
        ShowSouthCards();
    }
    else if (cmd == "StartDeal")
    {
        mClient.mBid.taker = Place(object["taker"].toString().toStdString());
        mClient.mBid.contract = Contract(object["contract"].toString().toStdString());
        mClient.mBid.slam = object["slam"].toBool();
        mClient.UpdateStatistics();
        mClient.mCurrentTrick.Clear();

        emit sigStartDeal();

        mSequence = IDLE;
        mCanvas->SetFilter(Canvas::BLOCK_ALL);
        mCanvas->ShowTaker(mClient.mBid.taker, mClient.mPlace);

        // We are ready, let's inform the server about that
        mNet.SendPacket(Protocol::ClientSyncStart(mClient.GetUuid(), mClient.mTableId));

    }
    else if (cmd == "AskForHandle")
    {
        AskForHandle();
    }
    else if (cmd == "ShowHandle")
    {
        Place place = Place(object["place"].toString().toStdString());
        mMyHandle.SetCards(object["handle"].toString().toStdString());

        Team team(Team::DEFENSE);
        if (place == mClient.mBid.taker)
        {
            team = Team::ATTACK;
        }
        mMyHandle.SetOwner(team);

        mCanvas->DrawCardsInPopup(mMyHandle);
        mSequence = SHOW_HANDLE;
        mCanvas->SetFilter(Canvas::BOARD);
    }
    else if (cmd == "ShowCard")
    {
        Place place = Place(object["place"].toString().toStdString());
        Card card = Card(object["card"].toString().toStdString());
        emit sigShowCard(place, card.GetName());

        mCanvas->DrawCard(card, place, mClient.mPlace);
        mClient.mCurrentTrick.Append(card);

        // We have seen the card, let's inform the server about that
        mNet.SendPacket(Protocol::ClientSyncShowCard(mClient.GetUuid(), mClient.mTableId));
    }
    else if (cmd == "PlayCard")
    {
        Place place = Place(object["place"].toString().toStdString());

        mCanvas->ShowSelection(place, mClient.mPlace);
        if (place == mClient.mPlace)
        {
            if (mAutoPlay)
            {
                mSequence = IDLE;
                mCanvas->SetFilter(Canvas::BLOCK_ALL);

                Card c = mClient.Play();
                mClient.Remove(c);
                mNet.SendPacket(Protocol::ClientCard(c.GetName(), mClient.GetUuid(), mClient.mTableId));
                ShowSouthCards();
            }
            else
            {
                mSequence = PLAY_TRICK;
                mCanvas->SetFilter(Canvas::CARDS);
            }
        }
    }
    else if (cmd == "EndOfTrick")
    {
        Place winner = Place(object["place"].toString().toStdString());
        emit sigWaitTrick(winner);

        mSequence = SYNC_END_OF_TRICK;
        mCanvas->SetFilter(Canvas::BOARD);

        // launch timer to clean cards, if needed
        if (mClientOptions.clickToClean == true)
        {
            QTimer::singleShot(mClientOptions.delayBeforeCleaning, this, SLOT(slotClickBoard()));
        }
    }
    else if (cmd == "EndOfDeal")
    {
        mPoints.pointsAttack = object["points"].toInt();
        mPoints.oudlers = object["oudlers"].toInt();
        mPoints.littleEndianOwner = static_cast<std::uint8_t>(object["little_bonus"].toInt());
        mPoints.handlePoints = object["handle_bonus"].toInt();
        mPoints.slamDone = object["slam_bonus"].toBool();

        QJsonValue val = object["result"];
        QJsonObject resultObj = val.toObject();
        QJsonDocument resultDoc(resultObj);
        mResult = resultDoc.toJson().toStdString();

        mCanvas->InitBoard();
        mCanvas->ResetCards();
        mCanvas->SetResult(mPoints, mClient.mBid);

        mSequence = SHOW_SCORE;
        mCanvas->SetFilter(Canvas::BOARD);

        emit sigAddScore();
    }
    else if (cmd == "EndOfGame")
    {
        // FIXME: deserialize the winner of the game
        /*
        QMessageBox::information(this, trUtf8("Game result"),
                                 trUtf8("The winner is ") + QString(winner.ToString().c_str()),
                                 QMessageBox::Ok);
        */
        mCanvas->SetFilter(Canvas::MENU);
        mCanvas->DisplayMainMenu(true);
    }
#endif


    else
    {
        TLogError("Command not supported.");
    }

    return event;
}


} // End of namespace helper
