/****************************************************************************
 **
 ** Copyright (C) 2011-2016 Philippe Steinmann.
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
#include "ComparisonEval.h"
#include "Mdt/FilterExpression/LikeExpressionRegexTransform.h"
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QAbstractItemModel>
#include <QModelIndex>

// #include <QDebug>

namespace Mdt{ namespace ItemModel{ namespace Expression{

QVariant getVariantValue(const FilterColumnData & col, const FilterEvalData & data)
{
  const auto * model = data.model();
  Q_ASSERT(model != nullptr);
  Q_ASSERT(col.columnIndex() < model->columnCount());

  const auto index = model->index(data.row(), col.columnIndex());
  Q_ASSERT(index.isValid());

  return model->data(index);
}

bool CompareLikeTo::isLike(const FilterColumnData & col, const QString & like, const FilterEvalData & data) const
{
  using Mdt::FilterExpression::LikeExpressionRegexTransform;

  QRegularExpression regex;

  if(data.caseSensitivity() == Qt::CaseInsensitive){
    regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
  }
  regex.setPattern( LikeExpressionRegexTransform::getRegexPattern(like) );
  Q_ASSERT(regex.isValid());

  return regex.match( getStringValue(col, data) ).hasMatch();
}

bool CompareEqualTo::isEqual(const FilterColumnData& col, const QString & value, const FilterEvalData & data)
{
  return (QString::compare( getStringValue(col, data), value, data.caseSensitivity() ) == 0);
}

bool CompareEqualTo::isEqual(const FilterColumnData & col, int value, const FilterEvalData & data)
{
  return ( getVariantValue(col, data) == value );
}

bool CompareLessThan::isLessThan(const FilterColumnData& col, const QString& value, const FilterEvalData& data) const
{
  return (QString::compare( getStringValue(col, data), value, data.caseSensitivity() ) < 0);
}

bool CompareLessThan::isLessThan(const FilterColumnData& col, int value, const FilterEvalData& data) const
{
  return ( getVariantValue(col, data) < value );
}

bool CompareLessEqualTo::isLessEqual(const FilterColumnData& col, const QString& value, const FilterEvalData& data) const
{
  return (QString::compare( getStringValue(col, data), value, data.caseSensitivity() ) <= 0);
}

bool CompareLessEqualTo::isLessEqual(const FilterColumnData& col, int value, const FilterEvalData& data) const
{
  return ( getVariantValue(col, data) <= value );
}

bool CompareGreaterThan::isGreaterThan(const FilterColumnData& col, const QString& value, const FilterEvalData& data) const
{
  return (QString::compare( getStringValue(col, data), value, data.caseSensitivity() ) > 0);
}

bool CompareGreaterThan::isGreaterThan(const FilterColumnData& col, int value, const FilterEvalData& data) const
{
  return ( getVariantValue(col, data) > value );
}

bool CompareGreaterEqualTo::isGreaterEqual(const FilterColumnData& col, const QString& value, const FilterEvalData& data) const
{
  return (QString::compare( getStringValue(col, data), value, data.caseSensitivity() ) >= 0);
}

bool CompareGreaterEqualTo::isGreaterEqual(const FilterColumnData& col, int value, const FilterEvalData& data) const
{
  return ( getVariantValue(col, data) >= value );
}

}}} // namespace Mdt{ namespace ItemModel{ namespace Expression{
