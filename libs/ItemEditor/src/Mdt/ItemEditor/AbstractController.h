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
#ifndef MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_H
#define MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_H

#include "RowState.h"
#include "ControllerState.h"
#include "ControllerStateMachine.h"

#include "ControllerStatePermission.h"

#include "ControllerRelationList.h"
#include "Mdt/ItemModel/ProxyModelContainer.h"
#include "Mdt/ItemModel/FilterProxyModel.h"
#include "Mdt/ItemModel/PrimaryKey.h"
#include "Mdt/ItemModel/ForeignKey.h"
#include "Mdt/Error.h"
#include <QObject>
#include <QPointer>
#include <QAbstractItemModel>
#include <initializer_list>

class QAbstractProxyModel;

namespace Mdt{ namespace ItemModel{

//   class RelationFilterProxyModel;
  class RelationFilterExpression;
  class PrimaryKeyProxyModel;
  class ForeignKeyProxyModel;

}} // namespace Mdt{ namespace ItemModel{

namespace Mdt{ namespace ItemEditor{

  class RowChangeEventDispatcher;

  /*! \brief Common base for controllers
   *
   * \par Edition
   *  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
   *
   * \par Primary key
   *  A primary key can be set with setPrimaryKey().
   *
   * \par Insertion
   *  To insert a row, use insert() slot, which will also update this controller state,
   *   and emit related signals.
   *   When a new row is inserted from model, for example using QAbstractItemModel::insertRow(),
   *   the controller will:
   *    - If rows have been inserted after currentRow(), nothing changes.
   *    - If rowCount() was 0, nothing changes.
   *    - If rows have been inserted before currentRow(),
   *       currentRow() will be adjusted to point to the same data as before insertion.
   *
   *      This allows applying relations filters to its child controllers automatically.
   *    NOTE: and when a filter is applyed ????????
   *
   * \par Filters
   *  To set a filter, use setFilter().
   *   Once the filter was applied, currentRow() will be updated:
   *    - For all cases, if rowCount() becomes 0, currentRow() will become invalid (-1).
   *    - If a primary key was set, and its values allready exists, currentRow() will be adjusted point to it.
   *    - If a primary key was set, but its values no longer exists, currentRow() will be adjusted point to first row.
   *    - If no primary key was set, currentRow() will be adjusted point to first row.
   *
   * \par Relation handling
   *  Common case of a relation is a child model having a foreign key referring to a primary key of a parent model:
   *   \code
   *    Controller clientController;
   *    clientController.setPrimaryKey({0});
   *    Controller addressController;
   *    addressController.setForeignKey({1});
   *    clientController.addChildController(addressController);
   *   \endcode
   *   Now, addressController will be filtered for values in column 1 that correspond to values in column 0 of clientController.
   *    A relation can handle more that 1 column, in whitch case they must all match (AND constraint).
   *   Using above code will offer a good support for edition.
   *   A relation can be defined with more complex conditions:
   *   \code
   *    Controller unitController;
   *    Controller linkController;
   *    ParentModelColumn unitId(0);
   *    ChildModelColumn startUnitId(0);
   *    ChildModelColumn endUnitId(1);
   *    unitController.addChildController(linkController, (startUnitId == unitId) || (endUnitId == unitId) );
   *   \endcode
   *   Support for edition can be offered, but will only take in account relation pairs that are equaly compared
   *    (ChildModelColumn == ParentModelColumn, ChildModelColumn <= ParentModelColumn, ChildModelColumn >= ParentModelColumn).
   *   When edition is supported in a relation, some event are handled:
   *    - When a new row is inserted into child controller's model,
   *       values of the parent controller are copied to child controller
   *       for each column pairs in the relation.
   *    - When a item, that is part of a relation key, is edited in parent controller's model,
   *       values in child controller's model are updated.
   */
  class AbstractController : public QObject
  {
   friend class AbstractControllerRelation;

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
      return mControllerState;
    }

    /*! \brief Get controller state machine
     */
    ControllerStateMachine controllerStateMachine() const
    {
      return mControllerStateMachine;
    }

    /*! \brief Get controller state permission
     */
    ControllerStatePermission controllerStatePermission() const
    {
      return mControllerStatePermission;
    }

    /*! \brief Check if it is allowed to change current row for current controller state
     */
    bool canChangeCurrentRow() const
    {
      return mControllerStatePermission.canChangeCurrentRow(mControllerState);
    }

    /*! \brief Check if it is allowed to insert for current controller state
     */
    bool canInsert() const
    {
      return mControllerStatePermission.canInsert(mControllerState);
    }

    /*! \brief Check if it is possible to submit for current controller state
     */
    bool canSubmit() const
    {
      return mControllerStatePermission.canSubmit(mControllerState);
    }

    /*! \brief Check if it is possible to revert for current controller state
     */
    bool canRevert() const
    {
      return mControllerStatePermission.canRevert(mControllerState);
    }

    /*! \brief Check if it is possible to remove for current controller state
     */
    bool canRemove() const
    {
      return mControllerStatePermission.canRemove(mControllerState);
    }

    /*! \brief Set insert location
     */
    void setInsertLocation(InsertLocation il);

    /*! \brief Get insert location
     */
    InsertLocation insertLocation() const
    {
      return mInsertLocation;
    }

    /*! \brief Set model
     *
     * \note Because model can be shared with several objects (f.ex. other views),
     *        the controller does not take ownership of it (it will not delete it).
     * \pre model must be a valid pointer
     */
//     void setModel(QAbstractItemModel *model);

    /*! \brief Get model
     *
     * Returns the model set with setModel(),
     *  or a nullptr if no one was set,
     *  or model was delete elsewhere in application.
     */
//     QAbstractItemModel *model() const
//     {
//       return mModelContainer.sourceModel();
//     }

    /*! \brief Get the source model
     *
     * Subclass that acts on a specific item model
     *  (derived from QAbstractItemModel)
     *  should have setModel() and model() methods.
     *  sourceModel() also returns the same as model(),
     *  but a pointer to QAbstractItemModel base class.
     *
     * Can also be a nullptr if no model was set.
     */
    QAbstractItemModel *sourceModel() const
    {
      return mModelContainer.sourceModel();
    }

    /*! \brief Get the model that is used by the view
     *
     * Can be the same as sourceModel() if no proxy model exists,
     *  a proxy model or a nullptr if no proxy model, and no model was set.
     */
    QAbstractItemModel *modelForView() const
    {
      if(mModelContainer.proxyModelCount() < 1){
        return mModelContainer.sourceModel();
      }
      return mModelContainer.lastProxyModel();
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

    /*! \brief Get row state
     *
     * \sa rowCount()
     * \sa currentRow()
     */
    RowState rowState() const;

    /*! \brief Prepend a proxy model
     *
     * If, after \a proxyModel was added, modelForView() has changed,
     *  setModelToView() is called and modelForViewChanged() is emitted.
     *
     * \pre \a proxyModel must be a valid pointer
     */
    void prependProxyModel(QAbstractProxyModel *proxyModel);

    /*! \brief Append a proxy model
     *
     * If, after \a proxyModel was added, modelForView() has changed,
     *  setModelToView() is called and modelForViewChanged() is emitted.
     *
     * \pre \a proxyModel must be a valid pointer
     */
    void appendProxyModel(QAbstractProxyModel *proxyModel);

    /*! \brief Remove a proxy model
     *
     * Remove the proxy model referenced by \a proxyModel.
     *
     * If, after \a proxyModel was removed, modelForView() has changed,
     *  setModelToView() is called and modelForViewChanged() is emitted.
     *
     * \note \a proxyModel is only removed from list of proxy models, but not deleted.
     */
    void removeProxyModel(QAbstractProxyModel *proxyModel);

    /*! \brief Set primary key support enabled
     *
     * Enabling support for primary key will insert a proxy model,
     *  disabling support will remove it.
     *
     * \sa setPrimaryKey()
     * \sa isPrimaryKeyEnabled()
     */
    void setPrimaryKeyEnabled(bool enable);

    /*! \brief Get primary key proxy model
     *
     * Return the primary key proxy model if support was enabled,
     *  otherwise a nullptr.
     *
     * \sa setPrimaryKeyEnabled()
     */
    Mdt::ItemModel::PrimaryKeyProxyModel *getPrimaryKeyProxyModel() const;

    /*! \brief Check if primary key support is enabled
     *
     * \sa setPrimaryKeyEnabled()
     */
    bool isPrimaryKeyEnabled() const;

    /*! \brief Set primary key
     *
     * If primary key support was not enabled,
     *  it will be enabled before setting \a pk.
     *
     * \pre \a pk must not be null
     */
    void setPrimaryKey(const Mdt::ItemModel::PrimaryKey & pk);

    /*! \brief Set primary key
     *
     * If primary key support was not enabled,
     *  it will be enabled before setting \a pk.
     *
     * \pre Each column in \a pk must be >= 0
     * \pre Each column in \a pk must be unique
     */
    void setPrimaryKey(std::initializer_list<int> pk);

    /*! \brief Get primary key
     */
    Mdt::ItemModel::PrimaryKey getPrimaryKey() const;

    /*! \brief Set primary key editable
     *
     * By default, primary key is editable
     */
    void setPrimaryKeyEditable(bool editable);

    /*! \brief Set primary key items enabled
     *
     * By default, primary key items are enabled.
     */
    void setPrimaryKeyItemsEnabled(bool enable);

    /*! \brief Set foreign key support enabled
     *
     * Enabling support for foreign key will insert a proxy model,
     *  disabling support will remove it.
     *
     * \sa setForeignKey()
     * \sa isForeignKeyEnabled()
     */
    void setForeignKeyEnabled(bool enable);

    /*! \brief Get foreign key proxy model
     *
     * Return the foreign key proxy model if support was enabled,
     *  otherwise a nullptr.
     *
     * \sa setForeignKeyEnabled()
     */
    Mdt::ItemModel::ForeignKeyProxyModel *getForeignKeyProxyModel() const;

    /*! \brief Check if foreign key support is enabled
     *
     * \sa setForeignKeyEnabled()
     */
    bool isForeignKeyEnabled() const;

    /*! \brief Set foreign key
     *
     * If foreign key support was not enabled,
     *  it will be enabled before setting \a fk.
     * 
     * \pre \a fk must not be null
     */
    void setForeignKey(const Mdt::ItemModel::ForeignKey & fk);

    /*! \brief Set foreign key
     *
     * If foreign key support was not enabled,
     *  it will be enabled before setting \a fk.
     *
     * \pre Each column in \a fk must be >= 0
     * \pre Each column in \a fk must be unique
     */
    void setForeignKey(std::initializer_list<int> fk);

    /*! \brief Get foreign key
     *
     * \note When source model changes,
     *        the foreign key will be cleared.
     */
    Mdt::ItemModel::ForeignKey getForeignKey() const;

    /*! \brief Set foreign key editable
     *
     * By default, foreign key is editable
     */
    void setForeignKeyEditable(bool editable);

    /*! \brief Set foreign key items enabled
     *
     * By default, foreign key items are enabled.
     */
    void setForeignKeyItemsEnabled(bool enable);

    /*! \brief Set filter enabled
     *
     * \note Enabling filter will insert a proxy model
     *        and disabling filter will remove it.
     *        To temporarily disable filtering,
     *        setDynamicFiltersEnabled() should be used.
     * \sa setDynamicFiltersEnabled()
     * \sa isFilterEnabled()
     */
    void setFilterEnabled(bool enable);

    /*! \brief Check if filter is enabled
     *
     * \sa setFilterEnabled()
     */
    bool isFilterEnabled() const;

    /*! \brief Set filter
     *
     * If filter is not enabled, it will be enabled before applying expression.
     *
     * \param expression Expression to apply as filter.
     * \pre \a expression must be a filter expression type.
     * \sa Mdt::ItemModel::FilterProxyModel
     * \sa setFilterEnabled()
     */
    template<typename Expr>
    void setFilter(const Expr & expression)
    {
      setFilterEnabled(true);
      filterModel()->setFilter(expression);
    }

    /*! \brief Set dynamic filters enabled/disabled
     *
     * Will act on dynamic filter property of filter
     *  and relation filter if enabled.
     *  If no filter and no relation filter
     *  is enabled, this method simply does nothing.
     *
     * \sa setFilter()
     * \sa setRelationFilter()
     */
    void setDynamicFiltersEnabled(bool enable);

    /*! \brief Add a child controller
     *
     * Once \a controller becomes child of this controller,
     *  it will be filtered regarding \a conditions .
     *
     * When state of this controller changes,
     *  state of all childs will also be updated.
     *
     * \note Because \a controller can be shared with several objects,
     *        this controller does not take ownership of it (it will not delete it).
     * \pre \a controller must be a valid pointer
     * \pre \a conditions must be a relation filter expression
     */
    void addChildController(AbstractController *controller, const Mdt::ItemModel::RelationFilterExpression & conditions);

    /*! \brief Add a child controller
     *
     * Once \a controller becomes child of this controller,
     *  it will be filtered regarding equality between
     *  the primary key of this controller
     *  and the foreign key of \a controller .
     *
     * When state of this controller changes,
     *  state of all childs will also be updated.
     *
     * \note Because \a controller can be shared with several objects,
     *        this controller does not take ownership of it (it will not delete it).
     * \pre \a controller must be a valid pointer
     * \pre This controller must have a non null primary key set
     * \pre \a controller must have a non null foreign key set
     * \pre Both primary of this controller and foreign key of \a controller
     *       must have the same count of columns, and maximum 4
     */
    void addChildController(AbstractController *controller);

    /*! \brief Set navigation controller
     *
     * Once this controller will become a child of \a controller ,
     *  current row will be ajusted regarding current row of \a controller.
     *
     * \note Because \a controller can be shared with several objects,
     *        this controller does not take ownership of it (it will not delete it).
     * \pre \a controller must be a valid pointer
     */
    void setNavigationController(AbstractController *controller);

    /*! \brief Get last error
     */
    Mdt::Error lastError() const
    {
      return mLastError;
    }

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

    /*! \brief Emitted each time source model changed
     *
     * \sa sourceModel()
     */
    void sourceModelChanged(QAbstractItemModel *model);

    /*! \brief Emitted each time model for the view changed
     *
     * If no model was set ( sourceModel() returns a nullptr ),
     *  and no proxy model is set, \a model is a nullptr.
     *
     * \sa modelForView()
     */
    void modelForViewChanged(QAbstractItemModel *model);

    /*! \brief Emitted each time row count or current row changed
     *
     * This is used for components that need row count
     *  and current row to update their state.
     *  A example of such object is NavigationActions .
     */
    void rowStateChanged(Mdt::ItemEditor::RowState rs);

    /*! \brief Emitted whenever current row changed
     *
     * This signal is typically used for widget mappers
     */
    void currentRowChanged(int row);

    /*! \brief Emitted whenever controller state changed
     */
    void controllerStateChanged(Mdt::ItemEditor::ControllerState newState);

//     /*! \brief Emitted whenever primary key changed
//      */
//     void primaryKeyChanged(const Mdt::ItemModel::PrimaryKey & pk);

   protected:

    /*! \brief Set controller state machine
     *
     * This controller will use \a stateMachine to know in which controller state it is
     *  and which action is permitted.
     *  Subclass that implements a concrete controller must pass a instance of ControllerStateMachine
     *  that has the expected implementation of stat chain and permissions.
     *
     * By default, this abstract controller has a null instance of ControllerStateMachine
     *  (a instance that has no implementation, and that gives no permission at all).
     *
     * This method should be called once, early in the initialization, like this:
     * \code
     * setControllerStateMachine( ControllerStateMachine::make<MyStateChain, MyPermission>() );
     * \endcode
     *
     * \pre \a stateMachine must have a implementation of state chain and permissions
     */
    void setControllerStateMachine(const ControllerStateMachine & stateMachine);

    /*! \brief Set controller state permission
     *
     * This controller will use \a permission to know which action is permitted in current controller state.
     *  Subclass that implements a concrete controller must pass a instance of ControllerStatePermission
     *  that has the expected implementation.
     *
     *
     * By default, this abstract controller has a null instance of ControllerStatePermission
     *  (a isntance that has no implementation, and that gives no permissions at all).
     *
     * This method should be called once, early in the initialization, like this:
     * \code
     * setControllerStatePermission( ControllerStatePermission::make<MyConcreteControllerStatePermission>() );
     * \endcode
     */
    void setControllerStatePermission(const ControllerStatePermission & permission);

    /*! \brief Register model
     *
     * This method should be called by a common base class
     *  that handles a more or less specific item model.
     *
     * This method should never be called 
     *  by a subclass implementing a concrete controller.
     *
     * \pre model must be a valid pointer
     * \sa setModelToView()
     */
    void registerModel(QAbstractItemModel *model);

//     /*! \brief Get model
//      *
//      * Returns the model set with registerModel(),
//      *  or a nullptr if no one was set,
//      *  or model was delete elsewhere in application.
//      */
//     [[deprecated]]
//     QAbstractItemModel *registeredModel() const
//     {
//       return mModelContainer.sourceModel();
//     }

    /*! \brief Set model to the view
     *
     * This method is called each time the model changes,
     *  because of a call of registerModel() ,
     *  or a proxy model was inserted/removed.
     *
     * When implementing a controller,
     *  subclass must implement this method.
     *  Once the model could be successfully be set to its view,
     *  modelSetToView() should be called.
     *
     * \sa modelSetToView()
     */
    virtual void setModelToView(QAbstractItemModel *model) = 0;

    /*! \brief Add model model to internal event handling
     *
     * Subclass that implements a controller
     *  should only call this method once it could set the model to the view.
     *
     * \sa setModelToView()
     */
    void modelSetToView();

    /*! \brief Submit data to model
     *
     * Subclass should request the view it handles
     *  to submit data to the model.
     */
    virtual bool submitDataToModel() = 0;

    /*! \brief Revert data from model
     */
    virtual void revertDataFromModel() = 0;

    /*! \brief Primary key changed event
     *
     * If subclass has some action to perform
     *  when primary key changed,
     *  it can implement this method.
     *
     * This default implementation does nothing.
     */
    virtual void primaryKeyChangedEvent(const Mdt::ItemModel::PrimaryKey & oldPrimaryKey, const Mdt::ItemModel::PrimaryKey & newPrimaryKey);

    /*! \brief Foreign key changed event
     *
     * If subclass has some action to perform
     *  when foreign key changed,
     *  it can implement this method.
     *
     * This default implementation does nothing.
     */
    virtual void foreignKeyChangedEvent(const Mdt::ItemModel::ForeignKey & oldForeignKey, const Mdt::ItemModel::ForeignKey & newForeignKey);

    /*! \brief Set last error
     */
    void setLastError(const Mdt::Error & error)
    {
      mLastError = error;
    }

   protected slots:

    /*! \brief Actions to perform once editing started
     *
     * Subclass implementing a concrete controller
     *  should call this method whenever the user starts editing.
     */
    void onDataEditionStarted();

    /*! \brief Actions to perform once editing ended
     *
     * Subclass implementing a concrete controller
     *  should call this method whenever edition was done.
     */
    void onDataEditionDone();

   private slots:

    /*! \brief Actions to perform once rows have been inserted
     *
     * This slot is only called by RowChangeEventDispatcher,
     *  to update this controller state.
     */
//     void onRowsInserted();

    /*! \brief Actions to perform once rows have been removed
     *
     * This slot is only called by RowChangeEventDispatcher,
     *  to update this controller state.
     */
    void onRowsRemoved();

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

//     /*! \brief Actions to perform when primary key's source model changed
//      *
//      * Main goal is to call primaryKeyChangedEvent()
//      */
//     void onPrimaryKeySourceModelChanged();

   private:

    /*! \brief Set controller state
     */
    void setControllerState(ControllerState state);

    /*! \brief Get filter proxy model
     *
     * \pre Filter must be enabled
     */
    Mdt::ItemModel::FilterProxyModel *filterModel() const;

    /*! \brief Remove and delete the first proxy model of type T
     *
     * If, after the first proxy model of type T was removed (and deleted), modelForView() has changed,
     *  setModelToView() is called and modelForViewChanged() is emitted.
     */
    template<typename T>
    void deleteFirstProxyModelOfType()
    {
      auto *oldModelForView = modelForView();
      mModelContainer.deleteFirstProxyModelOfType<T>();
      updateModelForViewIfChanged(oldModelForView);
    }

    /*! \brief Update model for view if changed
     *
     * If \a oldModelForView is different than modelForView(),
     *  setModelToView() is called and modelForViewChanged() is emitted.
     */
    void updateModelForViewIfChanged(QAbstractItemModel *oldModelForView);

    ControllerState mControllerState = ControllerState::Visualizing;
    ControllerStateMachine mControllerStateMachine;
    
    ControllerStatePermission mControllerStatePermission;
    RowChangeEventDispatcher *mRowChangeEventDispatcher;
    InsertLocation mInsertLocation;
//     Mdt::ItemModel::PrimaryKey mPrimaryKey;
//     Mdt::ItemModel::ForeignKey mForeignKey;
    Mdt::ItemModel::ProxyModelContainer mModelContainer;
//     ControllerRelationList<AbstractController, ControllerRelation> mRelationList;
    ControllerRelationList mRelationList;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_H
