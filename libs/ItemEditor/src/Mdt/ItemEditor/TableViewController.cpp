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
#include "TableViewController.h"
#include "TableViewControllerImplementation.h"
#include "TableViewControllerStateTable.h"
#include "TableViewControllerStatePermission.h"
#include "ControllerStateMachine.h"
#include "Mdt/ItemModel/PrimaryKey.h"
#include <QTableView>

// #include "Debug.h"

using namespace Mdt::ItemModel;

namespace Mdt{ namespace ItemEditor{

TableViewController::TableViewController(QObject* parent)
 : AbstractController(parent),
   mImpl( std::make_unique<TableViewControllerImplementation>() )
{
  setControllerStateMachine( ControllerStateMachine::makeNew<TableViewControllerStateTable, TableViewControllerStatePermission>(this) );
  connect(mImpl->proxyItemDelegate(), &EventCatchItemDelegate::dataEditionStarted, this, &TableViewController::onDataEditionStarted);
  connect(mImpl->proxyItemDelegate(), &EventCatchItemDelegate::dataEditionDone, this, &TableViewController::onDataEditionDone);
//   connect(this, &TableViewController::primaryKeyChanged, this, &TableViewController::onPrimaryKeyChanged);
}

TableViewController::~TableViewController()
{
}

void TableViewController::setView(QTableView* view)
{
  Q_ASSERT(view != nullptr);

  mImpl->setView(view);
  if( mImpl->connectToController(this) ){
    modelSetToView();
  }
}

QTableView* TableViewController::view() const
{
  return mImpl->view();
}

void TableViewController::setPrimaryKeyHidden(bool hide)
{
  mImpl->setPrimaryKeyHidden( getPrimaryKey(), hide );
}

void TableViewController::setForeignKeyHidden(bool hide)
{
  mImpl->setForeignKeyHidden( getForeignKey(), hide );
}

void TableViewController::setModelToView(QAbstractItemModel* model)
{
  mImpl->setModel(model);
  if( mImpl->connectToController(this) ){
    modelSetToView();
  }
}

bool TableViewController::setDataToModel()
{
  return mImpl->setDataToModel();
}

void TableViewController::revertDataFromModel()
{
  mImpl->revertDataFromModel();
}

void TableViewController::primaryKeyChangedEvent(const PrimaryKey & oldPrimaryKey, const PrimaryKey & newPrimaryKey)
{
  mImpl->primaryKeyChangedEvent(oldPrimaryKey, newPrimaryKey);
}

void TableViewController::foreignKeyChangedEvent(const ForeignKey& oldForeignKey, const ForeignKey& newForeignKey)
{
  mImpl->foreignKeyChangedEvent(oldForeignKey, newForeignKey);
}

}} // namespace Mdt{ namespace ItemEditor{
