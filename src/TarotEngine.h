/*=============================================================================
 * TarotClub - TarotEngine.h
 *=============================================================================
 * Moteur de jeu principal + serveur de jeu réseau
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
#ifndef TAROT_ENGINE_H
#define TAROT_ENGINE_H

#include "Card.h"
#include "Deck.h"
#include "Deal.h"
#include "Player.h"
#include "Common.h"
#include "ServerConfig.h"
#include "Observer.h"
#ifndef DESKTOP_PROJECT
#include "CouchDb.h"
#endif
#include "DummyRemoteDb.h"

/*****************************************************************************/
/**
 * @brief Tarot state engine
 *
 * This class stores information about one deal. It also manages the game states
 * and transitions.
 *
 */
class TarotEngine
{
public:
    enum Sequence
    {
        STOPPED,
        WAIT_FOR_PLAYERS,
        WAIT_FOR_READY,
        WAIT_FOR_CARDS,
        WAIT_FOR_BID,
        WAIT_FOR_SHOW_BID,
        WAIT_FOR_ALL_PASSED,
        WAIT_FOR_SHOW_DOG,
        WAIT_FOR_DISCARD,
        WAIT_FOR_START_DEAL,
        WAIT_FOR_HANDLE,
        WAIT_FOR_SHOW_HANDLE,
        WAIT_FOR_PLAYED_CARD,
        WAIT_FOR_SHOW_CARD,
        WAIT_FOR_END_OF_TRICK,
        WAIT_FOR_END_OF_DEAL
    };

    TarotEngine();
    ~TarotEngine();

    // Helpers
    void Initialize();
    void StopGame();
    void CreateTable(std::uint8_t nbPlayers);
    void NewGame();
    Tarot::Distribution NewDeal(const Tarot::Distribution &shuffle);
    Place StartDeal();
    void EndOfDeal(const Identity players[5U], const std::string &tableName);

    Place AddPlayer(std::uint32_t uuid);
    void RemovePlayer(std::uint32_t uuid);
    void BidSequence();
    void DiscardSequence();
    void GameSequence();
    bool Sync(Sequence sequence, std::uint32_t uuid);

    // Getters
    Player *GetPlayer(Place p);
    Player *GetPlayer(std::uint32_t uuid);
    Place GetPlayerPlace(std::uint32_t uuid);
    Place GetCurrentPlayer()
    {
        return mCurrentPlayer;
    }
    Sequence GetSequence()
    {
        return mSequence;
    }
    Points GetCurrentGamePoints();

    std::uint8_t    GetNbPlayers()
    {
        return mNbPlayers;
    }
    Tarot::Bid      GetBid()
    {
        return mBid;
    }

    Deck GetDog()
    {
        return mDeal.GetDog();
    }

    bool IsLastTrick()
    {
        return Tarot::IsDealFinished(mTrickCounter, mNbPlayers);
    }

    // Setters
    bool ValidatePlayer(std::uint32_t uuid);
    bool SetDiscard(const Deck &discard);
    bool SetHandle(const Deck &handle, Place p);
    bool SetCard(const Card &c, Place p);
    Contract SetBid(Contract c, bool slam, Place p);

private:
    Player  mPlayers[5];     // [3..5] deck of players with their UUID, index = Place
    Deck    currentTrick;   // store the current trick cards played

    Deal    mDeal;
    Points  mCurrentPoints;

    // Game state variables
    std::uint8_t    mNbPlayers;
    Sequence        mSequence;
    Tarot::Bid      mBid;
    std::uint8_t    mPosition;          // Current position, [1..numberOfPlayers]
    Place           mDealer;            // who has dealt the cards
    std::uint8_t    mTrickCounter;       // number of tricks played [1..18] for 4 players
    Place           mCurrentPlayer;
    unsigned        mSeed;
    bool            mHandleAsked[5U];

    bool AckFromAllPlayers();
    void ResetAck();
    void CreateDeal(Tarot::Distribution &shuffle);
    bool IsEndOfTrick();
};

#endif // TAROT_ENGINE_H

//=============================================================================
// End of file TarotEngine.h
//=============================================================================
