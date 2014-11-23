/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtSqlFieldSelectionDialog.h"
#include <QCheckBox>
#include <QVBoxLayout>
#include <QList>

#include <QDebug>

namespace mdtSqlFieldSelectionDialogPrivate
{
  selectionItem::selectionItem(QWidget *parent)
  {
    cb = new QCheckBox(parent);
  }
};

mdtSqlFieldSelectionDialog::mdtSqlFieldSelectionDialog(QWidget* parent, QSqlDatabase db)
 : QDialog(parent),
   pvDatabase(db)
{
  setupUi(this);
  Q_ASSERT(wItems->layout() != 0);
  Q_ASSERT(dynamic_cast<QVBoxLayout*>(wItems->layout()) != 0);
}

void mdtSqlFieldSelectionDialog::setMessage ( const QString& msg )
{
  lbMessage->setText(msg);
}

void mdtSqlFieldSelectionDialog::addField ( const QString& fieldName, const QString& labelName, bool setSelected )
{
  mdtSqlFieldSelectionDialogPrivate::selectionItem item;
  item.fieldName = fieldName;
  item.cb->setText(labelName);
  item.cb->setChecked(setSelected);
  pvItems.insert(labelName, item);
  displayItems();
}

void mdtSqlFieldSelectionDialog::displayItems()
{
  QMap<QString, mdtSqlFieldSelectionDialogPrivate::selectionItem>::const_iterator it;

  removeItems();
  for(it = pvItems.constBegin(); it != pvItems.constEnd(); ++it){
    wItems->layout()->addWidget(it.value().cb);
  }
}

void mdtSqlFieldSelectionDialog::removeItems()
{
  QList<QObject*> children = wItems->children();
  QWidget *w;
  int i;

  for(i = 0; i < children.size(); ++i){
    w = qobject_cast<QCheckBox*>(children.at(i));
    if(w != 0){
      wItems->layout()->removeWidget(w);
    }
  }
}
