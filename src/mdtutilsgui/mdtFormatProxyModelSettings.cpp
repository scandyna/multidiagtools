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
#include "mdtFormatProxyModelSettings.h"
#include <QSqlQueryModel>
#include <QSqlRecord>

/*
 * mdtFormatProxyModelBackground implementation
 */

void mdtFormatProxyModelBackground::addBackground(const QVariant &keyData, const QBrush &bg)
{
  pvItems.emplace_back(keyData, bg);
}

void mdtFormatProxyModelBackground::clear()
{
  pvItems.clear();
  keyDataColumn = 0;
}

QVariant mdtFormatProxyModelBackground::background(const QVariant & keyData) const
{
  for(const auto & item : pvItems){
    if(item.keyData == keyData){
      return item.background;
    }
  }
  return QVariant();
}

/*
 * mdtFormatProxyModelSettings implementation
 */

void mdtFormatProxyModelSettings::addTextAlignment(int column, Qt::Alignment alignment)
{
  pvTextAlignments.emplace_back(column, alignment);
}

void mdtFormatProxyModelSettings::addTextAlignment(QSqlQueryModel *model, const QString & fieldName, Qt::Alignment alignment)
{
  Q_ASSERT(model != nullptr);

  int col = model->record().indexOf(fieldName);

  if(col < 0){
    return;
  }
  addTextAlignment(col, alignment);
}

QVariant mdtFormatProxyModelSettings::textAlignment(int column) const
{
  for(const auto & ta : pvTextAlignments){
    if(ta.column == column){
      return static_cast<int>(ta.alignment);
    }
  }
  return QVariant();
}

void mdtFormatProxyModelSettings::setBackgroundKeyDataColumn(QSqlQueryModel *model, const QString &fieldName)
{
  Q_ASSERT(model != nullptr);

  int col = model->record().indexOf(fieldName);

  if(col < 0){
    return;
  }
  setBackgroundKeyDataColumn(col);
}

void mdtFormatProxyModelSettings::clear()
{
  pvBackground.clear();
  pvTextAlignments.clear();
}
