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
#include "mdtFormatProxyModel.h"
#include <QBrush>
#include <QColor>

//#include <QDebug>

mdtFormatProxyModel::mdtFormatProxyModel(QObject *parent)
 : QIdentityProxyModel(parent)
{
}

void mdtFormatProxyModel::setSettings(const mdtFormatProxyModelSettings & settings)
{
  pvSettings = settings;
}

void mdtFormatProxyModel::clearSettings()
{
  pvSettings.clear();
}

QVariant mdtFormatProxyModel::data(const QModelIndex & index, int role) const
{
  QVariant fmt;

  switch(role){
    case Qt::TextAlignmentRole:
      fmt = pvSettings.textAlignment(index.column());
      break;
    case Qt::BackgroundRole:
      fmt = backgroundRoleData(index.row());
      break;
    default:
      // Nothing (we must have a return after switch)
      ;
  }
  if(!fmt.isNull()){
    return fmt;
  }

  return QIdentityProxyModel::data(index, role);
}

QVariant mdtFormatProxyModel::backgroundRoleData(int row) const
{
  QModelIndex keyDataIndex = index(row, pvSettings.backgroundKeyDataColumn());
  QVariant keyData = data(keyDataIndex);
  return pvSettings.background(keyData);
}
