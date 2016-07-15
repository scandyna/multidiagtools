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
#ifndef MDT_ITEM_EDITOR_ROW_CHANGE_EVENT_DISPATCHER_H
#define MDT_ITEM_EDITOR_ROW_CHANGE_EVENT_DISPATCHER_H

#include "RowState.h"
#include <QObject>
#include <QPointer>
#include <QModelIndex>
#include <QItemSelectionModel>

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
   * <table class="srcdoc_td_center">
   *  <tr><th>Sender</th><th>Signal</th><th>Receiver</th><th>Slot (or signal)</th><th>Remarks</th></tr>
   *  <tr><td>QAbstractItemModel</td><td>modelReset()</td><td>RowChangeEventDispatcher</td><td>onModelReset()</td><td></td></tr>
   *  <tr><td>ItemSelectionModel</td><td>currentChanged()</td><td>RowChangeEventDispatcher</td><td>updateCurrentIndex()</td><td></td></tr>
   *  <tr><td>NavigationActions</td><td>toFirstTriggered()</td><td>AbstractController</td><td>toFirst()</td><td></td></tr>
   *  <tr><td>RowChangeEventDispatcher</td><td>currentIndexChanged()</td><td>ItemSelectionModel</td><td>setCurrentIndex()</td><td></td></tr>
   *  <tr><td>RowChangeEventDispatcher</td><td>rowStateUpdated()</td><td>AbstractController</td><td>updateRowState()</td><td></td></tr>
   *  <tr><td>AbstractController</td><td>rowStateChanged()</td><td>NavigationActions</td><td>setRowState()</td><td>Used to update NavigationActions state</td></tr>
   * </table>
   *
   *
   * Example of events chain when user selects a new row in a QTableView:
   *  - ItemSelectionModel::currentChanged() -> RowChangeEventDispatcher::updateCurrentIndex()
   *  - RowChangeEventDispatcher::rowStateUpdated() -> AbstractController::updateRowState()
   *  - AbstractController::rowStateChanged() -> NavigationActions::setRowState()
   *
   * Example of events when user clicks toNext button:
   *  - NavigationActions::toNextTriggered() -> AbstractController::toNext()
   *  - AbstractController (after calculated row and checks) calls RowChangeEventDispatcher::setCurrentRow()
   *  - RowChangeEventDispatcher::currentIndexChanged() -> ItemSelectionModel::setCurrentIndex()
   *  - ItemSelectionModel::currentChanged() -> RowChangeEventDispatcher::updateCurrentIndex()
   *  - RowChangeEventDispatcher::rowStateUpdated() -> AbstractController::updateRowState()
   *  - AbstractController::rowStateChanged() -> NavigationActions::setRowState()
   *
   * Example of events chain when model was set to controller, or model was repopulated:
   *  - AbstractController calls RowChangeEventDispatcher::setModel()
   *  - RowChangeEventDispatcher::rowStateUpdated() -> AbstractController::updateRowState()
   *  - AbstractController::rowStateChanged() -> NavigationActions::setRowState()
   */
  class RowChangeEventDispatcher : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit RowChangeEventDispatcher(QObject* parent = nullptr);

    // Copy disabled
    RowChangeEventDispatcher(const RowChangeEventDispatcher &) = delete;
    RowChangeEventDispatcher & operator=(const RowChangeEventDispatcher &) = delete;
    // Move disabled
    RowChangeEventDispatcher(RowChangeEventDispatcher &&) = delete;
    RowChangeEventDispatcher & operator=(RowChangeEventDispatcher &&) = delete;

    /*! \brief Set selection model
     */
    void setSelectionModel(QItemSelectionModel *model);

    /*! \brief Get selection model
     */
    QItemSelectionModel *selectionModel() const;

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

    /*! \brief Get row count
     */
    int rowCount() const;

    /*! \brief Get current row
     */
    int currentRow() const;

   signals:

    /*! \brief Emitted when row state was updated
     *
     * This signal tells the controller that row count
     *  or current row was updated by a event like
     *  current changed, model was set or repopulated.
     */
    void rowStateUpdated(Mdt::ItemEditor::RowState rs);

    /*! \brief Emitted when current row has changed
     *
     * This signal updates selection model, if it was set.
     */
//     void currentIndexChanged(const QModelIndex & index, QItemSelectionModel::SelectionFlags command);

   public slots:

    /*! \brief Set model
     */
    void setModel(QAbstractItemModel *model);

   private slots:

    /*! \brief Called from item model when it was repopulated
     */
    void onModelReset();

    /*! \brief Called from selection model
     */
    void updateCurrentIndex(const QModelIndex & current, const QModelIndex & previous);

    /*! \brief Set row state
     *
     * This slot is only called by RowChangeEventMapper,
     *  to tell that a model was changed or populated,
     *  a selection model was changed,
     *  or current row changed by selection model.
     */
//     void setRowState(Mdt::ItemEditor::RowState rs, Mdt::ItemEditor::RowChangeEventSource source);

   signals:

    /*! \brief Emitted each time row count or current row changed
     *
     * This is used for components that need row count
     *  and current row to update their state.
     *  A example of such object is NavigationActions .
     */
//     void rowStateChanged(Mdt::ItemEditor::RowState rs);

    /*! \brief Emitted when current row changed
     */
//     void currentRowChanged(int row);

    /*! \brief Emitted when current row changed
     */
//     void currentRowChangedForController(int row/**, Mdt::ItemEditor::RowChangeEventSource source*/);

    /*! \brief Emitted when model was set, or repopulated
     */
//     void modelReset();

   private:

    /*! \brief Update current row
     */
    void updateCurrentRow(int row);

    /*! \brief Set current index of selection model
     */
    void setSelectionModelCurrentIndex(int row);

//     RowState pvPreviousRowState;
    RowState pvRowState;
    QPointer<QAbstractItemModel> pvModel;
    QPointer<QItemSelectionModel> pvSelectionModel;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ROW_CHANGE_EVENT_DISPATCHER_H
