/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#include "FieldNameValueMap.h"

namespace Mdt{ namespace Sql{

void FieldNameValueMap::addValue(const FieldName& fieldName, const QVariant& value)
{
  Q_ASSERT(!fieldName.isNull());
  Q_ASSERT(!containsFieldName(fieldName.toString()));

  mMap.emplace_back( fieldName.toString(), value );
}

QVariant FieldNameValueMap::value(const QString& fieldName) const
{
  Q_ASSERT(!fieldName.trimmed().isEmpty());

  const auto it = findIteratorForFieldName(fieldName);
  if(it != mMap.cend()){
    return it->second;
  }

  return QVariant();
}

void FieldNameValueMap::clear()
{
  mMap.clear();
}

QStringList FieldNameValueMap::toFieldNameList() const
{
  QStringList fieldNames;

  fieldNames.reserve(mMap.size());
  for(const auto & item : mMap){
    fieldNames.append( item.first );
  }

  return fieldNames;
}

QVariantList FieldNameValueMap::toValueList() const
{
  QVariantList values;

  values.reserve(mMap.size());
  for(const auto & item : mMap){
    values.append( item.second );
  }

  return values;
}

bool FieldNameValueMap::containsFieldName(const QString& fieldName) const
{
  Q_ASSERT(!fieldName.trimmed().isEmpty());

  return (findIteratorForFieldName(fieldName) != mMap.cend());
}

bool FieldNameValueMap::compareFieldNames(const QString& a, const QString& b)
{
  return (QString::compare(a, b, Qt::CaseInsensitive) == 0);
}

FieldNameValueMap::const_iterator FieldNameValueMap::findIteratorForFieldName(const QString& fieldName) const
{
  const auto cmp = [&fieldName](const MapItem & item){
    return compareFieldNames(item.first, fieldName);
  };

  return std::find_if(mMap.cbegin(), mMap.cend(), cmp);
}

}} // namespace Mdt{ namespace Sql{
