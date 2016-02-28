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
#ifndef MDT_SQL_COPIER_TABLE_MAPPING_DIALOG_H
#define MDT_SQL_COPIER_TABLE_MAPPING_DIALOG_H

#include "ui_TableMappingDialog.h"
// #include "mdtSqlDatabaseCopierTableMapping.h"
#include "UniqueInsertCriteria.h"
#include <QDialog>
#include <memory>

class mdtSqlDatabaseCopierTableMappingModel;
class mdtComboBoxItemDelegate;
class QItemSelection;

namespace mdt{ namespace sql{ namespace copier{

  /*! \brief SQL database copier table mapping dialog
   */
  class TableMappingDialog : public QDialog, Ui::mdtSqlCopierTableMappingDialog
  {
  Q_OBJECT

  public:

    /*! \brief Constructor
     */
    TableMappingDialog(QWidget *parent = nullptr);

    /*! \internal Copy disabled
     */
    TableMappingDialog(const TableMappingDialog & other) = delete;

    /*! \brief Set source database and list of source tables to display
     *
     * \note db will be reused to fetch selected source table informations.
     */
//     void setSourceTables(const QSqlDatabase & db, const QStringList & tables);

    /*! \brief Set table mapping
     */
//     void setMapping(const std::shared_ptr<mdtSqlDatabaseCopierTableMapping> & m);

    /*! \brief Get table mapping
     */
//     std::shared_ptr<mdtSqlDatabaseCopierTableMapping> mapping() const;

  private slots:

    /*! \brief Set source table
     *
     * Called when cbSourceTable's index changed.
     */
//     void setSourceTable(int cbIndex);

    /*! \brief Reset field mapping
     */
    void resetFieldMapping();

    /*! \brief Map by field names
     */
    void mapByFieldName();

    /*! \brief Edit related table expression
    */
    void editRelatedTableExpression();

    /*! \brief Set unique insert criteria enabled/disabled
     */
    void setUniqueInsertCriteriaEnabled(bool enable);

    /*! \brief Edit unique insert criteria
     */
    void editUniqueInsertCriteria();

    /*! \brief Resize table view
     */
    void resizeTableViewToContents();

    /*! \brief Set selected row
     *
     * \note this will just update pvSelectedRow
     */
    void setSelectedRowFromSelection(const QItemSelection & selected, const QItemSelection & deselected);

  private:

    /*! \brief Set selected row
     *
     * \note this will just update pvSelectedRow
     */
    void setSelectedRow(int row);

//     mdtSqlDatabaseCopierTableMappingModel *pvMappingModel;
    mdtComboBoxItemDelegate *pvSourceFieldSelectionDelegate;
    int pvSelectedRow;
    ///QSqlDatabase pvSourceDatabase;
    mdt::sql::copier::UniqueInsertCriteria pvEditingUniqueInsertCriteria;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_TABLE_MAPPING_DIALOG_H
