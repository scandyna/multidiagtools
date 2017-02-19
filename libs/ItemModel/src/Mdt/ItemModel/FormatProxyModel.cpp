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
  mForegroundBrushMap.setPriority(priority);
}

void FormatProxyModel::setTextAlignmentForIndex(int row, int column, Qt::Alignment alignment)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(column >= 0);

  mTextAlignmentMap.setFormatForIndex(row, column, alignment);
  signalFormatChangedForIndex(row, column, Qt::TextAlignmentRole);
}

void FormatProxyModel::clearTextAlignmentForIndex(int row, int column)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(column >= 0);

  mTextAlignmentMap.clearFormatForIndex(row, column);
  signalFormatChangedForIndex(row, column, Qt::TextAlignmentRole);
}

void FormatProxyModel::setTextAlignmentForRow(int row, Qt::Alignment alignment)
{
  Q_ASSERT(row >= 0);

  mTextAlignmentMap.setFormatForRow(row, alignment);
  signalFormatChangedForRow(row, Qt::TextAlignmentRole);
}

void FormatProxyModel::clearTextAlignmentForRow(int row)
{
  Q_ASSERT(row >= 0);

  mTextAlignmentMap.clearFormatForRow(row);
  signalFormatChangedForRow(row, Qt::TextAlignmentRole);
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

void FormatProxyModel::setTextFontForIndex(int row, int column, const QFont& font)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(column >= 0);

  mTextFontMap.setFormatForIndex(row, column, font);
  signalFormatChangedForIndex(row, column, Qt::FontRole);
}

void FormatProxyModel::clearTextFontForIndex(int row, int column)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(column >= 0);

  mTextFontMap.clearFormatForIndex(row, column);
  signalFormatChangedForIndex(row, column, Qt::FontRole);
}

void FormatProxyModel::setTextFontForRow(int row, const QFont& font)
{
  Q_ASSERT(row >= 0);

  mTextFontMap.setFormatForRow(row, font);
  signalFormatChangedForRow(row, Qt::FontRole);
}

void FormatProxyModel::clearTextFontForRow(int row)
{
  Q_ASSERT(row >= 0);

  mTextFontMap.clearFormatForRow(row);
  signalFormatChangedForRow(row, Qt::FontRole);
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

void FormatProxyModel::setTextColorForIndex(int row, int column, const QColor& color)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(column >= 0);

  mForegroundBrushMap.setFormatForIndex(row, column, QBrush(color));
  signalFormatChangedForIndex(row, column, Qt::ForegroundRole);
}

void FormatProxyModel::clearTextColorForIndex(int row, int column)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(column >= 0);

  mForegroundBrushMap.clearFormatForIndex(row, column);
  signalFormatChangedForIndex(row, column, Qt::ForegroundRole);
}

void FormatProxyModel::setTextColorForRow(int row, const QColor& color)
{
  Q_ASSERT(row >= 0);

  mForegroundBrushMap.setFormatForRow(row, QBrush(color));
  signalFormatChangedForRow(row, Qt::ForegroundRole);
}

void FormatProxyModel::clearTextColorForRow(int row)
{
  Q_ASSERT(row >= 0);

  mForegroundBrushMap.clearFormatForRow(row);
  signalFormatChangedForRow(row, Qt::ForegroundRole);
}

void FormatProxyModel::setTextColorForColumn(int column, const QColor& color)
{
  Q_ASSERT(column >= 0);

  mForegroundBrushMap.setFormatForColumn(column, QBrush(color));
  signalFormatChangedForColumn(column, Qt::ForegroundRole);
}

void FormatProxyModel::clearTextColorForColumn(int column)
{
  Q_ASSERT(column >= 0);

  mForegroundBrushMap.clearFormatForColumn(column);
  signalFormatChangedForColumn(column, Qt::ForegroundRole);
}

QVariant FormatProxyModel::foregroundBrush(int row, int column) const
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());
  Q_ASSERT(column >= 0);
  Q_ASSERT(column < columnCount());

  return mForegroundBrushMap.formatForIndex(row, column);
}

void FormatProxyModel::setBackgroundBrushForIndex(int row, int column, const QBrush & brush)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(column >= 0);

  mBackgroundBrushMap.setFormatForIndex(row, column, brush);
  signalFormatChangedForIndex(row, column, Qt::BackgroundRole);
}

void FormatProxyModel::setBackgroundColorForIndex(int row, int column, const QColor& color, Qt::BrushStyle style)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(column >= 0);

  setBackgroundBrushForIndex(row, column, QBrush(color, style));
}

void FormatProxyModel::clearBackgroundBrushForIndex(int row, int column)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(column >= 0);

  mBackgroundBrushMap.clearFormatForIndex(row, column);
  signalFormatChangedForIndex(row, column, Qt::BackgroundRole);
}

void FormatProxyModel::setBackgroundBrushForRow(int row, const QBrush& brush)
{
  Q_ASSERT(row >= 0);

  mBackgroundBrushMap.setFormatForRow(row, brush);
  signalFormatChangedForRow(row, Qt::BackgroundRole);
}

void FormatProxyModel::setBackgroundColorForRow(int row, const QColor& color, Qt::BrushStyle style)
{
  Q_ASSERT(row >= 0);

  setBackgroundBrushForRow(row, QBrush(color, style));
}

void FormatProxyModel::clearBackgroundBrushForRow(int row)
{
  Q_ASSERT(row >= 0);

  mBackgroundBrushMap.clearFormatForRow(row);
  signalFormatChangedForRow(row, Qt::BackgroundRole);
}

void FormatProxyModel::setBackgroundBrushForColumn(int column, const QBrush& brush)
{
  Q_ASSERT(column >= 0);

  mBackgroundBrushMap.setFormatForColumn(column, brush);
  signalFormatChangedForColumn(column, Qt::BackgroundRole);
}

void FormatProxyModel::setBackgroundColorForColumn(int column, const QColor& color, Qt::BrushStyle style)
{
  Q_ASSERT(column >= 0);

  setBackgroundBrushForColumn(column, QBrush(color, style));
}

void FormatProxyModel::clearBackgroundBrushForColumn(int column)
{
  Q_ASSERT(column >= 0);

  mBackgroundBrushMap.clearFormatForColumn(column);
  signalFormatChangedForColumn(column, Qt::BackgroundRole);
}

QVariant FormatProxyModel::backgroundBrush(int row, int column) const
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < rowCount());
  Q_ASSERT(column >= 0);
  Q_ASSERT(column < columnCount());

  return mBackgroundBrushMap.formatForIndex(row, column);
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
    case Qt::ForegroundRole:
      return foregroundBrush(index.row(), index.column());
    case Qt::BackgroundRole:
      return backgroundBrush(index.row(), index.column());
    default:
      ;
  }
  return QIdentityProxyModel::data(index, role);
}

void FormatProxyModel::signalFormatChangedForIndex(int row, int column, int role)
{
  Q_ASSERT(row >= 0);
  Q_ASSERT(column >= 0);

  if(sourceModel() == nullptr){
    return;
  }
  Q_ASSERT(row < rowCount());
  Q_ASSERT(column < columnCount());

  const auto idx = index(row, column);
  emit dataChanged(idx, idx, {role});
}

void FormatProxyModel::signalFormatChangedForRow(int row, int role)
{
  Q_ASSERT(row >= 0);

  if(sourceModel() == nullptr){
    return;
  }
  Q_ASSERT(row < rowCount());

  const auto topLeft = index(row, 0);
  const auto bottomRight = index(row, columnCount()-1);
  emit dataChanged(topLeft, bottomRight, {role});
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
