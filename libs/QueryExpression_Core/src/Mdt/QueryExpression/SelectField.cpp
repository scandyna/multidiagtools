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
#include "SelectField.h"
#include "QueryField.h"

namespace Mdt{ namespace QueryExpression{

SelectField::SelectField(const SelectAllField & field)
 : mVariant(field)
{
}

SelectField::SelectField(const QString & fieldName, const FieldAlias & fieldAlias)
 : mVariant( EntityAndField(fieldName, fieldAlias) )
{
  Q_ASSERT(!fieldName.trimmed().isEmpty());
}

SelectField::SelectField(const QueryEntity& entity, const QString & fieldName, const FieldAlias & fieldAlias)
 : mVariant( EntityAndField(entity, fieldName, fieldAlias) )
{
  Q_ASSERT(!entity.isNull());
  Q_ASSERT(!fieldName.trimmed().isEmpty());
}

SelectField::SelectField(const QueryField & field)
 : mVariant( field.entityAndField() )
{
//   Q_ASSERT(!field.isNull());
}

// SelectField::SelectField(const SelectAllField & field)
//  : BaseClass( Expression::make(field) )
// {
// }
// 
// SelectField::SelectField(const QString & fieldName, const FieldAlias & fieldAlias)
//  : BaseClass( Expression::make( EntityAndField(fieldName, fieldAlias) ) )
// {
//   Q_ASSERT(!fieldName.trimmed().isEmpty());
// }
// 
// SelectField::SelectField(const QueryEntity& entity, const QString & fieldName, const FieldAlias & fieldAlias)
//  : BaseClass( Expression::make( EntityAndField(entity, fieldName, fieldAlias) ) )
// {
//   Q_ASSERT(!entity.isNull());
//   Q_ASSERT(!fieldName.trimmed().isEmpty());
// }
// 
// SelectField::SelectField(const QueryField & field)
//  : BaseClass( Expression::make(field.entityAndField()) )
// {
// //   Q_ASSERT(!field.isNull());
// }

}} // namespace Mdt{ namespace QueryExpression{
