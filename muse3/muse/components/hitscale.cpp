//=========================================================
//  MusE
//  Linux Music Editor
//    $Id: hitscale.cpp,v 1.3.2.1 2007/01/27 14:52:43 spamatica Exp $
//  (C) Copyright 1999 Werner Schweer (ws@seh.de)
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
//=========================================================

#include "hitscale.h"
#include "midieditor.h"
#include "gconfig.h"

#include <QMouseEvent>
#include <QPainter>

#include "song.h"

namespace MusEGui {

//---------------------------------------------------------
//   HitScale
//---------------------------------------------------------

HitScale::HitScale(int* r, QWidget* parent, int xs)
   : View(parent, xs, 1)
      {
      raster = r;
      pos[0] = MusEGlobal::song->cpos();
      pos[1] = MusEGlobal::song->lpos();
      pos[2] = MusEGlobal::song->rpos();
      button = Qt::NoButton;
      setMouseTracking(true);
      connect(MusEGlobal::song, SIGNAL(posChanged(int, unsigned, bool)), this, SLOT(setPos(int, unsigned, bool)));
      setFixedHeight(18);
      }

//---------------------------------------------------------
//   setPos
//---------------------------------------------------------

void HitScale::setPos(int idx, unsigned val, bool)
      {
      if (val == pos[idx])
            return;
      unsigned int opos = mapx(pos[idx]); // in order preventing comparison of sigend & unsigned int ??is this OK?
      pos[idx] = val;
      if (!isVisible())
            return;
      val = mapx(val);
      int x = -9;
      int w = 18;
      if (opos > val) {			//here would be the comparison signed/unsigned
            w += opos - val;
            x += val;
            }
      else {
            w += val - opos;
            x += opos;
            }
      paint(QRect(x, 0, w, height()));
      }

void HitScale::viewMousePressEvent(QMouseEvent* event)
      {
      button = event->button();
      viewMouseMoveEvent(event);
      }

void HitScale::viewMouseReleaseEvent(QMouseEvent*)
      {
      button = Qt::NoButton;
      }

void HitScale::viewMouseMoveEvent(QMouseEvent* event)
      {
      int x = event->x();
      if(x < 0)
        x = 0;
      x = MusEGlobal::sigmap.raster(x, *raster);
      emit timeChanged(x);
      MusECore::Song::POSTYPE posType;

      switch (button) {
      case Qt::LeftButton:
          if ((MusEGlobal::config.rangeMarkersSet == MusEGlobal::CONF_SET_MARKERS_CTRL_LEFT_CTRL_RIGHT) && (event->modifiers() & Qt::ControlModifier))
              posType = MusECore::Song::LPOS;
          else
              posType = MusECore::Song::CPOS;
          break;
      case Qt::MidButton:
          posType = MusECore::Song::LPOS;
          break;
      case Qt::RightButton:
          if ((MusEGlobal::config.rangeMarkersSet == MusEGlobal::CONF_SET_MARKERS_CTRL_LEFT_CTRL_RIGHT) && (event->modifiers() & Qt::ControlModifier))
              posType = MusECore::Song::RPOS;
          else if ((MusEGlobal::config.rangeMarkersSet == MusEGlobal::CONF_SET_MARKERS_CTRL_RIGHT_RIGHT) && (event->modifiers() & Qt::ControlModifier))
              posType = MusECore::Song::LPOS;
          else
              posType = MusECore::Song::RPOS;
          break;
      default:
          return;
      }
      MusECore::Pos p(x, true);
      MusEGlobal::song->setPos(posType, p);
      }

//---------------------------------------------------------
//   leaveEvent
//---------------------------------------------------------

void HitScale::leaveEvent(QEvent*)
      {
      emit timeChanged(-1);
      }

//---------------------------------------------------------
//   draw
//---------------------------------------------------------

void HitScale::pdraw(QPainter& p, const QRect& r, const QRegion&)
      {
      int x = r.x();
      int w = r.width();

//      x -= 10;
//      w += 20;

      if (x < 0)
            x = 0;

      //---------------------------------------------------
      //    draw location marker
      //---------------------------------------------------

      p.setPen(Qt::blue);
      int xp = mapx(pos[1]);
      if (xp >= x && xp < x+w)
            p.drawLine(xp, 0, xp, height());
      xp = mapx(pos[2]);
      if (xp >= x && xp < x+w)
            p.drawLine(xp, 0, xp, height());
      // Draw the red main position cursor last, on top of the others.
      p.setPen(Qt::red);
      xp = mapx(pos[0]);
      if (xp >= x && xp < x+w)
            p.drawLine(xp, 0, xp, height());
      }

}

