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

#include "TableMappingModel.h"
#include "DatabaseCopierTableMapping.h"
#include "mdtError.h"
#include <QAbstractTableModel>
#include <QModelIndex>
#include <QSqlField>
#include <QVariant>
#include <memory>

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief Table model to access SQL table copier mapping
   *
   * \sa mdtSqlDatabaseCopierTableMapping
   */
  class DatabaseCopierTableMappingModel : public mdt::sql::copier::TableMappingModel
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    DatabaseCopierTableMappingModel(QObject *parent = nullptr);

    /*! \internal Copy disabled
     */
    DatabaseCopierTableMappingModel(const DatabaseCopierTableMappingModel & other) = delete;

    /*! \brief Set source database
     *
     * Will also reset field mapping.
     */
    void setSourceDatabase(const QSqlDatabase & db);

    /*! \brief Set source table
     *
     * Will also reset field mapping.
     */
    bool setSourceTable(const QString & tableName);

    /*! \brief Set destination table
     *
     * Will also reset field mapping.
     */
    bool setDestinationTable(const QString & tableName, const QSqlDatabase & db);

    /*! \brief Set mapping
     */
    void setMapping(const std::shared_ptr<DatabaseCopierTableMapping> & m);

    /*! \brief Get mapping
     */
    std::shared_ptr<DatabaseCopierTableMapping> mapping() const
    {
      return pvMapping;
    }

  private:

    /*! \brief Reference internal table mapping
    */
    const std::shared_ptr<const mdt::sql::copier::TableMapping> mappingBase() const override
    {
      return pvMapping;
    }

    /*! \brief Reference internal table mapping
    */
    const std::shared_ptr<mdt::sql::copier::TableMapping> mappingBase() override
    {
      return pvMapping;
    }

    std::shared_ptr<DatabaseCopierTableMapping> pvMapping;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_DATABASE_COPIER_TABLE_MAPPING_MODEL_H
