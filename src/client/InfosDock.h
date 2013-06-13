/*=============================================================================
 * TarotClub - InfosDock.h
 *=============================================================================
 * Fenêtre "dockable" où sont stockées quelques informations du jeu en cours.
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

#ifndef INFOSDOCK_H
#define INFOSDOCK_H

// Includes Qt
#include <QDockWidget>
#include <QLabel>
#include <ui_InfosDockUI.h>

// Includes locales
#include "../defines.h"
#include "../Deck.h"

/*****************************************************************************/
class InfosDock : public QDockWidget
{
private:
    Ui::infosDock  ui;

public:
    InfosDock(QWidget *parent);

    void clear();
    void setContrat(Contract c);
    void setPreneur(QString preneur);
    void setDonne(int n);
    void printStats(Deck::Statistics &stats);
};

#endif // INFOSDOCK_H

//=============================================================================
// Fin du fichier InfosDock.h
//=============================================================================
