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
#include "mdtSqlDatabaseSchema.h"
#include "mdtSqlDatabaseSchemaThread.h"

void mdtSqlDatabaseSchema::addTable(const mdtSqlSchemaTable & st)
{
  pvTableList.append(st);
}

void mdtSqlDatabaseSchema::addView(const mdtSqlViewSchema::Schema & vs)
{
  pvViewList.append(vs);
}

void mdtSqlDatabaseSchema::addTablePopulation(const mdtSqlTablePopulationSchema& tps)
{
  pvTablePopulationSchemaList.append(tps);
}

bool mdtSqlDatabaseSchema::createSchema(const QSqlDatabase& db)
{
  mdtSqlDatabaseSchemaThread thread;

  return thread.createSchemaBlocking(*this, db);
}
