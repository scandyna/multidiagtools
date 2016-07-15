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
#include "AbstractItemViewController.h"
#include "EventCatchItemDelegate.h"
#include "ItemSelectionModel.h"
#include <QAbstractItemModel>
#include <QAbstractItemView>

// #include <QDebug>

namespace Mdt{ namespace ItemEditor{

AbstractItemViewController::AbstractItemViewController(QObject* parent)
 : AbstractController(parent)
{
  connect(this, &AbstractItemViewController::modelChanged, this, &AbstractItemViewController::setModelToView);
}

void AbstractItemViewController::setView(QAbstractItemView* view)
{
  Q_ASSERT(view != nullptr);

  pvView = view;
  setModelToView(model());
  // Replace delegate with our proxy delegate
  auto *delegate = pvView->itemDelegate();
  auto *proxyDelegate = new EventCatchItemDelegate(view);
  if(delegate != nullptr){
    proxyDelegate->setItemDelegate(delegate);
  }
  pvView->setItemDelegate(proxyDelegate);
}

QAbstractItemView* AbstractItemViewController::view() const
{
  return pvView;
}

void AbstractItemViewController::setModel(QAbstractItemModel* model)
{
  Q_ASSERT(model != nullptr);

  /*
   * Order of signal/slot connections matters here.
   * We must be sure that model is set to the view
   * before it is registered (set to RowChangeEventMapper).
   * Note doing so will produces a problem when model resets:
   *  - Controller receives the event and updates current row to 0 (if model contains data)
   *  - Controller updates current index of view
   *  - View will reset (and current will also be lost!)
   */
  referenceItemModel(model);
}

void AbstractItemViewController::setModelToView(QAbstractItemModel* model)
{
  if( (model != nullptr) && (!pvView.isNull())){
    /*
     * As described in documentation of QAbstractItemView,
     * the selection model will be replaced.
     * QAbstractItemView does not delete the old selection model.
     * While we are owning our view, we can safely delete it here.
     */
    auto *oldSelectionModel = pvView->selectionModel();
    pvView->setModel(model);
    if(oldSelectionModel != nullptr){
      delete oldSelectionModel;
    }
    // Replace selection model
    pvView->setSelectionModel(new ItemSelectionModel(model));
    registerItemModel();
    registerSelectionModel(pvView->selectionModel());
  }
}

}} // namespace Mdt{ namespace ItemEditor{
