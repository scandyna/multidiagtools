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
#include "DriverPostgreSQL.h"
#include "FieldTypeName.h"
#include <QStringBuilder>

namespace Mdt{ namespace Sql{ namespace Schema{

DriverPostgreSQL::DriverPostgreSQL(const QSqlDatabase& db)
 : DriverImplementationInterface(db)
{
  Q_ASSERT(qsqlDriver()->dbmsType() == QSqlDriver::PostgreSQL);
}

QString DriverPostgreSQL::getCollationDefinition(const Collation& collation) const
{

}

QString DriverPostgreSQL::getFieldDefinition(const Field& field) const
{

}

QString DriverPostgreSQL::getPrimaryKeyFieldDefinition(const AutoIncrementPrimaryKey& pk) const
{

}

QString DriverPostgreSQL::getPrimaryKeyFieldDefinition(const SingleFieldPrimaryKey& pk) const
{

}


}}} // namespace Mdt{ namespace Sql{ namespace Schema{
