/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#include "FormatProxyModel.h"
#include <QVector>

namespace Mdt{ namespace ItemModel{

FormatProxyModel::FormatProxyModel(QObject* parent)
 : QIdentityProxyModel(parent)
{
}

void FormatProxyModel::setPriority(const std::array<FormatMapPriority, int(3)> & priority)
{
  mTextAlignmentMap.setPriority(priority);
  mTextFontMap.setPriority(priority);
  mForegroundBrushColumnMap.setPriority(priority);
}

void FormatProxyModel::setTextAlignmentForColumn(int column, Qt::Alignment alignment)
{
  Q_ASSERT(column >= 0);

  mTextAlignmentMap.setFormatForColumn(column, alignment);
  signalFormatChangedForColumn(column, Qt::TextAlignmentRole);
}

void FormatProxyModel::clearTextAlignmentForColumn(int column)
{
  Q_ASSERT(column >= 0);
  Q_ASSERT(column < columnCount());

  mTextAlignmentMap.clearFormatForColumn(column);
  signalFormatChangedForColumn(column, Qt::TextAlignmentRole);
}


QVariant FormatProxyModel::textAlignment(int row, int column) const
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());
  Q_ASSERT(column >= 0);
  Q_ASSERT(column < columnCount());

  return mTextAlignmentMap.formatForIndex(row, column);
}

void FormatProxyModel::setTextFontForColumn(int column, const QFont& font)
{
  Q_ASSERT(column >= 0);

  mTextFontMap.setFormatForColumn(column, font);
  signalFormatChangedForColumn(column, Qt::FontRole);
}

void FormatProxyModel::clearTextFontForColumn(int column)
{
  Q_ASSERT(column >= 0);
  Q_ASSERT(column < columnCount());

  mTextFontMap.clearFormatForColumn(column);
  signalFormatChangedForColumn(column, Qt::FontRole);
}

QVariant FormatProxyModel::textFont(int row, int column) const
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());
  Q_ASSERT(column >= 0);
  Q_ASSERT(column < columnCount());

  return mTextFontMap.formatForIndex(row, column);
}

void FormatProxyModel::setTextColorForColumn(int column, const QColor& color)
{
  Q_ASSERT(column >= 0);
}

void FormatProxyModel::clearTextColorForColumn(int column)
{
  Q_ASSERT(column >= 0);
}

QVariant FormatProxyModel::foregroundBrush(int row, int column) const
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());
  Q_ASSERT(column >= 0);
  Q_ASSERT(column < columnCount());

  return QVariant();
}

QVariant FormatProxyModel::data(const QModelIndex & index, int role) const
{
  if(!index.isValid()){
    return QVariant();
  }
  if( (index.row() >= rowCount()) || (index.column() >= columnCount()) ){
    return QVariant();
  }
  switch(role){
    case Qt::TextAlignmentRole:
      return textAlignment(index.row(), index.column());
    case Qt::FontRole:
      return textFont(index.row(), index.column());
    default:
      ;
  }
  return QIdentityProxyModel::data(index, role);
}

void FormatProxyModel::signalFormatChangedForColumn(int column, int role)
{
  Q_ASSERT(column >= 0);

  if(sourceModel() == nullptr){
    return;
  }
  Q_ASSERT(column < columnCount());

  const int n = rowCount();
  for(int row = 0; row < n; ++row){
    const auto idx = index(row, column);
    emit dataChanged(idx, idx, {role});
  }
}

}} // namespace Mdt{ namespace ItemModel{
