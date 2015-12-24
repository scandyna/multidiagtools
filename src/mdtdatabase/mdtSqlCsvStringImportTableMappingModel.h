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
#ifndef MDT_SQL_CSV_STRING_IMPORT_TABLE_MAPPING_MODEL_H
#define MDT_SQL_CSV_STRING_IMPORT_TABLE_MAPPING_MODEL_H

#include "mdtSqlCopierTableMappingModel.h"
#include "mdtSqlCsvStringImportTableMapping.h"
#include "mdtError.h"
#include <QAbstractTableModel>
#include <QModelIndex>
#include <QSqlField>
#include <QVariant>
#include <QString>

class mdtComboBoxItemDelegate;

/*! \brief Table model to access SQL CSV string import table mapping
 *
 * \sa mdtSqlCsvStringImportTableMapping
 */
class mdtSqlCsvStringImportTableMappingModel : public mdtSqlCopierTableMappingModel
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlCsvStringImportTableMappingModel(QObject *parent = nullptr);

  /*! \brief Set source CSV string
   *
   * Will also reset field mapping.
   *
   * If delegate is not null,
   *  its combobox will be populated with available fields.
   */
  bool setSourceCsvString(const QString & csv, const mdtCsvParserSettings & settings, mdtComboBoxItemDelegate *delegate = nullptr);

  /*! \brief Set destination table
   *
   * Will also reset field mapping.
   */
  bool setDestinationTable(const QString & tableName, const QSqlDatabase & db);

 private:

  /*! \brief Reference internal table mapping
   */
  const mdtSqlCopierTableMapping & mappingBase() const
  {
    return pvMapping;
  }

  /*! \brief Reference internal table mapping
   */
  mdtSqlCopierTableMapping & mappingBase()
  {
    return pvMapping;
  }

  mdtSqlCsvStringImportTableMapping pvMapping;
};

#endif // #ifndef MDT_SQL_CSV_STRING_IMPORT_TABLE_MAPPING_MODEL_H
