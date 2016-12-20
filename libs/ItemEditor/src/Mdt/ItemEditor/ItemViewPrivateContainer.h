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
#ifndef MDT_ITEM_EDITOR_ITEM_VIEW_PRIVATE_CONTAINER_H
#define MDT_ITEM_EDITOR_ITEM_VIEW_PRIVATE_CONTAINER_H

#include "ItemSelectionModel.h"
#include "EventCatchItemDelegate.h"
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QPointer>
#include <memory>

namespace Mdt{ namespace ItemEditor{

  /*! \brief Helper class to store a view, delegate and selection model
   *
   * Some of item view based controllers have to react at signals
   *  emitted from model, selection model and delegate.
   *  To make shure that all works properly,
   *  the order of signal/slot connections is important.
   *
   * ItemViewPrivateContainer accepts setting model, delegate, view and selection model
   *  at any order, and will do these connection when appropriate.
   */
  class ItemViewPrivateContainer
  {
   public:

    /*! \brief Constructor
     */
    ItemViewPrivateContainer();

    /*! \brief Destructor
     *
     * If a view was set, its original item delegate is restored
     */
    ~ItemViewPrivateContainer();

    // Disable copy
    ItemViewPrivateContainer(const ItemViewPrivateContainer &) = delete;
    ItemViewPrivateContainer & operator=(const ItemViewPrivateContainer &) = delete;
    // Disable move
    ItemViewPrivateContainer & operator=(ItemViewPrivateContainer &&) = delete;
    ItemViewPrivateContainer(ItemViewPrivateContainer &&) = delete;

    /*! \brief Set model
     *
     * This container does not take ownership
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
      return mSelectionModel->model();
    }

    /*! \brief Set view
     *
     * This container does not take ownership
     *  of \a view (it will not delete it).
     *
     * A proxy delegate will be inserted between \a view and its item delegate.
     *  When this container is deleted, the proxy delegate is removed and the original one is restored.
     *
     * Selection model will be replaced in \a view.
     *  When this container is deleted, \a view will not have any selection model
     *  (QAbstractItemView handles this, but some methods will no longer work as expected).
     *
     * \pre \a view must be a valid pointer
     */
    void setView(QAbstractItemView * const view);

    /*! \brief Get view
     *
     * Returns a nullptr if view was not set
     *
     * \sa setView()
     */
    QAbstractItemView *view() const
    {
      return mView;
    }

    /*! \brief Get item delegate
     */
    QAbstractItemDelegate *itemDelegate() const
    {
      return mItemDelegate->itemDelegate();
    }

    /*! \brief Get proxy item delegate
     */
    EventCatchItemDelegate *proxyItemDelegate() const
    {
      return mItemDelegate.get();
    }

    /*! \brief Get selection model
     */
    ItemSelectionModel *selectionModel() const
    {
      return mSelectionModel.get();
    }

   private:

    void setItemDelegateToView();
    void setModelToView();
    void setSelectionModelToView();

    QPointer<QAbstractItemView> mView;
    std::unique_ptr<EventCatchItemDelegate> mItemDelegate;
    std::unique_ptr<ItemSelectionModel> mSelectionModel;
  };

}} // namespace Mdt{ namespace ItemEditor{

#endif // #ifndef MDT_ITEM_EDITOR_ITEM_VIEW_PRIVATE_CONTAINER_H
