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
#ifndef MDT_SQL_CSV_FILE_IMPORT_TABLE_MAPPING_MODEL_H
#define MDT_SQL_CSV_FILE_IMPORT_TABLE_MAPPING_MODEL_H

#include "mdt/sql/copier/TableMappingModel.h"
#include "mdtSqlCsvFileImportTableMapping.h"
#include "mdtError.h"
#include <QModelIndex>
#include <QSqlField>
#include <QVariant>
#include <QString>
#include <memory>

class mdtComboBoxItemDelegate;

/*! \brief Table model to access SQL CSV file import table mapping
 *
 * \sa mdtSqlCsvFileImportTableMapping
 */
class mdtSqlCsvFileImportTableMappingModel : public mdt::sql::copier::TableMappingModel
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlCsvFileImportTableMappingModel(QObject *parent = nullptr);

  /*! \brief Set source CSV file
   *
   * Will also reset field mapping.
   *
   * If delegate is not null,
   *  its combobox will be populated with available fields.
   */
  bool setSourceCsvFile(const QFileInfo & csvFile, const QByteArray & csvFileEncoding, const mdtCsvParserSettings & settings, mdtComboBoxItemDelegate *delegate = nullptr);

  /*! \brief Set destination table
   *
   * Will also reset field mapping.
   */
  bool setDestinationTable(const QString & tableName, const QSqlDatabase & db);

 private:

  /*! \brief Reference internal table mapping
   */
  const std::shared_ptr<const mdt::sql::copier::TableMapping> mappingBase() const override
  {
    return pvMapping;
  }

//   const mdt::sql::copier::TableMapping & mappingBase() const
//   {
//     return pvMapping;
//   }

  /*! \brief Reference internal table mapping
   */
  const std::shared_ptr<mdt::sql::copier::TableMapping> mappingBase() override
  {
    return pvMapping;
  }

//   mdt::sql::copier::TableMapping & mappingBase()
//   {
//     return pvMapping;
//   }

  ///mdtSqlCsvFileImportTableMapping pvMapping;
  std::shared_ptr<mdtSqlCsvFileImportTableMapping> pvMapping;
};

#endif // #ifndef MDT_SQL_CSV_FILE_IMPORT_TABLE_MAPPING_MODEL_H
