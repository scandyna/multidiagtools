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
#ifndef MDT_SQL_TABLE_WIDGET_H
#define MDT_SQL_TABLE_WIDGET_H

#include "mdtAbstractSqlWidget.h"

#include "mdtSqlTableViewController.h"
#include "mdtSqlTableSelection.h"
#include <QModelIndex>
#include <QStyledItemDelegate>
#include <QString>
#include <QStringList>
#include <QWidget>
#include <QSqlDatabase>
#include <memory>

class QTableView;
class QHBoxLayout;
class QPushButton;
class QItemSelectionModel;
///class QWidget;
class QKeyEvent;

/*
 * We use on manual submit edit strategy,so we can handle errors
 *  (display message to the user and let him the choice to revert, correct, wait, ...).
 * But, this way the user must allways click on save and insert button,
 *  which is not a fast solution for table based editor.
 * This class helps to catch key press events, and permits saving and inserting rows as expected.
 */
/**
class mdtSqlTableWidgetKeyEventFilter : public QObject
{
 Q_OBJECT

 public:
  mdtSqlTableWidgetKeyEventFilter(QObject *parent);

 signals:
  void knownKeyPressed(int key);

 protected:
  bool eventFilter(QObject *obj, QEvent *event);
};
*/

/*
 * To handle internal state machine correctly
 *  we need a signal that indicates that user begins to edit a item.
 *  We create a custom delegate that provide this signal.
 */
/**
class mdtSqlTableWidgetItemDelegate : public QStyledItemDelegate
{
 Q_OBJECT

 public:
  mdtSqlTableWidgetItemDelegate(QObject *parent);
  QWidget *createEditor(QWidget * parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const;

 signals:
  void dataEditionBegins() const;
};
*/

/*! \brief Table view based SQL widget
 *
 * Internally, QTableView is used.
 *  This class add some common needed methods,
 *  like insertion, removing, ...
 */
///class mdtSqlTableWidget : public mdtAbstractSqlWidget
class mdtSqlTableWidget :  public QWidget
{
 Q_OBJECT

 public:

  /**
   * \todo TO REMOVE !!!!!!
   */
  QSqlTableModel *model() { return 0; }
  void setModel(QSqlTableModel *m) {}
  void addChildWidget(mdtAbstractSqlWidget *widget, mdtSqlRelation *relation){}
  void addChildWidget(mdtSqlTableWidget *widget, mdtSqlRelation *relation){}

  /*! \brief Constructor
   *
   * Setup a QTableView and init some other things.
   * The table view will not be editable, use setEditionEnabled() to alter this.
   */
  mdtSqlTableWidget(QWidget *parent = 0);

  /*! \brief Destructor
   */
  ~mdtSqlTableWidget();

  /*! \brief Set table name
   */
  void setTableName(const QString & tableName, QSqlDatabase db, const QString & userFriendlyTableName = QString());

  /*! \brief Set user friendly table name
   */
  void setUserFriendlyTableName(const QString & name) { pvController->setUserFriendlyTableName(name); }

  /*! \brief Get table name
   */
  QString tableName() const { return pvController->tableName(); }

  /*! \brief Set table view controller
   *
   * By default, a controller is created.
   *  For simple cases, use setTableName().
   *
   * This method will replace internal controller
   *  with given one.
   */
  void setTableController(std::shared_ptr<mdtSqlTableViewController> controller);

  /*! \brief Add a child table
   *
   * \param relationInfo Informations of relation between current controller's table (parent) and child controller's table (child)
   *               Note: parent table name is ignored.
   * \param db QSqlDatabase object to use
   * \param userFriendlyChildTableName User friendly table name for child controller.
   *
   * \pre Table model must be set with setTableName() begore calling this method.
   */
  bool addChildTable(const mdtSqlRelationInfo & relationInfo, QSqlDatabase db, const QString & userFriendlyChildTableName = QString());

  /*! \brief Add a child table
   *
   * \param relationInfo Informations of relation between current controller's table (parent) and child controller's table (child)
   *               Note: parent table name is ignored.
   * \param userFriendlyChildTableName User friendly table name for child controller.
   *
   * \pre Table model must be set with setTableName() begore calling this method.
   */
  bool addChildTable(const mdtSqlRelationInfo & relationInfo, const QString & userFriendlyChildTableName = QString());

  /*! \brief Get internal table controller for given tableName
   *
   * Can return a Null pointer if tableName was not found.
   */
  std::shared_ptr<mdtSqlTableViewController> tableController(const QString & tableName);

  /*! \brief Start internal state machine
   *
   * \pre Table model must be set with setTableName() begore calling this method.
   */
  void start() { pvController->start(); }

  /*! \brief Stop internal state machine
   *
   * \pre Table model must be set with setTableName() begore calling this method.
   */
  void stop() { pvController->stop(); }

  /*! \brief Select data in main table
   *
   * \pre Table model must be set with setTableName() begore calling this method.
   * \pre Internal state machine must run (see start() ).
   */
  bool select() { return pvController->select(); }

  /*! \brief Get the current row
   */
  inline int currentRow() const { return pvController->currentRow(); }

  /*! \brief Get current data for given field name
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  inline QVariant currentData(const QString &fieldName)
  {
    return pvController->currentData(fieldName);
  }

  /*! \brief Get current data for given field name
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  inline QVariant currentData(const QString &fieldName, bool & ok)
  {
    return pvController->currentData(fieldName, ok);
  }

  /*! \brief Set edition enabled/disabled
   */
  void setEditionEnabled(bool enable);

  /*! \brief Enable local edition
   *
   * Will add insert/revert/save/delete buttons.
   *
   * In normal way, when using mdtSqlWindow,
   *  local navigation is not needed.
   *  In master/detail forms, if this widget
   *  is a details (child) widget and edition
   *  is needed, then call this method.
   */
  void enableLocalEdition();

  /*! \brief Add a custom widget to local bar
   *
   * Typical usage can be to add a QPushButton
   *  for custom usage (edit, select, ...)
   *
   * Note: widget will be parented to layout,
   *  so Qt will delete it itself.
   *
   * \pre widget must be a valid pointer.
   */
  void addWidgetToLocalBar(QWidget *widget);

  /*! \brief Add a strech to local bar
   */
  void addStretchToLocalBar();

  /*! \brief Get internal table view's selection model
   */
  QItemSelectionModel *selectionModel();

  /*! \brief Get internal table view
   */
  QTableView *tableView();

  /*! \brief Set a user friendly name for a column
   *
   * \pre Model must be set with setModel() before using this method.
   */
  void setHeaderData(const QString &fieldName, const QString &data);

  /*! \brief Set a coulumn as hidden
   */
  void setColumnHidden(int column, bool hide);

  /*! \brief Set a coulumn as hidden
   *
   * \pre Model must be set with setModel() before using this method.
   */
  void setColumnHidden(const QString &fieldName, bool hide);

  /*! \brief Set default column to select
   *
   * Some methods have to update selection in table view.
   *  Here it's possible to define witch column must be selected
   *  in such case.
   *
   * \pre Model must be set with setModel() before using this method.
   */
  void setDefaultColumnToSelect(int column);

  /*! \brief Set default column to select
   *
   * Same as setDefaultColumnToSelect(int), but you can give a field name.
   *
   * \pre Model must be set with setModel() before using this method.
   */
  void setDefaultColumnToSelect(const QString &fieldName);

  /*! \brief Add a column to columns sort order
   *
   * Columns sort order is similar meaning
   *  than SQL ORDER BY clause.
   *
   * Internally, mdtSortFilterProxyModel is used, witch provide a natural sort for strings.
   *
   * For example, to sort columns in order "Id_PK", "FirstName", "LastName", all ascending, call:
   *  - clearColumnsSortOrder();
   *  - addColumnToSortOrder("Id_PK", Qt::AscendingOrder);
   *  - addColumnToSortOrder("FirstName", Qt::AscendingOrder);
   *  - addColumnToSortOrder("LastName", Qt::AscendingOrder);
   *
   * Note: if given field not exists, it will simply be ignored.
   *
   * Note: to apply sorting, call sort()
   *
   * \pre Model must be set with setModel() before using this method.
   */
  inline void addColumnToSortOrder(const QString & fieldName, Qt::SortOrder order = Qt::AscendingOrder){
    pvController->addColumnToSortOrder(fieldName, order);
  }

  /*! \brief Clear columns sort order
   *
   * \sa addColumnToSortOrder()
   */
  inline void clearColumnsSortOrder() { pvController->clearColumnsSortOrder(); }

  /*! \brief Sort data
   *
   * \sa addColumnToSortOrder()
   */
  inline void sort() { pvController->sort(); }

  /*! \brief Get current selection
   *
   * The returned selection will only contain data related
   *  to given field list.
   *
   * \pre Model must be set with setModel() before using this method.
   */
  inline mdtSqlTableSelection currentSelection(const QStringList &fieldList) { return pvController->currentSelection(fieldList); }

  /*! \brief Get current selection
   *
   * The returned selection will only contain data related
   *  to given field.
   *
   * \pre Model must be set with setModel() before using this method.
   */
  inline mdtSqlTableSelection currentSelection(const QString &field) { return pvController->currentSelection(field); }

  /*! \brief Get a list of currently selected indexes in a list of rows
   *
   * \deprecated
   *
   * Will return a list of indexes that contains only coulumns
   *  specified by columnList.
   * This alos works for multiple rows selection.
   *
   * \pre Model must be set with setModel() before using this method.
   */
  QList<QModelIndexList> indexListOfSelectedRowsByRowsList(const QList<int> &columnList);

  /*! \brief Get a list of currently selected indexes in a list of rows
   *
   * \overload indexListOfSelectedRowsByRowsList(const QList<int> &)
   */
  QList<QModelIndexList> indexListOfSelectedRowsByRowsList(const QStringList &fieldList);

  /*! \brief Get a list of currently selected indexes
   *
   * \deprecated
   *
   * Will return a list of indexes that contains only coulumns
   *  specified by columnList.
   * This alos works for multiple rows selection.
   *
   * \pre Model must be set with setModel() before using this method.
   */
  QModelIndexList indexListOfSelectedRows(const QList<int> &columnList);

  /*! \brief Get a list of currently selected indexes
   *
   * \deprecated
   *
   * Will return a list of indexes that contains only fields
   *  specified by fieldList.
   * This alos works for multiple rows selection.
   *
   * \pre Model must be set with setModel() before using this method.
   */
  QModelIndexList indexListOfSelectedRows(const QStringList &fieldList);

  /*! \brief Get a list of currently selected indexes
   *
   * \deprecated
   *
   * \overload indexListOfSelectedRows(const QStringList &)
   *
   * This is a variant for single column.
   */
  QModelIndexList indexListOfSelectedRows(const QString &field);

 public slots:

  /*! \brief Set row as current record
   * 
   * \todo REMOVE
   */
  ///void setCurrentIndex(int row);

  /*! \brief Resize view to contents
   */
  void resizeViewToContents();

  /*! \brief Refresh data
   */
  void refresh();

 private slots:

  /*! \internal Set internal delegateIsEditingData flag
   *
   * This slot is called by mdtSqlTableWidgetItemDelegate when editor was created.
   *  See remark in onTableViewKnownKeyPressed()
   *
   * This slot will emit dataEdited()
   */
  ///void onDataEditionBegins();

  /*! \internal Reset internal delegateIsEditingData
   *
   * See remark in onTableViewKnownKeyPressed()
   */
  ///void onDataChanged(const QModelIndex &, const QModelIndex &);

  /*! \brief Does some tasks when entering new row
   */
  ///void onCurrentRowChanged(const QModelIndex & current, const QModelIndex & previous);

  /*! \brief Execute a action after a known key was pressed in table view
   */
  ///void onTableViewKnownKeyPressed(int key);

  /*! \brief Select first row in table view
   *
   * When select() or similar method is called on model,
   *  QTableView's selection is loosed.
   *  If a child widget is attached, it is also not updated.
   * If model has data after select() was called, currentRowChanged() signal
   *  is emitted using table view's selection model,
   *  else this method will emit it with a invalid row (-1).
   */
  ///void onModelSelected();

  /*! \brief Get column index of first visible column
   */
  ///int firstVisibleColumnIndex();

  /*! \brief Copy table to clipboard
   */
  void copyTableToClipBoard();

 private:

  /*! \brief Catch the copy key sequence event
   *
   * Will also call copySelectionToClipBoard() if qey sequence is copy (typically ctrl+c)
   */
  void keyPressEvent(QKeyEvent *event);

  /*! \brief Copy selection to clipboard
   */
  void copySelectionToClipBoard();

  /*! \brief Set model
   *
   * Set the model that handle database.
   *  (See Qt's QSqlTableModel documentation for details).
   *
   * \pre model must be a valid pointer.
   */
  ///void doSetModel(QSqlTableModel *model);

  ///bool doSubmit();

  ///bool doRevert();

  ///bool doInsert();

  ///bool doSubmitNewRow();

  ///bool doRevertNewRow();

  ///bool doRemove();

  /*! \brief Set first record as current record
   */
  ///void toFirst();

  /*! \brief Set last record as current record
   */
  ///void toLast();

  /*! \brief Set previous record as current record
   */
  ///void toPrevious();

  /*! \brief Set next record as current record
   */
  ///void toNext();

  /*! \brief Create local edition elements
   */
  void createLocalEditionElements();

  /*! \brief Make table controller <-> edit actions connection
   */
  void connectLocalEditionSignals();
  void disconnectLocalEditionSignals();

  Q_DISABLE_COPY(mdtSqlTableWidget);

  QTableView *pvTableView;
  std::shared_ptr<mdtSqlTableViewController> pvController;
  QHBoxLayout *pvTopHorizontalLayout;
  QHBoxLayout *pvBottomHorizontalLayout;
  // Navigation buttons
  QPushButton *pbNavToFirst;
  QPushButton *pbNavToPrevious;
  QPushButton *pbNavToNext;
  QPushButton *pbNavToLast;
  // Edition buttons
  QPushButton *pbInsert;
  QPushButton *pbSubmit;
  QPushButton *pbRevert;
  QPushButton *pbRemove;
  // Flags
  ///bool pvDelegateIsEditingData;   // See remark in onTableViewKnownKeyPressed()
  ///int pvDefaultColumnToSelect;    // Used by setCurrentIndex()
};

#endif  // #ifndef MDT_SQL_TABLE_WIDGET_H
