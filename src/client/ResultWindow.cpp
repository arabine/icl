/*=============================================================================
 * TarotClub - ResultWindow.cpp
 *=============================================================================
 * ResultWindow : affiche le résultat d'une donne
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

#include "ResultWindow.h"
#include "textes.h"

/*****************************************************************************/
ResultWindow::ResultWindow(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
}
/*****************************************************************************/
void ResultWindow::SetResult(ScoreInfo &score, GameState &info)
{
    QString result_str;

    // On affiche les caractéristiques de la partie
    if (info.taker == SUD)
    {
        ui.lblTaker->setText(STR_SUD);
    }
    else if (info.taker == EST)
    {
        ui.lblTaker->setText(STR_EST);
    }
    else if (info.taker == NORD)
    {
        ui.lblTaker->setText(STR_NORD);
    }
    else
    {
        ui.lblTaker->setText(STR_OUEST);
    }

    if (info.contract == PRISE)
    {
        ui.lblContrat->setText(STR_PRISE);
    }
    else if (info.contract == GARDE)
    {
        ui.lblContrat->setText(STR_GARDE);
    }
    else if (info.contract == GARDE_SANS)
    {
        ui.lblContrat->setText(STR_GARDE_SANS);
    }
    else
    {
        ui.lblContrat->setText(STR_GARDE_CONTRE);
    }

    ui.lblNbBouts->setNum(score.bouts);
    ui.lblPoints->setNum((int)score.attaque);

    if ((int)(score.attaque) >= score.pointsAFaire)
    {
        result_str = QString("<font color=\"#0D851B\">") + STR_WIN;
    }
    else
    {
        result_str = QString("<font color=\"red\">") + STR_LOSE;
    }
    ui.lblResultat->setText(result_str + QString().setNum(score.difference) +
                            STR_POINTS + QString("</font>"));

    ui.lblGains->setNum(score.difference);
    ui.lblPetit->setNum(score.points_petit_au_bout);
    ui.lblMultiple->setNum(score.multiplicateur);
    ui.lblPoignee->setNum(score.points_poignee);
    ui.lblChelem->setNum(score.points_chelem);
    ui.lblTotalDefense->setText(QString().setNum(score.points_defense) + STR_POINTS);
    ui.lblTotalAttaque->setText(QString().setNum(score.points_defense * (-3)) + STR_POINTS);
}


//=============================================================================
// Fin du fichier ResultWindow.cpp
//=============================================================================
