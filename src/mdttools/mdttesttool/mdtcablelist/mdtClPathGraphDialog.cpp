/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "mdtClPathGraphDialog.h"
#include "mdtClPathGraph.h"
#include <QMessageBox>

mdtClPathGraphDialog::mdtClPathGraphDialog(QWidget *parent, QSqlDatabase db)
 : QDialog(parent)
{
  setupUi(this);
  pvPathGraph = new mdtClPathGraph(db);
  gvPath->setRenderHint(QPainter::Antialiasing);
  gvPath->setCacheMode(QGraphicsView::CacheBackground);
  gvPath->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
  pvPathGraph->attachView(gvPath);
}

mdtClPathGraphDialog::~mdtClPathGraphDialog()
{
  delete pvPathGraph;
}

bool mdtClPathGraphDialog::loadLinkList()
{
  if(!pvPathGraph->loadLinkList()){
    displayError(pvPathGraph->lastError());
    return false;
  }

  return true;
}

bool mdtClPathGraphDialog::drawPath(const QVariant & fromConnectionId)
{
  if(!pvPathGraph->drawPath(fromConnectionId)){
    displayError(pvPathGraph->lastError());
    return false;
  }

  return true;
}

void mdtClPathGraphDialog::displayError(const mdtError& error)
{
  QMessageBox msgBox(this);
  msgBox.setText(error.text());
  msgBox.setInformativeText(error.informativeText());
  msgBox.setDetailedText(error.systemErrorString());
  msgBox.setIcon(QMessageBox::Critical);
  msgBox.exec();
}
