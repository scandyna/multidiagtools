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
#include "mdtClUnitConnectionData.h"

mdtClUnitConnectionData::mdtClUnitConnectionData()
 : mdtSqlRecord()
{
}

/// \todo Check that the generated flag is updated (use hasData() for check...)
mdtClUnitConnectionData::mdtClUnitConnectionData(const QSqlRecord & record)
 : mdtSqlRecord(record)
{
  Q_ASSERT(indexOf("Id_PK") >= 0);
  Q_ASSERT(indexOf("Unit_Id_FK") >= 0);
  Q_ASSERT(indexOf("UnitConnector_Id_FK") >= 0);
  Q_ASSERT(indexOf("ArticleConnection_Id_FK") >= 0);
  Q_ASSERT(indexOf("UnitContactName") >= 0);
  Q_ASSERT(indexOf("SchemaPage") >= 0);
  Q_ASSERT(indexOf("SwAddress") >= 0);
  Q_ASSERT(indexOf("FunctionEN") >= 0);
  Q_ASSERT(indexOf("FunctionFR") >= 0);
  Q_ASSERT(indexOf("FunctionDE") >= 0);
  Q_ASSERT(indexOf("FunctionIT") >= 0);
}

bool mdtClUnitConnectionData::setup(const QSqlDatabase & db, bool setupAcd) 
{
  if(!addAllFields("UnitConnection_tbl", db)){
    return false;
  }
  if(setupAcd){
    return pvArticleConnectionData.setup(db);
  }
  return true;
}

const mdtClArticleConnectionData & mdtClUnitConnectionData::articleConnectionData() const
{
  return pvArticleConnectionData;
}

void mdtClUnitConnectionData::setArticleConnectionData(const mdtClArticleConnectionData& data)
{
  setValue("ArticleConnection_Id_FK", data.value("Id_PK"));
  pvArticleConnectionData = data;
}

bool mdtClUnitConnectionData::isBasedOnArticleConnection() const
{
  return !value("ArticleConnection_Id_FK").isNull();
}
