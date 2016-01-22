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
#ifndef MDT_SQL_DATABASE_COPIER_TABLE_MAPPING_MODEL_H
#define MDT_SQL_DATABASE_COPIER_TABLE_MAPPING_MODEL_H

#include "mdt/sql/copier/TableMappingModel.h"
#include "mdtSqlDatabaseCopierTableMapping.h"
#include "mdtError.h"
#include <QAbstractTableModel>
#include <QModelIndex>
#include <QSqlField>
#include <QVariant>
#include <memory>

class mdtComboBoxItemDelegate;

/*! \brief Table model to access SQL table copier mapping
 *
 * \sa mdtSqlDatabaseCopierTableMapping
 */
class mdtSqlDatabaseCopierTableMappingModel : public mdt::sql::copier::TableMappingModel
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlDatabaseCopierTableMappingModel(QObject *parent = nullptr);

  /*! \internal Copy disabled
   */
  mdtSqlDatabaseCopierTableMappingModel(const mdtSqlDatabaseCopierTableMappingModel & other) = delete;

  /*! \brief Set source table
   *
   * Will also reset field mapping.
   *
   * If delegate is not null,
   *  its combobox will be populated with available fields.
   */
  bool setSourceTable(const QString & tableName, const QSqlDatabase & db, mdtComboBoxItemDelegate *delegate = nullptr);

  /*! \brief Set destination table
   *
   * Will also reset field mapping.
   */
  bool setDestinationTable(const QString & tableName, const QSqlDatabase & db);

  /*! \brief Set mapping
   */
  void setMapping(const std::shared_ptr<mdtSqlDatabaseCopierTableMapping> & m);

  /*! \brief Get mapping
   */
  std::shared_ptr<mdtSqlDatabaseCopierTableMapping> mapping() const
  {
    return pvMapping;
  }
//   mdtSqlDatabaseCopierTableMapping mapping() const
//   {
//     return pvMapping;
//   }

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

  ///mdtSqlDatabaseCopierTableMapping pvMapping;
  std::shared_ptr<mdtSqlDatabaseCopierTableMapping> pvMapping;
};

#endif // #ifndef MDT_SQL_DATABASE_COPIER_TABLE_MAPPING_MODEL_H
