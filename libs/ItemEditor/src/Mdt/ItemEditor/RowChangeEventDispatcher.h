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

  class AbstractController;

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
   * <table class="srcdoc_td_center">
   *  <tr><th>Sender</th><th>Signal</th><th>Receiver</th><th>Slot (or signal)</th><th>Remarks</th></tr>
   *  <tr><td>QAbstractItemModel</td><td>modelReset()</td><td>RowChangeEventDispatcher</td><td>onModelReset()</td><td></td></tr>
   *  <tr><td>ItemSelectionModel</td><td>currentRowChangeRequested()</td><td>AbstractController</td><td>setCurrentRow()</td><td></td></tr>
   *  <tr><td>NavigationActions</td><td>toFirstTriggered()</td><td>AbstractController</td><td>toFirst()</td><td></td></tr>
   *  <tr><td>RowChangeEventDispatcher</td><td>rowStateUpdated()</td><td>AbstractController</td><td>updateRowState()</td><td></td></tr>
   *  <tr><td>AbstractController</td><td>rowStateChanged()</td><td>NavigationActions</td><td>setRowState()</td><td>Used to update NavigationActions state</td></tr>
   *  <tr><td>AbstractController</td><td>currentRowChanged()</td><td>ItemSelectionModel</td><td>updateCurrentRow()</td><td></td></tr>
   * </table>
   *
   *
   * Example of events chain when user selects a new row in a QTableView:
   *  - ItemSelectionModel::currentRowChangeRequested() -> AbstractController::setCurrentRow()
   *  - AbstractController (after calculated row and checks) calls RowChangeEventDispatcher::setCurrentRow()
   *  - RowChangeEventDispatcher::rowStateUpdated() -> AbstractController::updateRowState()
   *  - AbstractController::currentRowChanged() -> ItemSelectionModel::updateCurrentRow()
   *  - AbstractController::rowStateChanged() -> NavigationActions::setRowState()
   *
   * Example of events when user clicks toNext button:
   *  - NavigationActions::toNextTriggered() -> AbstractController::toNext()
   *  - AbstractController (after calculated row and checks) calls RowChangeEventDispatcher::setCurrentRow()
   *  - RowChangeEventDispatcher::rowStateUpdated() -> AbstractController::updateRowState()
   *  - AbstractController::currentRowChanged() -> ItemSelectionModel::updateCurrentRow()
   *  - AbstractController::rowStateChanged() -> NavigationActions::setRowState()
   *
   * Example of events chain when model was set to controller, or model was repopulated:
   *  - AbstractController calls RowChangeEventDispatcher::setModel()
   *  - RowChangeEventDispatcher::rowStateUpdated() -> AbstractController::updateRowState()
   *  - AbstractController::currentRowChanged() -> ItemSelectionModel::updateCurrentRow()
   *  - AbstractController::rowStateChanged() -> NavigationActions::setRowState()
   */
  class RowChangeEventDispatcher : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit RowChangeEventDispatcher(AbstractController *controller);

    // Copy disabled
    RowChangeEventDispatcher(const RowChangeEventDispatcher &) = delete;
    RowChangeEventDispatcher & operator=(const RowChangeEventDispatcher &) = delete;
    // Move disabled
    RowChangeEventDispatcher(RowChangeEventDispatcher &&) = delete;
    RowChangeEventDispatcher & operator=(RowChangeEventDispatcher &&) = delete;

    /*! \brief Get model
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

    /*! \brief Emitted when row state was updated
     *
     * This signal tells the controller that row count
     *  or current row was updated by a event like
     *  current changed, model was set or repopulated.
     */
    void rowStateUpdated(Mdt::ItemEditor::RowState rs);

    /*! \brief Emitted when rows have been inserted into the model
     *
     * This signal only tells the controller that some rows where
     *  inserted into the model.
     *  rowStateUpdated() is also emitted to tell new row state.
     */
    void rowsInserted();

    /*! \brief Emitted when rows have been removed from the model
     *
     * This signal only tells the controller that some rows where
     *  removed from the model.
     *  rowStateUpdated() is also emitted to tell new row state.
     */
    void rowsRemoved();

   public slots:

    /*! \brief Set model
     */
    void setModel(QAbstractItemModel *model);

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
    QPointer<AbstractController> mController;
    bool mInsertingFromController = false;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ROW_CHANGE_EVENT_DISPATCHER_H
