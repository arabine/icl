/*=============================================================================
 * TarotClub - Game.cpp
 *=============================================================================
 * Utility class to store various game state information
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

#include "Game.h"
#include "defines.h"
#include <QMap>

/*****************************************************************************/
Game::Game()
{
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime())); // seed init

    Initialize();
}
/*****************************************************************************/
void Game::Initialize()
{
    position = 0;
    trickCounter = 0;
    numberOfPlayers = 4;
    dealer = static_cast<Place>(qrand() % 4);
    contract = PASS;
    slamAnnounced = false;
    attackHandle.declared = false;
    defenseHandle.declared = false;
    gameMode = LOCAL_ONEDEAL;
}
/*****************************************************************************/
void Game::NewDeal()
{
    dealer = NextPlayer(dealer);
    contract = PASS;
    slamAnnounced = false;
    attackHandle.declared = false;
    defenseHandle.declared = false;
    position = 0;
    trickCounter = 0;
    currentPlayer = NextPlayer(dealer); // The first player on the dealer's right begins the bid
}
/*****************************************************************************/
void Game::Stop()
{
    sequence = STOP;
}
/*****************************************************************************/
void Game::StartDeal()
{
    sequence = PLAY_TRICK;
    // In case of Chelem, the first player to play is the taker.
    // Otherwise, it is the player on the right of the dealer
    if (slamAnnounced == true)
    {
        currentPlayer = taker;
    }
    else
    {
        currentPlayer = NextPlayer(dealer); // The first player on the dealer's right
    }
}
/*****************************************************************************/
bool Game::IsDealFinished()
{
    if (trickCounter >= GetNumberOfCards())
    {
        return true;
    }
    else
    {
        return false;
    }
}
/*****************************************************************************/
bool Game::Next()
{
    bool endOfTrick = false;
    position++;
    if (!(position > numberOfPlayers))
    {
        position = 0;
        trickCounter++;
        endOfTrick = true;
    }
    else
    {
        currentPlayer = NextPlayer(currentPlayer);
        endOfTrick = false;
    }
    return endOfTrick;
}
/*****************************************************************************/
int Game::GetRemainingTurns()
{
    return (GetNumberOfCards() - trickCounter);
}
/*****************************************************************************/
int Game::GetNumberOfCards()
{
    if (numberOfPlayers == 3)
    {
        return 24;
    }
    if (numberOfPlayers == 5)
    {
        return 15;
    }
    else
    {
        // 4 players
        return 18;
    }
}
/*****************************************************************************/
int Game::GetNumberOfDogCards()
{
    return (78-(GetNumberOfCards()*numberOfPlayers));
}
/*****************************************************************************/
Place Game::NextPlayer(Place j)
{
    Place p;

    if (j == SOUTH)
    {
        p = EAST;
    }
    else if (j == EAST)
    {
        p = NORTH;
    }
    else if (j == NORTH)
    {
        p = WEST;
    }
    else
    {
        p = SOUTH;
    }
    return(p);
}

//=============================================================================
// End of file Game.cpp
//=============================================================================
