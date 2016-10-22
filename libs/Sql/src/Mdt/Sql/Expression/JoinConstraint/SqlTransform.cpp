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
#include "SqlTransform.h"
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QStringBuilder>
#include <QSqlField>

namespace Mdt{ namespace Sql{ namespace Expression{ namespace JoinConstraint{

QString GetTerminalSql::operator()(const TableField & /*f*/, const QSqlDatabase & /*db*/) const
{
  return "TermSql";
}

QString GetTerminalSql::operator()(const QVariant & value, const QSqlDatabase & db) const
{
  Q_ASSERT(db.driver() != nullptr);
  
  QSqlField field("", value.type());
  field.setValue(value);

  return db.driver()->formatValue(field);
}



QString GetCompareEqualSql::operator()(const QString & left, const QString & right) const
{
  return left % QStringLiteral("=") % right;
}

QString GetLogicalAndSql::operator()(const QString& left, const QString& right) const
{
  return left % QStringLiteral(" AND ") % right;
}

}}}} // namespace Mdt{ namespace Sql{ namespace Expression{ namespace JoinConstraint{