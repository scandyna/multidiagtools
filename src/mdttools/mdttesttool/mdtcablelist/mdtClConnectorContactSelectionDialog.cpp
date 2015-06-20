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
#include "mdtClConnectorContactSelectionDialog.h"
#include <QStringList>

mdtClConnectorContactSelectionDialog::mdtClConnectorContactSelectionDialog(QWidget *parent)
 : mdtSqlSelectionDialog(parent)
{
  resize(400, 300);
  setWindowTitle(tr("Contacts selection"));
}

bool mdtClConnectorContactSelectionDialog::select(QSqlDatabase db, const mdtClConnectorKeyData & connectorKey, bool allowMultiSelection)
{
  QString sql;

  sql = "SELECT * FROM ConnectorContact_tbl ";
  sql += " WHERE Connector_Id_FK = " + connectorKey.id.toString();
  setMessage("Please select contacts to use:");
  if(!setQuery(sql, db, allowMultiSelection)){
    return false;
  }
  setColumnHidden("Id_PK", true);
  setColumnHidden("Connector_Id_FK", true);
  addColumnToSortOrder("Name", Qt::AscendingOrder);
  sort();

  return true;
}

mdtClConnectorContactData mdtClConnectorContactSelectionDialog::selectedContactData() const
{
  mdtClConnectorContactData data;
  QList<mdtClConnectorContactData> dataList;

  if(result() != Accepted){
    return data;
  }
  dataList = selectedContactDataList();
  if(dataList.isEmpty()){
    return data;
  }
  Q_ASSERT(dataList.size() == 1);
  data = dataList.at(0);

  return data;
}

QList<mdtClConnectorContactData> mdtClConnectorContactSelectionDialog::selectedContactDataList() const
{
  QList<mdtClConnectorContactData> dataList;
  QStringList fields;

  if(result() != Accepted){
    return dataList;
  }
  fields << "Id_PK" << "Connector_Id_FK" << "ConnectionType_Code_FK" << "Name";
  auto s = selection(fields);
  dataList.reserve(s.rowCount());
  for(int row = 0; row < s.rowCount(); ++row){
    mdtClConnectorContactKeyData key;
    mdtClConnectorContactData data;
    mdtClConnectorKeyData connectorFk;
    key.id = s.data(row, "Id_PK");
    connectorFk.id = s.data(row, "Connector_Id_FK");
    key.setConnectorFk(connectorFk);
    key.setConnectionTypeCode(s.data(row, "ConnectionType_Code_FK"));
    data.setKeyData(key);
    data.name = s.data(row, "Name");
    dataList.append(data);
  }

  return dataList;
}
