/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#include "mdtSqlTableViewController.h"
#include "mdtSortFilterProxyModel.h"
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QItemSelectionModel>
#include <QModelIndex>

#include <QDebug>

mdtSqlTableViewController::mdtSqlTableViewController(QObject* parent)
 : mdtAbstractSqlTableController(parent)
{
  pvTableView = 0;
}

mdtSqlTableViewController::~mdtSqlTableViewController()
{
}

void mdtSqlTableViewController::setTableView(QTableView* tv)
{
  Q_ASSERT(tv != 0);

  pvTableView = tv;
  connect(pvTableView, SIGNAL(destroyed(QObject*)), this, SLOT(onTableViewDestroyed(QObject*)));
  pvTableView->setModel(proxyModel().get());
}

int mdtSqlTableViewController::currentRow() const
{
  QModelIndex index;

  if(pvTableView == 0){
    return -1;
  }
  Q_ASSERT(pvTableView->selectionModel() != 0);
  if(!model()){
    return -1;
  }
  index = pvTableView->currentIndex();
  if(index.isValid()){
    return index.row();
  }else{
    return -1;
  }
}

void mdtSqlTableViewController::onTableViewDestroyed(QObject* obj)
{
  qDebug() << "onTableViewDestroyed() - obj: " << obj << " - pvTableView: " << pvTableView;
  pvTableView = 0;
}

void mdtSqlTableViewController::currentRowChangedEvent(int row)
{

}

void mdtSqlTableViewController::modelSetEvent()
{
  Q_ASSERT(model());
  Q_ASSERT(pvTableView != 0);

  model()->setEditStrategy(QSqlTableModel::OnManualSubmit);
  ///pvTableView->setModel(model().get());
}

bool mdtSqlTableViewController::doSubmit()
{

}

bool mdtSqlTableViewController::doRevert()
{

}

bool mdtSqlTableViewController::doInsert()
{

}

bool mdtSqlTableViewController::doSubmitNewRow()
{

}

bool mdtSqlTableViewController::doRevertNewRow()
{

}

bool mdtSqlTableViewController::doRemove()
{

}
