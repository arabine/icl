/*=============================================================================
 * TarotClub - Tapis.cpp
 *=============================================================================
 * CanvasView : visual game contents
 *=============================================================================
 * TarotClub ( http://www.tarotclub.fr ) - This file is part of TarotClub
 * Copyright (C) 2003-2999 - Anthony Rabine
 * anthony@tarotclub.fr
 *
 * This file must be used under the terms of the CeCILL.
 * This source file is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.  The terms
 * are also available at
 * http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
 *
 *=============================================================================
 */

#include "Tapis.h"
#include <QResizeEvent>
#include <QMouseEvent>
#include "textes.h"
#include "../Jeu.h"

/*****************************************************************************/
Tapis::Tapis( QWidget *parent )
    : QGraphicsView( parent )
{
   setScene(new QGraphicsScene(this));
   QGraphicsScene *s = scene();

   //==============================================================
   //       BOUTONS ENCHERES
   //==============================================================
   groupBoutons = new QGroupBox( tr("Enchères"), this );
   groupBoutons->setGeometry( 20, 250, 120, 230 );
   groupBoutons->hide();

   boutonPasse = new QPushButton(STR_PASSE, groupBoutons);
   boutonPasse->move(10,20);
   boutonPrise = new QPushButton(STR_PRISE, groupBoutons);
   boutonPrise->move(10,55);
   boutonGarde = new QPushButton(STR_GARDE, groupBoutons);
   boutonGarde->move(10,90);
   boutonGardeSans = new QPushButton(STR_GARDE_SANS, groupBoutons);
   boutonGardeSans->move(10,125);
   boutonGardeContre = new QPushButton(STR_GARDE_CONTRE, groupBoutons);
   boutonGardeContre->move(10,160);
   chelem = new QCheckBox( "Chelem", groupBoutons );
   chelem->move(10,195);
   //==============================================================

   boutonAccepterChien = new QPushButton(tr("Accepter le chien"), this);
   boutonAccepterChien->move(800, 462);
   boutonAccepterChien->setMinimumSize( boutonAccepterChien->sizeHint() );
   boutonAccepterChien->hide();

   //==============================================================
   //       ELEMENTS DU CANVAS
   //==============================================================
   btNord = new PlayerBox(440, 10, QBrush(Qt::yellow), s);
   btOuest = new PlayerBox(210, 113, QBrush(Qt::yellow), s);
   btSud = new PlayerBox(440, 462, QBrush(Qt::yellow), s);
   btEst = new PlayerBox(677, 113, QBrush(Qt::yellow), s);
   btNordOuest = new PlayerBox(677, 236, QBrush(Qt::yellow), s);

   btNord->show();
   btOuest->show();
   btSud->show();
   btEst->show();

   enchNord = new TextBox(440, 50, QBrush(Qt::red), s);
   enchOuest = new TextBox(210, 153, QBrush(Qt::red), s);
   enchSud = new TextBox(440, 422, QBrush(Qt::red), s);
   enchEst = new TextBox(677, 153, QBrush(Qt::red), s);
   enchNordOuest = new TextBox(555, 236, QBrush(Qt::red), s);

   connect( boutonPasse, SIGNAL(clicked()), this, SLOT(slotBoutton1()) );
   connect( boutonPrise, SIGNAL(clicked()), this, SLOT(slotBoutton2()) );
   connect( boutonGarde, SIGNAL(clicked()), this, SLOT(slotBoutton3()) );
   connect( boutonGardeSans, SIGNAL(clicked()), this, SLOT(slotBoutton4()) );
   connect( boutonGardeContre, SIGNAL(clicked()), this, SLOT(slotBoutton5()) );
   connect( boutonAccepterChien, SIGNAL(clicked()), this, SLOT(slotAccepteChien()) );

   // appel mouseEvent dès que la souris bouge, sans appuis sur les boutons
   viewport()->setMouseTracking(true);
   filter = AUCUN;
}
/*****************************************************************************/
void Tapis::setBackground(const QString &fichier)
{
   QPixmap pm;
   QGraphicsScene *canvas = scene();
   QBrush background;

   // On teste si on a un arriere plan
   if( pm.load( fichier ) == false ) {
      pm.fill( Qt::darkGreen );
   }
   background.setTexture( pm );
   canvas->setBackgroundBrush( background );
}
/*****************************************************************************/
GfxCard *Tapis::getGfxCard(int i)
{
   return cardsPics.at(i);
}
/*****************************************************************************/
Card *Tapis::getObjectCard(GfxCard *gc)
{
   Card *c = NULL;
   for( int i=0; i<cardsPics.size(); i++ ) {
      if( cardsPics.at(i) == gc ) {
         c = Jeu::getCard(i);
         break;
      }
   }
   return(c);
}
/*****************************************************************************/
void Tapis::setAccepterChienVisible(bool v)
{
   if (v == true) {
      boutonAccepterChien->show();
   } else {
      boutonAccepterChien->hide();
   }
}
/*****************************************************************************/
int Tapis::loadCards()
{
   int i,j,n;
   QString varImg;
   QString image;
   QGraphicsScene *canvas = scene();

   //----- 4 couleurs
   for( i=0; i<4; i++ ){
      if( i==0 ) {
         varImg = "pique";
      } else if( i==1 ) {
         varImg = "coeur";
      } else if( i==2 ) {
         varImg = "trefle";
      } else {
         varImg = "carreau";
      }

      // de l'as au roi (14 cartes)
      for( j=0; j<14; j++ ) {
         n = i*14+j;
         image = QString(":/cards/default/") + varImg + QString("-") + QString().sprintf("%02d.svg",j+1);

         QGraphicsSvgItem *item = new QGraphicsSvgItem(image);
         item->hide();
         cardsPics.insert(n, (GfxCard*)item);
         canvas->addItem(item);
      }
   }

   //----- 21 atouts
   for( i=56; i<77; i++) {
      image = QString(":/cards/default/atout-") + QString().sprintf("%02d.svg",i-55);
      QGraphicsSvgItem *item = new QGraphicsSvgItem(image);
      item->hide();
      cardsPics.insert(n, (GfxCard*)item);
      canvas->addItem(item);
   }

   //----- L'excuse
   image = QString(":/cards/default/excuse.svg");
   QGraphicsSvgItem *item = new QGraphicsSvgItem(image);
   item->hide();
   cardsPics.insert(n, (GfxCard*)item);
   canvas->addItem(item);

   return 0;
}
/*****************************************************************************/
void Tapis::resizeEvent( QResizeEvent *e )
{
   QSize s;
   s = e->size();
   setSceneRect( 0, 0, s.width(), s.height() ); // TODO ?? pas vraiment sûr, à tester en redimensionnant la fenêtre
}
/*****************************************************************************/
void Tapis::mousePressEvent( QMouseEvent *e )
{
   QList<QGraphicsItem *> list;

   emit sgnlViewportClicked();

   if( filter == AUCUN ) {
      return;
   }
   if( filter == JEU ) {
      list = scene()->items(e->pos());
      if ( !list.isEmpty() ) {
         QGraphicsItem *item = list.first();
         if( qgraphicsitem_cast<GfxCard *>(item) ) {
            GfxCard *c = (GfxCard *)item;
            emit sgnlClickCard( c );
         }
      }
   }
}
/*****************************************************************************/
/*
 * Emit a signal each time the cursor goes over a card
 */
void Tapis::mouseMoveEvent( QMouseEvent * e )
{
   QList<QGraphicsItem *> list;

   if( filter == AUCUN ) {
      return;
   }

   list = scene()->items(e->pos());

   if ( !list.isEmpty() ) {
      QGraphicsItem *item = list.first();
      // Si c'est une carte, retourne l'obet, sinon 0
      if( qgraphicsitem_cast<GfxCard *>(item) ) {
         GfxCard *c = (GfxCard *)item;
         emit sgnlMoveCursor( c );
      }
   } else {
      setCursorType( ARROW );
   }
}
/*****************************************************************************/
/**
 * n==0 : curseur normal avec une flèche
 * n==1 : curseur interdiction
 */
void Tapis::setCursorType( CursorType t )
{
   if( t == ARROW ) {
      QCursor cu( Qt::ArrowCursor );
      setCursor( cu );
   } else {
      QCursor cu( Qt::ForbiddenCursor );
      setCursor( cu );
   }
}
/*****************************************************************************/
void Tapis::colorisePreneur( Place preneur )
{
   if( preneur == SUD ) {
      btSud->setCouleur( QBrush(Qt::green) );
   } else if( preneur == EST ) {
      btEst->setCouleur( QBrush(Qt::green) );
   } else if( preneur == NORD ) {
      btNord->setCouleur( QBrush(Qt::green) );
   } else { // OUEST
      btOuest->setCouleur( QBrush(Qt::green) );
   }
}
/*****************************************************************************/
void Tapis::setText(Place p, const QString &txt)
{
   if( p == SUD ) {
      btSud->setText( txt );
   } else if( p == EST ) {
      btEst->setText( txt );
   } else if( p == NORD ) {
      btNord->setText( txt );
   } else { // OUEST
      btOuest->setText( txt );
   }
}
/*****************************************************************************/
void Tapis::setAvatar(Place p, const QString &file)
{
   if( p == SUD ) {
      btSud->setAvatar( file );
   } else if( p == EST ) {
      btEst->setAvatar( file );
   } else if( p == NORD ) {
      btNord->setAvatar( file );
   } else { // OUEST
      btOuest->setAvatar( file );
   }
}
/*****************************************************************************/
void Tapis::setNbPlayers(int n)
{
   btOuest->show();
   btSud->show();
   btEst->show();
   if( n == 4 ) {
      btNord->show();
   }
   if( n == 5 ) {
      btNordOuest->show();
   }
}
/*****************************************************************************/
/**
 * Change la place absolue (vue du serveur, qui est toujours SUD) en relatif
 * au joueur
 */
Place Tapis::retournePlace( Place origine, Place place_absolue )
{
   Place pl = SUD;

   if( origine == OUEST) {
      Place tab[] = {EST, NORD, OUEST, SUD};
      pl = tab[place_absolue];
   } else if( origine == NORD) {
      Place tab[] = {NORD, OUEST, SUD, EST};
      pl = tab[place_absolue];
   } else if( origine == EST) {
      Place tab[] = {OUEST, SUD, EST, NORD};
      pl = tab[place_absolue];
   } else {
      Place tab[] = {SUD, EST, NORD, OUEST};
      pl = tab[place_absolue];
   }

   return (pl);
}
/*****************************************************************************/
/**
 * Affiche les noms sur le tapis selon la position relative du joueur
 */
void Tapis::printNames( Identity *identities, Place place )
{
   Place serveur; // place relative du serveur

   serveur = retournePlace( place, SUD );

    btSud->setText( identities[retournePlace( serveur, SUD )].name );
    btEst->setText( identities[retournePlace( serveur,EST )].name  );
    btNord->setText( identities[retournePlace( serveur,NORD )].name  );
    btOuest->setText( identities[retournePlace( serveur,OUEST )].name  );

    btSud->setAvatar( identities[retournePlace( serveur,SUD )].avatar );
    btEst->setAvatar( identities[retournePlace( serveur,EST )].avatar );
    btNord->setAvatar( identities[retournePlace( serveur,NORD )].avatar );
    btOuest->setAvatar( identities[retournePlace( serveur,OUEST )].avatar );
}
/*****************************************************************************/
/**
 * Affiche une carte "c" à l'emplacement "p" de la table.
 * p = NORD, OUEST, SUD, EST
 */
void Tapis::afficheCarte( GfxCard *c, Place p )
{
   int x, y;

   if( p == NORD ) {
      x = btNord->x()+5;
      y = btNord->y()+40;
   } else if( p == OUEST ) {
      x = btOuest->x()+5;
      y = btOuest->y()+40;
   } else if( p == SUD ) {
      x = btSud->x()+5;
      y = btSud->y()-206;
   } else if( p == EST ) {
      x = btEst->x()+5;
      y = btEst->y()+40;
   } else {// Nord Ouest
      x = btNordOuest->x()+5;
      y = btNordOuest->y()+40;
   }

   c->setPos(x,y);
   c->show();
}
/*****************************************************************************/
void Tapis::afficheSelection( Place p )
{
   if( p == NORD ) {
      btNord->setPen( 1 );
      btOuest->setPen( 0 );
      btEst->setPen( 0 );
      btSud->setPen( 0 );
      btNordOuest->setPen( 0 );
   } else if( p == OUEST ) {
      btNord->setPen( 0 );
      btOuest->setPen( 1 );
      btEst->setPen( 0 );
      btSud->setPen( 0 );
      btNordOuest->setPen( 0 );
   } else if( p == SUD ) {
      btNord->setPen( 0 );
      btOuest->setPen( 0 );
      btEst->setPen( 0 );
      btSud->setPen( 1 );
      btNordOuest->setPen( 0 );
   } else if( p == EST ) {
      btNord->setPen( 0 );
      btOuest->setPen( 0 );
      btEst->setPen( 1 );
      btSud->setPen( 0 );
      btNordOuest->setPen( 0 );
   } else {// Nord ouest
      btNord->setPen( 0 );
      btOuest->setPen( 0 );
      btEst->setPen( 0 );
      btSud->setPen( 0 );
      btNordOuest->setPen( 1 );
   }
}
/*****************************************************************************/
void Tapis::slotAfficheEnchere( Place enchereur, Contrat cont )
{
   QString mot;

   if( cont == GARDE_CONTRE ) {
      mot = STR_GARDE_CONTRE;
   } else if( cont == GARDE_SANS ) {
      mot = STR_GARDE_SANS;
   } else if( cont == GARDE ) {
      mot = STR_GARDE;
   } else if( cont == PRISE ) {
      mot = STR_PRISE;
   } else {
      mot = STR_PASSE;
   }

   if( enchereur == NORD ) {
      enchNord->setText(mot);
      enchNord->show();
   } else if( enchereur == OUEST ) {
      enchOuest->setText(mot);
      enchOuest->show();
   } else if( enchereur == SUD ) {
      enchSud->setText(mot);
      enchSud->show();
   } else { // EST
      enchEst->setText(mot);
      enchEst->show();
   }
}
/*****************************************************************************/
void Tapis::cacheEncheres()
{
   enchNord->hide();
   enchOuest->hide();
   enchSud->hide();
   enchEst->hide();
}
/*****************************************************************************/
void Tapis::slotAfficheBoutons( Contrat contrat )
{
   boutonPrise->show();
   boutonGarde->show();
   boutonGardeSans->show();
   boutonGardeContre->show();

   if( contrat >= PRISE ) {
      boutonPrise->hide();
   }
   if( contrat >= GARDE ) {
      boutonGarde->hide();
   }
   if( contrat >= GARDE_SANS ) {
      boutonGardeSans->hide();
   }
   if( contrat >= GARDE_CONTRE ) {
      boutonGardeContre->hide();
   }
   groupBoutons->show();
}
/*****************************************************************************/
void Tapis::showAvatars( bool b, int nb_players )
{
   btSud->enableAvatar(b);
   btEst->enableAvatar(b);
   btOuest->enableAvatar(b);

   if( nb_players > 3 ) {
      btNord->enableAvatar(b);
   }
   if( nb_players > 4 ) {
      btNordOuest->enableAvatar(b);
   }
}
/*****************************************************************************/
void Tapis::cacheBoutons()
{
   groupBoutons->hide();
}
/*****************************************************************************/
void Tapis::razTapis()
{
   cacheEncheres();
   groupBoutons->hide();

   btSud->setCouleur( QBrush(Qt::yellow) );
   btEst->setCouleur( QBrush(Qt::yellow) );
   btNord->setCouleur( QBrush(Qt::yellow) );
   btOuest->setCouleur( QBrush(Qt::yellow) );

   btNord->setPen( 0 );
   btOuest->setPen( 0 );
   btEst->setPen( 0 );
   btSud->setPen( 0 );
   btNordOuest->setPen( 0 );
}
/*****************************************************************************/
void Tapis::slotBoutton1()
{
   emit sgnlContrat( PASSE );
}
/*****************************************************************************/
void Tapis::slotBoutton2()
{
   emit sgnlContrat( PRISE );
}
/*****************************************************************************/
void Tapis::slotBoutton3()
{
   emit sgnlContrat( GARDE );
}
/*****************************************************************************/
void Tapis::slotBoutton4()
{
   emit sgnlContrat( GARDE_SANS );
}
/*****************************************************************************/
void Tapis::slotBoutton5()
{
   emit sgnlContrat( GARDE_CONTRE );
}
/*****************************************************************************/
void Tapis::slotAccepteChien()
{
   emit sgnlAccepteChien();
}
/*****************************************************************************/
void Tapis::setFilter( Filter f )
{
   filter = f;
}

//=============================================================================
// End of file Tapis.cpp
//=============================================================================
