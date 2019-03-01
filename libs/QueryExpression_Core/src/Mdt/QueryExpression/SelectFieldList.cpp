/****************************************************************************
 **
 ** Copyright (C) 2011-2019 Philippe Steinmann.
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
#include "SelectFieldList.h"
#include "EntityAndField.h"
#include "QueryField.h"
#include <QString>
#include <QStringBuilder>
#include <QLatin1Char>
#include <boost/variant.hpp>
#include <algorithm>
#include <iterator>

namespace Mdt{ namespace QueryExpression{

class ExtractEntityAndFieldVisitor : public boost::static_visitor<QString>
{
 public:

  QString operator()(const NullSelectField &) const
  {
    return QString();
  }

  QString operator()(const SelectAllField &) const
  {
    return QString();
  }

  QString operator()(const EntityAndField & field) const
  {
    return field.entityName() % QLatin1Char('.') % field.fieldName();
  }
};

// class SelectFieldListCompareEntityAndFieldVisitor : public boost::static_visitor<bool>
// {
//  public:
// 
//   SelectFieldListCompareEntityAndFieldVisitor(const EntityAndField & a)
//    : mA(a)
//   {
//   }
// 
//   bool operator()(const NullSelectField &) const
//   {
//     return QString();
//   }
// 
//   bool operator()(const SelectAllField &) const
//   {
//     return QString();
//   }
// 
//   bool operator()(const EntityAndField & field) const
//   {
//     return field.entityName() % QLatin1Char('.') % field.fieldName();
//   }
// 
//  private:
// 
//   const EntityAndField & mA;
// };

void SelectFieldList::addField(const SelectAllField& field)
{
  mList.emplace_back(field);
}

void SelectFieldList::addField(const SelectField& field)
{
  mList.push_back(field);
}

void SelectFieldList::addField(const QString & fieldName, const FieldAlias & fieldAlias)
{
  Q_ASSERT(!fieldName.trimmed().isEmpty());

  mList.emplace_back(fieldName, fieldAlias);
}

void SelectFieldList::addField(const QueryEntity & entity, const QString & fieldName, const FieldAlias & fieldAlias)
{
  Q_ASSERT(!entity.isNull());
  Q_ASSERT(!fieldName.trimmed().isEmpty());

  mList.emplace_back(entity, fieldName, fieldAlias);
}

int SelectFieldList::fieldIndex(const SelectField & field) const
{
  Q_ASSERT(!field.isNull());

  const ExtractEntityAndFieldVisitor visitor;
  const QString entityAndField = boost::apply_visitor(visitor, field.internalVariant().internalVariant());

  const auto pred = [&entityAndField, &visitor](const SelectField & candidateField){
    const QString candidateEntityAndField = boost::apply_visitor(visitor, candidateField.internalVariant().internalVariant());
    return candidateEntityAndField == entityAndField;
  };
  const auto it = std::find_if( mList.cbegin(), mList.cend(), pred );
  if(it == mList.cend()){
    return -1;
  }

  return std::distance(mList.cbegin(), it);
}

int SelectFieldList::fieldIndex(const QueryField & field) const
{
}

void SelectFieldList::clear()
{
  mList.clear();
}

}} // namespace Mdt{ namespace QueryExpression{
