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
#include "ProxyModelContainer.h"
#include <QAbstractProxyModel>
#include <QAbstractItemModel>

namespace Mdt{ namespace ItemModel{

void ProxyModelContainer::setSourceModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

  mSourceModel = model;
  if(!mList.empty()){
    firstProxyModel()->setSourceModel(model);
  }
}

void ProxyModelContainer::appendProxyModel(QAbstractProxyModel* model)
{
  Q_ASSERT(model != nullptr);

  if(mList.empty()){
    if(!mSourceModel.isNull()){
      model->setSourceModel(mSourceModel);
    }
  }else{
    model->setSourceModel(lastProxyModel());
  }
  mList.push_back(model);
}

void ProxyModelContainer::prependProxyModel(QAbstractProxyModel* model)
{
  Q_ASSERT(model != nullptr);

  if(!mSourceModel.isNull()){
    model->setSourceModel(mSourceModel);
  }
  if(!mList.empty()){
    firstProxyModel()->setSourceModel(model);
  }
  mList.insert(mList.cbegin(), model);
}

void ProxyModelContainer::removeProxyModelAt(int index)
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < proxyModelCount());

  mList.erase(mList.cbegin() + index);
  if(mList.empty()){
    return;
  }
  updateSourceModelForProxyModelAt(index);
}

void ProxyModelContainer::deleteProxyModelAt(int index)
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < proxyModelCount());

  delete mList[index];
  removeProxyModelAt(index);
}

void ProxyModelContainer::updateSourceModelForProxyModelAt(int index)
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < proxyModelCount());

  if(index == 0){
    updateSourceModel(firstProxyModel(), mSourceModel);
  }else if(index == (proxyModelCount()-1) ){
    updateSourceModel(lastProxyModel(), proxyModelAt(index-1));
  }else{
    updateSourceModel(proxyModelAt(index), proxyModelAt(index-1));
  }
}

void ProxyModelContainer::updateSourceModel(QAbstractProxyModel* proxyModel, QAbstractItemModel* sourceModel)
{
  if( (proxyModel == nullptr) || (sourceModel == nullptr) ){
    return;
  }
  proxyModel->setSourceModel(sourceModel);
}

}} // namespace Mdt{ namespace ItemModel{
