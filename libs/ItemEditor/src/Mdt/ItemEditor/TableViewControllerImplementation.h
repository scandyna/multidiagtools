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
#ifndef MDT_ITEM_EDITOR_TABLE_VIEW_CONTROLLER_IMPLEMENTATION_H
#define MDT_ITEM_EDITOR_TABLE_VIEW_CONTROLLER_IMPLEMENTATION_H

#include "ItemViewPrivateContainer.h"
#include "Mdt/ItemModel/PrimaryKey.h"
#include "Mdt/ItemModel/ForeignKey.h"

class QAbstractItemModel;
class QTableView;

namespace Mdt{ namespace ItemEditor{

  class AbstractController;
  class EventCatchItemDelegate;

  /*! \brief TableViewControllerImplementation is a helper class to implement TableViewController
   */
  class TableViewControllerImplementation
  {
   public:

//     /*! \brief Construct a implementation for \a controller
//      *
//      * \code
//      * TableViewController::TableViewController()
//      *  : mImpl( new TableViewControllerImplementation(this) )
//      * {
//      * }
//      * \endcode
//      *
//      * \pre \a controller must be a valid pointer for the entiere lifetime of this implementation object.
//      */
//     TableViewControllerImplementation(AbstractController *controller);

    TableViewControllerImplementation() = default;
    ~TableViewControllerImplementation() = default;
    TableViewControllerImplementation(const TableViewControllerImplementation &) = delete;
    TableViewControllerImplementation & operator=(const TableViewControllerImplementation &) = delete;
    TableViewControllerImplementation(TableViewControllerImplementation &&) = delete;
    TableViewControllerImplementation & operator=(TableViewControllerImplementation &&) = delete;

    /*! \brief Set model
     *
     * This class does not take ownership
     *  of \a model (it will not delete it).
     *
     * \pre \a model must be a valid pointer
     */
    void setModel(QAbstractItemModel * const model);

    /*! \brief Get model
     *
     * Returns a nullptr if model was not set
     *
     * \sa setModel()
     */
    QAbstractItemModel *model() const
    {
      return mContainer.model();
    }

    /*! \brief Attach view to this controller
     *
     * Once view is attached, this controller
     *  take control of it.
     *
     * \note A controller will only control 1 view.
     *       Also, controller does not take ownership
     *       of the view (it will not delete it).
     * \pre view must be a valid pointer.
     */
    void setView(QTableView * const view);

    /*! \brief Get view attached to this controller
     *
     * Will also return a nullptr if no view was set
     */
    QTableView *view() const;

    /*! \brief Get proxy item delegate
     */
    EventCatchItemDelegate *proxyItemDelegate() const
    {
      return mContainer.proxyItemDelegate();
    }

    /*! \brief Make signal/slots connections between \a controller and view's components
     *
     * This method should be called each time model or view has changed.
     *
     * Returns true if connections have been made,
     *  which means that AbstractController can be notified that the model was set to view.
     *
     * Example of a implementation of setModelToView():
     * \code
     * void setModelToView(QAbstractItemModel *model) override
     * {
     *   mImpl->setModel(model);
     *   if( connectToController(this) ){
     *     modelSetToView();
     *   }
     * }
     * \endcode
     *
     * Example of implementation of setView():
     * \code
     * void setView(QTableView *view)
     * {
     *   mImpl->setView(view);
     *   if( connectToController(this) ){
     *     modelSetToView();
     *   }
     * }
     * \endcode
     *
     * \pre \a controller must be a valid pointer
     * \pre This implement object's model must be the same than \a controller 's model for view
     */
    bool connectToController(AbstractController *controller);

    /*! \brief Submit data to model
     */
    bool setDataToModel();

    /*! \brief Revert data from model
     */
    void revertDataFromModel();

    /*! \brief Set primary key hidden
     *
     * If \a hide is true, columns that are part of primary key will be hidden.
     */
    void setPrimaryKeyHidden(const ItemModel::PrimaryKey & pk, bool hide);

    /*! \brief Set foreign key hidden
     *
     * If \a hide is true, columns that are part of foreign key will be hidden.
     */
    void setForeignKeyHidden(const ItemModel::ForeignKey & fk, bool hide);

    /*! \brief Update columns visibility regarding old and new primary keys
     */
    void primaryKeyChangedEvent(const ItemModel::PrimaryKey & oldPrimaryKey, const ItemModel::PrimaryKey & newPrimaryKey);

    /*! \brief Update columns visibility regarding old and new foreign keys
     */
    void foreignKeyChangedEvent(const ItemModel::ForeignKey & oldForeignKey, const ItemModel::ForeignKey & newForeignKey);

   private:

    void updatePrimaryKeyColumnsVisibility(const ItemModel::PrimaryKey & pk);
    void updateForeignKeyColumnsVisibility(const ItemModel::ForeignKey & fk);

    ItemViewPrivateContainer mContainer;
    bool mPrimaryKeyColumnsHidden = false;
    bool mForeignKeyColumnsHidden = false;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_TABLE_VIEW_CONTROLLER_IMPLEMENTATION_H
