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
#include "mdtTtTestSystemComponent.h"
#include "mdtSqlRecord.h"

mdtTtTestSystemComponent::mdtTtTestSystemComponent(QObject* parent, QSqlDatabase db)
 : mdtTtBase(parent, db)
{
}

mdtTtTestSystemComponent::mdtTtTestSystemComponent(QSqlDatabase db)
 : mdtTtBase(nullptr, db)
{
}

bool mdtTtTestSystemComponent::addTestSystemAssignation(const mdtTtTestSystemAssignationData & data)
{
  mdtSqlRecord record;

  if(!record.addAllFields("TestSystem_TestSystemComponent_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  record.setValue("TestSystem_Id_FK", data.testSystemId);
  record.setValue("TestSystemComponent_Id_FK", data.testSystemComponentId);

  return addRecord(record, "TestSystem_TestSystemComponent_tbl");
}

bool mdtTtTestSystemComponent::removeTestSystemAssignation(const mdtTtTestSystemAssignationData & data)
{
  return removeData("TestSystem_TestSystemComponent_tbl", "TestSystem_Id_FK", data.testSystemId, "TestSystemComponent_Id_FK", data.testSystemComponentId);
}

bool mdtTtTestSystemComponent::removeTestSystemAssignation(const mdtSqlTableSelection& s)
{
  return removeData("TestSystem_TestSystemComponent_tbl", s, true);
}
