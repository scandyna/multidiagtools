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
#ifndef MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_H
#define MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_H

#include "RowState.h"
#include "ControllerState.h"
#include <QObject>
#include <QPointer>
#include <QAbstractItemModel>

namespace Mdt{ namespace ItemEditor{

  class RowChangeEventDispatcher;
  class ItemSelectionModel;
  class EventCatchItemDelegate;

  /*! \brief Common base for controllers
   */
  class AbstractController : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Insert location
     */
    enum InsertLocation
    {
      InsertAtBeginning,  /*!< New row will be inserted at the beginning, and will become row 0. This is the default. */
      InsertAtEnd         /*!< New row will be inserted at end.
                                Note that the controller must fetch all data before the insertion, which may be slow. */
    };

    /*! \brief Constructor
     */
    explicit AbstractController(QObject* parent = nullptr);

    // Copy disabled
    AbstractController(const AbstractController &) = delete;
    AbstractController & operator=(const AbstractController &) = delete;
    // Move disabled
    AbstractController(AbstractController &&) = delete;
    AbstractController & operator=(AbstractController &&) = delete;

    /*! \brief Get controller state
     */
    ControllerState controllerState() const
    {
      return pvControllerState;
    }

    /*! \brief Set insert location
     */
    void setInsertLocation(InsertLocation il);

    /*! \brief Get insert location
     */
    InsertLocation insertLocation() const
    {
      return pvInsertLocation;
    }

    /*! \brief Set model
     *
     * \note Because model can be shared with several objects (f.ex. other views),
     *        the controller does not take ownership of it (it will not delete it).
     * \pre model must be a valid pointer
     */
    virtual void setModel(QAbstractItemModel *model);

    /*! \brief Get model
     *
     * Returns the model set with setModel(),
     *  or a nullptr if no one was set,
     *  or model was delete elsewhere in application.
     */
    QAbstractItemModel *model() const
    {
      return pvModel;
    }

    /*! \brief Get row count
     *
     * If not model was set, 0 is returned.
     *
     * \note If this controller acts on a model
     *        that handles cache, this method
     *        will report row count in cache.
     *       See QAbstractItemModel documentation,
     *        at least canFetchMore() and fetchMore() ,
     *        for more details about cache.
     *
     * \todo Add remark on fetchAll() when..
     */
    int rowCount() const;

    /*! \brief Get current row
     *
     * If model was not set,
     *  or is empty, or for some other reasons
     *  (for example, a view has currently no current index),
     *  -1 is returned.
     */
    int currentRow() const;

   public slots:

    /*! \brief Set current row
     *
     * \pre row must be >= -1
     */
    bool setCurrentRow(int row);

    /*! \brief Go to first row
     */
    void toFirst();

    /*! \brief Go to previous row
     */
    void toPrevious();

    /*! \brief Go to next row
     */
    void toNext();

    /*! \brief Go to last row
     */
    void toLast();

    /*! \brief Submit data to model
     */
    bool submit();

    /*! \brief Revert data from model
     */
    void revert();

    /*! \brief Insert a row
     *
     * This slot is affected by InsertLocation
     */
    bool insert();

    /*! \brief Remove current row
     */
    bool remove();

   signals:

    /*! \brief Emitted each time row count or current row changed
     *
     * This is used for components that need row count
     *  and current row to update their state.
     *  A example of such object is NavigationActions .
     */
    void rowStateChanged(Mdt::ItemEditor::RowState rs);

    /*! \brief Emitted whenever controller state changed
     */
    void controllerStateChanged(Mdt::ItemEditor::ControllerState newState);

    /*! \brief Emitted whenever current row changed
     *
     * This signal is typically used for widget mappers
     */
    void currentRowChanged(int row);

    /*! \brief Emitted each time a model was set
     */
    void modelChanged(QAbstractItemModel *model);

   protected:

    /*! \brief Submit data to model
     *
     * Subclass should request the view it handles
     *  to submit data to the model.
     */
    virtual bool submitDataToModel() = 0;

    /*! \brief Revert data from model
     */
    virtual void revertDataFromModel() = 0;

    /*! \brief Reference model
     *
     * Will only hold a reference to model and emit modelChanged().
     *
     * \sa registerItemModel()
     */
    void referenceItemModel(QAbstractItemModel *model);

    /*! \brief Register item model
     *
     * The model set with referenceModel() will be added to internal event handling.
     *
     * \sa referenceItemModel()
     */
    void registerItemModel();

    /*! \brief Register selection model
     *
     * Will set selectionModel to the internal event handling.
     *
     * If a editor uses a view that handle selection with a QItemSelectionModel (like QTableView),
     *  replace its selection model with a ItemSelectionModel,
     *  and register it with this method.
     *  This way, the controller can block row change when needed.
     */
    void registerSelectionModel(ItemSelectionModel *selectionModel);

    /*! \brief Register item delegate
     *
     * Once delegate is registered, this controller is able to detect when user beginns editing,
     *  and can update components in a coherent manner.
     */
    void registerItemDelegate(EventCatchItemDelegate *delegate);

   protected slots:

    /*! \brief Actions to perform once editing started
     */
    void onDataEditionStarted();

    /*! \brief Actions to perform once editing ended
     */
    void onDataEditionDone();

   private slots:

    /*! \brief Update row state
     *
     * This slot is only called by RowChangeEventDispatcher,
     *  to tell that a model was changed or populated,
     *  a selection model was changed,
     *  or current row changed by selection model.
     *
     * It will also emit rowStateChanged() and currentRowChanged().
     */
    void updateRowState(Mdt::ItemEditor::RowState rs);

   private:

    /*! \brief Set controller state
     */
    void setControllerState(ControllerState state);

    ControllerState pvControllerState = ControllerState::Visualizing;
    RowChangeEventDispatcher *pvRowChangeEventDispatcher;
    QPointer<QAbstractItemModel> pvModel;
    QPointer<ItemSelectionModel>  pvSelectionModel;
    QPointer<EventCatchItemDelegate> pvDelegate;
    InsertLocation pvInsertLocation;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_H
