/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#ifndef MDT_SQL_TABLE_SELECTION_H
#define MDT_SQL_TABLE_SELECTION_H

class QSqlQueryModel;

#include "mdtSqlTableSelectionRow.h"
#include <QList>
#include <QModelIndex>
#include <QString>
#include <QStringList>
#include <QVariant>

/*! \brief Handle selection of model indexes in a QTableView
 *
 * Qt offers some classes to handle selections in item view/model
 *  classes.
 *
 * But, in database applications, it's common that the user can select one or many items,
 *  and we want to get a result for selected rows, and only some fields.
 *
 * For example, say we have a table:
 *
 *  <table border="1" cellpadding="5">
 *    <tr><th>Id_PK</th><th>Name</th><th>Remarks</th></tr>
 *    <tr><td>1</td><td>User 1</td><td>A remark on User 1</td></tr>
 *    <tr><td>2</td><td>User 2</td><td>A remark on User 2</td></tr>
 *  </table>
 *
 * This table is displayed in a QTableView and data are handled by a QSqlTableModel.
 *  For commodity, we let the user select individual items (not complete rows).
 *  Now, its possible to edit a user by double clicking on a row. Or, it's also possible
 *  to remove several users by selecting corresponding rows, and clicking a push button (or a menu item, ...).
 *  The data we need, is in Id_PK field, witch is probably not shown to the user. If hi wants to edit, hi will
 *   probably double click on "User 1" item in view. Here, we need to get value of Id_PK of row 0 (=1).
 *
 * This class is used by mdtSqlTableWidget, mdtSqlSelectionDialog.
 */
class mdtSqlTableSelection
{
 public:
 
  /*! \brief Constructor
   */
  mdtSqlTableSelection();

  /*! \brief Clear selection (will not affect any model, view or selection model)
   */
  void clear();

  /*! \brief Set model index list
   *
   * \param indexList Typically a result returned from QItemSelectionModel::selectedIndexes()
   * \param fieldList List of fields that are needed
   * \param model Needed to get some informations (typically field names related to a model index)
   * \pre model must be a valid pointer
   * \pre Each model index in indexList must point to given model
   *
   * Note: calling this method will clear previous data.
   */
  void setIndexList(const QModelIndexList & indexList, const QStringList & fieldList, QSqlQueryModel *model);

  /*! \brief Get row count
   */
  inline int rowCount() const { return pvRowList.size(); }

  /*! \brief Get data for given row and field name
   *
   * Note: row does not match any model index, it's a row index of internally stored selection rows.
   *
   * \pre row must be in a valid range ( 0 <= row < rowCount() ).
   */
  QVariant data(int row, const QString & fieldName) const;

  /*! \brief Get a list of data for given field
   */
  QList<QVariant> dataList(const QString & fieldName) const;

 private:

  QList<mdtSqlTableSelectionRow> pvRowList;
};

#endif // #ifndef MDT_SQL_TABLE_SELECTION_H
