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
#include "ItemViewPrivateContainer.h"

namespace Mdt{ namespace ItemEditor{

ItemViewPrivateContainer::ItemViewPrivateContainer()
 : mItemDelegate( new EventCatchItemDelegate(nullptr) ),
   mSelectionModel( new ItemSelectionModel(nullptr) )
{
}

ItemViewPrivateContainer::~ItemViewPrivateContainer()
{
  if( (!mView.isNull()) && (itemDelegate() != nullptr) ){
    mView->setItemDelegate(itemDelegate());
  }
}

void ItemViewPrivateContainer::setModel(QAbstractItemModel*const model)
{
  Q_ASSERT(model != nullptr);

  mSelectionModel->setModel(model);
  setModelToView();
  /*
   * As documented in QAbstractItemView,
   * when setting a model to a view,
   * a new selection model is created.
   * We also have to set our selection model again.
   */
  setSelectionModelToView();
}

void ItemViewPrivateContainer::setView(QAbstractItemView*const view)
{
  Q_ASSERT(view != nullptr);

  mView = view;
  setModelToView();
  setSelectionModelToView();
  setItemDelegateToView();
}

void ItemViewPrivateContainer::setItemDelegateToView()
{
  Q_ASSERT(mItemDelegate);

  if(mView.isNull()){
    return;
  }
  auto *delegate = mView->itemDelegate();
  if(delegate == mItemDelegate.get()){
    return;
  }
  // Insert our proxy delegate
  if(delegate != nullptr){
    mItemDelegate->setItemDelegate(delegate);
  }
  mView->setItemDelegate(mItemDelegate.get());
}

void ItemViewPrivateContainer::setModelToView()
{
  if( (model() == nullptr) || mView.isNull()){
    return;
  }
  if(mView->model() == model()){
    return;
  }
  mView->setModel(model());
}

void ItemViewPrivateContainer::setSelectionModelToView()
{
  if( (model() == nullptr) || mView.isNull()){
    return;
  }
  /*
   * As documented in QAbstractItemView ,
   * when setting the model, selection model is not deleted.
   * We assume that selection model is not shared with other views,
   * so we delete it.
   */
  auto *oldSelectionModel = mView->selectionModel();
  mView->setSelectionModel(mSelectionModel.get());
  if(oldSelectionModel != nullptr){
    delete oldSelectionModel;
  }
  Q_ASSERT(mSelectionModel->model() == model());
}


}} // namespace Mdt{ namespace ItemEditor{