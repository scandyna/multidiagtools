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
#include "UniqueInsertCriteria.h"
#include <QDialog>
#include <memory>

class mdtComboBoxItemDelegate;
class QItemSelection;
class QLayout;

namespace mdt{ namespace sql{ namespace copier{

  class TableMapping;
  class TableMappingModel;

  /*! \brief SQL copier table mapping dialog
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

  protected:

    /*! \brief Set layout that contains source selection widgets
     */
    void setSourceTableLayout(QLayout *l);

    /*! \brief Set table mapping model
     *
     * \pre Subclass must call this function exactly once
     */
    void setModel(TableMappingModel *model);

    /*! \brief Update table mapping
     *
     * Must be called by subclass each time its table mapping changed
     */
    void updateMapping();

    /*! \brief Update source table field selection delegate
     */
    void updateSourceTableFieldSelectionDelegate();

//     /*! \brief Access source field selection delegate
//      */
//     mdtComboBoxItemDelegate *sourceFieldSelectionDelegate()
//     {
//       return pvSourceFieldSelectionDelegate;
//     }

    /*! \brief Reference internal table mapping (read only version)
     */
    virtual const std::shared_ptr<const TableMapping> mappingBase() const = 0;

    /*! \brief Reference internal table mapping
     */
    virtual const std::shared_ptr<TableMapping> mappingBase() = 0;

    /*! \brief Get table mapping model base
     */
    virtual TableMappingModel *mappingModelBase() = 0;

  private slots:

    /*! \brief Resize table view
     */
    void resizeTableViewToContents();

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

    mdtComboBoxItemDelegate *pvSourceFieldSelectionDelegate;
    int pvSelectedRow;
    UniqueInsertCriteria pvEditingUniqueInsertCriteria;
  };

}}} // namespace mdt{ namespace sql{ namespace copier{

#endif // #ifndef MDT_SQL_COPIER_TABLE_MAPPING_DIALOG_H
