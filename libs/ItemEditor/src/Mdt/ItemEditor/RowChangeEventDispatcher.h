/****************************************************************************
 **
 ** Copyright (C) 2011-2017 Philippe Steinmann.
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
#ifndef MDT_ITEM_EDITOR_ROW_CHANGE_EVENT_DISPATCHER_H
#define MDT_ITEM_EDITOR_ROW_CHANGE_EVENT_DISPATCHER_H

#include "RowState.h"
#include <QObject>
#include <QPointer>
#include <QModelIndex>

class QAbstractItemModel;

namespace Mdt{ namespace ItemEditor{

  /*! \brief RowChangeEventDispatcher receives row change events from different objects and signal them when appropriate
   *
   * A RowState contains informations about row count and current row.
   *  Row state has several reasons to change. For example, if a model is set to the controller,
   *  or the model is repopulated, row count changes, and current row can also be lost
   *  (this is the case when using, for example, a QTableView).
   *
   * To keep the user interface of a item editor coherent, all its components must be updated when a row state changes.
   *
   * This table shows some signals/slots connections that exits:
   * |          Sender          |              Signal         |         Receiver         |  Slot (or signal)  | Remarks |
   * | :----------------------: | :-------------------------: | :----------------------: | :----------------: | :------ |
   * | QAbstractItemModel       | modelReset()                | RowChangeEventDispatcher | onModelReset()     | |
   * | ItemSelectionModel       | currentRowChangeRequested() | AbstractController       | setCurrentRow()    | |
   * | NavigationActions        | toFirstTriggered()          | AbstractController       | toFirst()          | |
   * | RowChangeEventDispatcher | rowStateChanged()           | AbstractController       | rowStateChanged()   | |
   * | AbstractController       | rowStateChanged()           | NavigationActions        | setRowState()      | Used to update NavigationActions state |
   * | AbstractController       | currentRowToBeSet()         | ItemSelectionModel       | updateCurrentRow() | |
   * | AbstractController       | currentRowToBeSet()         | DataWidgetMapper         | setCurrentRow()    | |
   *
   *
   * Example of events chain when user selects a new row in a QTableView:
   *  - ItemSelectionModel::currentRowChangeRequested() -> AbstractController::setCurrentRow()
   *  - AbstractController (after calculated row and checks) calls RowChangeEventDispatcher::setCurrentRow()
   *  - RowChangeEventDispatcher::rowStateChanged() -> AbstractController::rowStateChanged()
   *  - AbstractController::currentRowToBeSet() -> ItemSelectionModel::updateCurrentRow()
   *  - AbstractController::rowStateChanged() -> NavigationActions::setRowState()
   *
   * Example of events when user clicks toNext button:
   *  - NavigationActions::toNextTriggered() -> AbstractController::toNext()
   *  - AbstractController (after calculated row and checks) calls RowChangeEventDispatcher::setCurrentRow()
   *  - RowChangeEventDispatcher::rowStateChanged() -> AbstractController::rowStateChanged()
   *  - AbstractController::currentRowToBeSet() -> ItemSelectionModel::updateCurrentRow()
   *  - AbstractController::rowStateChanged() -> NavigationActions::setRowState()
   *
   * Example of events chain when model was set to controller, or model was repopulated:
   *  - AbstractController calls RowChangeEventDispatcher::setModel() (case of setting a new model)
   *  - RowChangeEventDispatcher::rowStateChanged() -> AbstractController::rowStateChanged()
   *  - AbstractController::currentRowToBeSet() -> ItemSelectionModel::updateCurrentRow()
   *  - AbstractController::rowStateChanged() -> NavigationActions::setRowState()
   */
  class RowChangeEventDispatcher : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit RowChangeEventDispatcher(QObject *parent);

    // Copy disabled
    RowChangeEventDispatcher(const RowChangeEventDispatcher &) = delete;
    RowChangeEventDispatcher & operator=(const RowChangeEventDispatcher &) = delete;
    // Move disabled
    RowChangeEventDispatcher(RowChangeEventDispatcher &&) = delete;
    RowChangeEventDispatcher & operator=(RowChangeEventDispatcher &&) = delete;

    /*! \brief Get model
     *
     * Can return a nullptr if not model was set,
     *  or a nullptr was set as model.
     */
    QAbstractItemModel *model() const;

    /*! \brief Set current row
     *
     * This method is called from AbstractController.
     *  If a selection model was set,
     *  it will also change its current index.
     *
     * \pre row must be >= -1
     * \pre row must be < rowCount()
     */
    void setCurrentRow(int row);

    /*! \brief Get current row state
     */
    RowState currentRowState() const
    {
      return mRowState;
    }

    /*! \brief Get row count
     */
    int rowCount() const;

    /*! \brief Get current row
     */
    int currentRow() const;

    /*! \brief Insert a row at beginning
     *
     * The controller uses this method to insert a row into the model.
     *  When using this method, this dispatcher will allways tell
     *  the new inserted row as current row.
     *
     * \pre A model must be set
     */
    bool insertRowAtBeginning();

    /*! \brief Insert a row at end
     *
     * The controller uses this method to insert a row into the model.
     *  When using this method, this dispatcher will allways tell
     *  the new inserted row as current row.
     *
     * \pre A model must be set
     */
    bool insertRowAtEnd();

   signals:

    /*! \brief Emitted whenever row state changed
     *
     * This signal tells the controller that row count
     *  or current row has changed.
     */
    void rowStateChanged(Mdt::ItemEditor::RowState rs);

    /*! \brief Emitted whenever a component should set its current row
     *
     * This signal is emitted whenever current row changed,
     *  or when a new model was set, or after a model reset.
     *
     * \note When a nullptr model is set to this RowChangeEventDispatcher,
     *        currentRowToBeSet() will also be emitted.
     *        Receiver should also check if it acts on a valid model or not.
     */
    void currentRowToBeSet(int row);

    /*! \brief Emitted when rows have been inserted into the model
     *
     * This signal only tells the controller that some rows where
     *  inserted into the model.
     *  rowStateChanged() is also emitted to tell new row state.
     */
    void rowsInserted();

    /*! \brief Emitted when rows have been removed from the model
     *
     * This signal only tells the controller that some rows where
     *  removed from the model.
     *  rowStateChanged() is also emitted to tell new row state.
     */
    void rowsRemoved();

   public slots:

    /*! \brief Set model
     */
    void setModel(QAbstractItemModel *model);

    /*! \brief Stop watching on rows inserted and removed
     *
     * After calling this method, this RowChangeEventDispatcher
     *  will no longer update row state when rows have been
     *  inserted or removed to the model.
     *
     * A use case is when a QSortFilterProxyModel applies a filter,
     *  it removes and/or insert rows.
     *
     * By default, rows inserted and removed is watched.
     *
     * \sa startWatchingRowsInsertedRowsRemoved()
     *
     * \todo To be implemented and tested
     */
    void stopWatchingRowsInsertedRowsRemoved();

    /*! \brief Start watching on rows inserted and removed
     *
     * Will first update and notify row state,
     *  then watch about rows inserted and removed again.
     *
     * \sa stopWatchingRowsInsertedRowsRemoved()
     *
     * \todo To be implemented and tested
     */
    void startWatchingRowsInsertedRowsRemoved();

   private slots:

    /*! \brief Called from item model when it was repopulated
     */
    void onModelReset();

    /*! \brief Called from item model when rows have been inserted
     */
    void onRowsInserted(const QModelIndex & parent, int first, int last);

    /*! \brief Called from item model when rows have been removed
     */
    void onRowsRemoved(const QModelIndex & parent, int first, int last);

   private:

    /*! \brief Insert a row
     *
     * The controller uses this method to insert a row into the model.
     *  When using this method, this dispatcher will allways tell
     *  the new inserted row as current row.
     *
     * \pre A model must be set
     * \pre row must be in correct range ( 0 <= row <= model->rowCount() )
     */
    bool insertRowBefore(int row);

    /*! \brief Update current row
     */
    void updateCurrentRow(int row);

    RowState mRowState;
    QPointer<QAbstractItemModel> mModel;
    bool mInsertingFromController = false;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ROW_CHANGE_EVENT_DISPATCHER_H
