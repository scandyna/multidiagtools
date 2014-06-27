/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtTtTestNodeUnitSetupData.h"
#include <QString>

mdtTtTestNodeUnitSetupData::mdtTtTestNodeUnitSetupData()
{
}

mdtTtTestNodeUnitSetupData::mdtTtTestNodeUnitSetupData(const QSqlRecord & record)
 : mdtSqlRecord(record)
{
  Q_ASSERT(contains("TestModelItem_Id_FK"));
  Q_ASSERT(contains("TestNodeUnit_Id_FK"));
  Q_ASSERT(contains("State"));
  Q_ASSERT(contains("Value"));
}

bool mdtTtTestNodeUnitSetupData::setup(const QSqlDatabase& db)
{
  if(!addAllFields("TestNodeUnitSetup_tbl", db)){
    return false;
  }

  return true;
}
