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
#include "ControllerRelationImpl.h"
#include "ControllerRelationList.h"
#include "Mdt/ItemModel/ProxyModelContainer.h"
#include "Mdt/ItemModel/FilterProxyModel.h"
#include "Mdt/Error.h"
#include <QObject>
#include <QPointer>
#include <QAbstractItemModel>

namespace Mdt{ namespace ItemModel{

  class RelationFilterProxyModel;
  class RelationFilterExpression;

}} // namespace Mdt{ namespace ItemModel{

namespace Mdt{ namespace ItemEditor{

  class RowChangeEventDispatcher;

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
     * Can be the same as model() if no proxy model exists,
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

    /*! \brief Set filter enabled
     *
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

    /*! \brief Set relation filter enabled
     *
     * \sa isRelationFilterEnabled()
     */
    void setRelationFilterEnabled(bool enable);

    /*! \brief Check if relation filter is enabled
     *
     * \sa setRelationFilterEnabled()
     */
    bool isRelationFilterEnabled() const;

    /*! \brief Set relation filter's parent model
     *
     * If relation filter is not enabled, it will be enabled before setting its parent model.
     *
     * \pre \a model must be a valid pointer.
     */
    void setRelationFilterParentModel(QAbstractItemModel *model);

    /*! \brief Set relation filter
     *
     * If relation filter is not enabled, it will be enabled before applying expression.
     *
     * \param expression Expression to apply as filter.
     * \pre \a expression must be a relation filter expression.
     * \sa Mdt::ItemModel::RelationFilterProxyModel
     * \sa setRelationFilterEnabled()
     * \sa setRelationFilterParentModel()
     */
    void setRelationFilter(const Mdt::ItemModel::RelationFilterExpression & expression);

    /*! \brief Get relation filter proxy model
     *
     * \pre Relation filter must be enabled
     */
    Mdt::ItemModel::RelationFilterProxyModel *relationFilterModel() const;

    /*! \brief Add a child controller
     *
     * Once \a controller becomes child of this controller,
     *  it will be filtered regarding \a conditions .
     *
     * When state of this controller changes,
     *  state all childs will also be updated.
     *
     * \note Because \a controller can be shared with several objects,
     *        this controller does not take ownership of it (it will not delete it).
     * \pre \a controller must be a valid pointer
     * \pre \a expression must be a relation filter expression
     */
    void addChildController(AbstractController *controller, const Mdt::ItemModel::RelationFilterExpression & conditions);

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

   protected:

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

    /*! \brief Get model
     *
     * Returns the model set with registerModel(),
     *  or a nullptr if no one was set,
     *  or model was delete elsewhere in application.
     */
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

    /*! \brief Set last error
     */
    void setLastError(const Mdt::Error & error)
    {
      mLastError = error;
    }

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

    /*! \brief Get filter proxy model
     *
     * \pre Filter must be enabled
     */
    Mdt::ItemModel::FilterProxyModel *filterModel() const;

    ControllerState pvControllerState = ControllerState::Visualizing;
    RowChangeEventDispatcher *pvRowChangeEventDispatcher;
    InsertLocation pvInsertLocation;
    Mdt::ItemModel::ProxyModelContainer mModelContainer;
    ControllerRelationList<AbstractController, ControllerRelationImpl> mRelationList;
    Mdt::Error mLastError;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_H
