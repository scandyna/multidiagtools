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
#include <QMap>

#include <QDebug>

namespace mdtSqlFieldSelectionDialogPrivate
{
  selectionItem::selectionItem(QWidget *parent)
  {
    cb = new QCheckBox(parent);
  }
};

mdtSqlFieldSelectionDialog::mdtSqlFieldSelectionDialog(QWidget* parent)
 : QDialog(parent)
{
  setupUi(this);
  Q_ASSERT(wItems->layout() != 0);
  Q_ASSERT(dynamic_cast<QVBoxLayout*>(wItems->layout()) != 0);
}

void mdtSqlFieldSelectionDialog::setMessage ( const QString& msg )
{
  lbMessage->setText(msg);
}

void mdtSqlFieldSelectionDialog::addField ( const QString& fieldName, const QString& labelName, bool setSelected)
{
  mdtSqlFieldSelectionDialogPrivate::selectionItem item;
  item.fieldName = fieldName;
  item.cb->setText(labelName);
  item.cb->setChecked(setSelected);
  pvItemList.append(item);
  wItems->layout()->addWidget(item.cb);
}

void mdtSqlFieldSelectionDialog::sort ( Qt::SortOrder sortOrder )
{
  QMap<QString, mdtSqlFieldSelectionDialogPrivate::selectionItem> sortMap;
  QMap<QString, mdtSqlFieldSelectionDialogPrivate::selectionItem>::const_iterator it;
  int i;

  /*
   * By inserting items in QMap, they are sorted by keys.
   * So we simply build a map, and get values back.
   */
  // Build the map
  for(i = 0; i < pvItemList.size(); ++i){
    mdtSqlFieldSelectionDialogPrivate::selectionItem item = pvItemList.at(i);
    sortMap.insert(item.cb->text(), item);
  }
  // Update internal container
  if(sortOrder == Qt::AscendingOrder){
    pvItemList = sortMap.values();
  }else{
    QList<mdtSqlFieldSelectionDialogPrivate::selectionItem> lst;
    lst = sortMap.values();
    pvItemList.clear();
    for(i = lst.size() - 1; i >= 0; --i){
      pvItemList.append(lst.at(i));
    }
  }
  // Update item widgets
  removeItemWidgets();
  for(i = 0; i < pvItemList.size(); ++i){
    wItems->layout()->addWidget(pvItemList.at(i).cb);
  }
}

QStringList mdtSqlFieldSelectionDialog::getFieldNames() const
{
  QStringList fieldNames;
  int i;

  for(i = 0; i < pvItemList.size(); ++i){
    fieldNames.append(pvItemList.at(i).fieldName);
  }

  return fieldNames;
}

QStringList mdtSqlFieldSelectionDialog::getSelectedFieldNames() const
{
  QStringList fieldNames;
  int i;

  for(i = 0; i < pvItemList.size(); ++i){
    if(pvItemList.at(i).cb->isChecked()){
      fieldNames.append(pvItemList.at(i).fieldName);
    }
  }

  return fieldNames;
}

void mdtSqlFieldSelectionDialog::removeItemWidgets()
{
  int i;

  for(i = 0; i < pvItemList.size(); ++i){
    wItems->layout()->removeWidget(pvItemList.at(i).cb);
  }
}
