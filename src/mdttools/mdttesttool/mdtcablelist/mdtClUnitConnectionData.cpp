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
#include "mdtClUnitConnectionData.h"

#include <QDebug>

void mdtClUnitConnectionData::setKeyData(const mdtClUnitConnectionKeyData &key)
{
  pvKeyData = key;
}

void mdtClUnitConnectionData::setUnitConnectorFk(const mdtClUnitConnectorKeyData &fk)
{
  pvKeyData.setUnitConnectorFk(fk);
}

void mdtClUnitConnectionData::clearUnitConnectorFk()
{
  pvKeyData.clearUnitConnectorFk();
}

void mdtClUnitConnectionData::setArticleConnectionFk(const mdtClArticleConnectionKeyData & fk)
{
  pvKeyData.setArticleConnectionFk(fk);
}

void mdtClUnitConnectionData::clearArticleConnectionFk()
{
  pvKeyData.clearArticleConnectionFk();
}

void mdtClUnitConnectionData::setConnectionType(mdtClConnectionType_t t)
{
  Q_ASSERT(!isBasedOnArticleConnection());
  pvKeyData.setConnectionType(t);
}


void mdtClUnitConnectionData::clear()
{
  pvKeyData.clear();
  name.clear();
  resistance.clear();
  schemaPage.clear();
  signalName.clear();
  swAddress.clear();
  functionEN.clear();
  functionFR.clear();
  functionDE.clear();
  functionIT.clear();
}


/*
 * OLD stuff
 */
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
  Q_ASSERT(contains("ConnectionType_Code_FK"));
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

mdtClArticleConnectionData& mdtClUnitConnectionData::articleConnectionData()
{
  return pvArticleConnectionData;
}

void mdtClUnitConnectionData::setArticleConnectionData(const mdtClArticleConnectionData & data)
{
  setValue("ArticleConnection_Id_FK", data.value("Id_PK"));
  pvArticleConnectionData = data;
}

// bool mdtClUnitConnectionData::isBasedOnArticleConnection() const
// {
//   return !value("ArticleConnection_Id_FK").isNull();
// }
