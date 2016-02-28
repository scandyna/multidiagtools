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
#ifndef MDT_SQL_COPIER_DATABASE_COPIER_TABLE_MAPPING_DIALOG_H
#define MDT_SQL_COPIER_DATABASE_COPIER_TABLE_MAPPING_DIALOG_H

#include "TableMappingDialog.h"
#include "mdtSqlDatabaseCopierTableMapping.h"
#include <QSqlDatabase>

class mdtSqlDatabaseCopierTableMappingModel;
class QComboBox;

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief SQL database copier table mapping dialog
   */
  class DatabaseCopierTableMappingDialog : public TableMappingDialog
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    DatabaseCopierTableMappingDialog(QWidget *parent = nullptr);

    /*! \brief Set source database and list of source tables to display
     */
    void setSourceDatabase(const QSqlDatabase & db);

    /*! \brief Set table mapping
     */
    void setMapping(const std::shared_ptr<mdtSqlDatabaseCopierTableMapping> & m);

    /*! \brief Get table mapping
     */
    std::shared_ptr<mdtSqlDatabaseCopierTableMapping> mapping() const;

   private slots:

    /*! \brief Set source table
     *
     * Called when cbSourceTable's index changed.
     */
    void setSourceTable(int cbIndex);

   private:

    /*! \brief Reference internal table mapping (read only version)
     */
    const std::shared_ptr<const TableMapping> mappingBase() const override;

    /*! \brief Reference internal table mapping
     */
    const std::shared_ptr<TableMapping> mappingBase() override;

    /*! \brief Get table mapping model base
     */
    TableMappingModel *mappingModelBase() override;

    /*! \brief (Re-)populate source tables combobox
     */
    void populateSourceTableCombobox(const mdtSqlDatabaseCopierTableMapping * const tm);

    mdtSqlDatabaseCopierTableMappingModel *pvMappingModel;
    QComboBox *cbSourceTable;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_DATABASE_COPIER_TABLE_MAPPING_DIALOG_H
