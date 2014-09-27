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
#ifndef MDT_SQL_TABLE_VIEW_CONTROLLER_H
#define MDT_SQL_TABLE_VIEW_CONTROLLER_H

#include "mdtAbstractSqlTableController.h"
#include "mdtSqlTableSelection.h"
#include <QStyledItemDelegate>
#include <QModelIndex>
#include <QString>
#include <QStringList>

class QTableView;
class QAbstractItemDelegate;
class QEvent;

/*! \brief Item delegate for SQL table view controller
 *
 * To handle internal state machine correctly
 *  we need a signal that indicates that user begins to edit a item.
 *  We create a custom delegate that provide this signal.
 */
class mdtSqlTableViewControllerItemDelegate : public QStyledItemDelegate
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlTableViewControllerItemDelegate(QObject *parent);

  /*! \brief Re-implementation to emit dataEditionBegins()
   */
  QWidget *createEditor(QWidget * parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const;

  /*! \brief Re-implementation to emit dataEditionDone()
   */
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex & index) const;

 signals:

  /*! \brief Emited when data edition begins
   */
  void dataEditionBegins() const;

  /*! \brief Emited when data edition begins
   */
  void dataEditionDone() const;

};


/*! \brief Table view controller
 */
class mdtSqlTableViewController : public mdtAbstractSqlTableController
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlTableViewController(QObject* parent = 0);

  /*! \brief Destructor
   */
  ~mdtSqlTableViewController();

  /*! \brief Set table view to control
   *
   * To handle states correctly during edition,
   *  a item delegate that emits some signals is needed:
   *   - void dataEditionBegins() : when data editing beginns
   *   - void dataEditionDone() : when data editing is finished
   *
   *  If no delegate is given, a default will be created.
   */
  void setTableView(QTableView *tv, QAbstractItemDelegate *delegate = 0);

  /*! \brief Set default column to select
   *
   * Some methods have to update selection in table view.
   *  Here it's possible to define witch column must be selected
   *  in such case.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  void setDefaultColumnToSelect(int column);

  /*! \brief Set default column to select
   *
   * Same as setDefaultColumnToSelect(int), but you can give a field name.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  void setDefaultColumnToSelect(const QString &fieldName);

  /*! \brief Get column index of first visible column
   */
  int firstVisibleColumnIndex();

  /*! \brief Get current row
   */
  ///int currentRow() const;

  /*! \brief Get current selection
   *
   * The returned selection will only contain data related
   *  to given field list.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  mdtSqlTableSelection currentSelection(const QStringList &fieldList);

  /*! \brief Get current selection
   *
   * The returned selection will only contain data related
   *  to given field.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  mdtSqlTableSelection currentSelection(const QString &field);

 private slots:

  /*! \brief Will set pvTableView pointer to Null
   */
  void onTableViewDestroyed(QObject *obj);

  /*! \brief Update current row
   */
  void onTableViewcurrentRowChanged(const QModelIndex & current, const QModelIndex & previous);

  /*! \brief Activity after Visualizing state entered
   */
  void onStateVisualizingEntered();

  /*! \brief Activity after Editing state entered
   *
   * Will unset pvEditionDone
   */
  void onStateEditingEntered();

  /*! \brief Activity after EditingNewRow state entered
   *
   * Will unset pvEditionDone
   */
  void onStateEditingNewRowEntered();

  /*! \brief Set pvEditionDone flag
   */
  void onEditionDone();

 private:

  /*! \brief Wait until edition is done
   */
  void waitEditionDone();

  /*! \brief Current row changed event
   */
  void currentRowChangedEvent(int row);

  /*! \brief Insert done event
   */
  void insertDoneEvent(int row);

  /*! \brief Table model set event
   */
  void modelSetEvent();

  /*! \brief Submit current row to model
   */
  bool doSubmit();

  /*! \brief Revert current row from model
   */
  bool doRevert();

  /*! \brief Insert a new row to model
   */
  ///bool doInsert();

  /*! \brief Submit new row to model
   */
  bool doSubmitNewRow();

  /*! \brief Revert new row
   */
  ///bool doRevertNewRow();

  /*! \brief Remove current row from model
   */
  bool doRemove();

  Q_DISABLE_COPY(mdtSqlTableViewController);

  QTableView *pvTableView;
  int pvDefaultColumnToSelect;
  bool pvEditionDone;
  bool pvRowChangingByInternalEvent;
};

#endif // #ifndef MDT_SQL_TABLE_VIEW_CONTROLLER_H
