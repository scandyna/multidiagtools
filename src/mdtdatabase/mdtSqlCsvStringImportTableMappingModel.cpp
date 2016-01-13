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
#include "mdtSqlCsvStringImportTableMappingModel.h"
#include "mdtComboBoxItemDelegate.h"

mdtSqlCsvStringImportTableMappingModel::mdtSqlCsvStringImportTableMappingModel(QObject* parent)
 : mdtSqlCopierTableMappingModel(parent)
{
}

bool mdtSqlCsvStringImportTableMappingModel::setSourceCsvString(const QString & csv, const mdtCsvParserSettings & settings, mdtComboBoxItemDelegate* delegate)
{
  bool ok;

  beginResetModel();
  ok = pvMapping.setSourceCsvString(csv, settings);
  if( (ok) && (delegate != nullptr) ){
    delegate->clear();
    delegate->addItem("");
    delegate->addItems(pvMapping.getSourceTableFieldNameList());
  }
  endResetModel();
  if(!ok){
    pvLastError = pvMapping.lastError();
    return false;
  }

  return true;
}

bool mdtSqlCsvStringImportTableMappingModel::setDestinationTable(const QString & tableName, const QSqlDatabase & db)
{
  bool ok;

  beginResetModel();
  ok = pvMapping.setDestinationTable(tableName, db);
  endResetModel();
  if(!ok){
    pvLastError = pvMapping.lastError();
    return false;
  }

  return true;
}
