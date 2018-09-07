/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
 **
 ** This file is part of Mdt library.
 **
 ** Mdt is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** Mdt is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with Mdt.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "VehicleTypeClassEditionWindow.h"
#include "ui_VehicleTypeClassEditionWindow.h"
#include "Mdt/Railway/Entity/VehicleTypeClass.h"
#include "Mdt/Railway/VehicleTypeClassTableModel.h"
#include "Mdt/Railway/EditVehicleTypeClassTableModel.h"
#include "Mdt/Entity/FieldAttributesItemDelegate.h"
#include "Mdt/ItemModel/RemoveRows.h"
#include "Mdt/ErrorDialog.h"
#include <QIcon>
#include <QLatin1String>
#include <QUndoCommand>
#include <QCoreApplication>

using Mdt::Entity::AbstractReadOnlyCacheTableModel;

namespace Mdt{ namespace Railway{


  class AddVehicleTypeClassCommand : public QUndoCommand
  {
    Q_DECLARE_TR_FUNCTIONS(AddVehicleTypeClassCommand)

   public:

    AddVehicleTypeClassCommand(VehicleTypeClassTableModel *model, QUndoCommand *parent = 0)
     : QUndoCommand(parent),
       mTableModel(model)
    {
      Q_ASSERT(mTableModel != nullptr);

      setText(tr("Add vehicle type class"));
    }

    void redo() override
    {
      mCurrentRow = mTableModel->rowCount();
      mTableModel->insertRow(mCurrentRow);
    }

    void undo() override
    {
      Q_ASSERT(mCurrentRow >= 0);
      Q_ASSERT(mCurrentRow < mTableModel->rowCount());

      mTableModel->removeRow(mCurrentRow);

//       const auto lastRow = mTableModel->rowCount() - 1;
//       Q_ASSERT(lastRow >= 0);
//       mTableModel->removeRow(lastRow);
    }

   private:

    VehicleTypeClassTableModel *mTableModel;
    int mCurrentRow;
  };

VehicleTypeClassEditionWindow::VehicleTypeClassEditionWindow(QWidget* parent)
 : QMainWindow(parent),
   mUi( std::make_unique<Ui::VehicleTypeClassEditionWindow>() ),
   mTableModel( new VehicleTypeClassTableModel(this) ),
   mEditVehicleTypeClassTableModel( new EditVehicleTypeClassTableModel(this) ),
   mEditVehicleTypeClassTableModelAsync( new AbstractReadOnlyCacheTableModel(this) ),
   mVehicleTypeClass( new VehicleTypeClass(this) ),
   mUndoStack( new QUndoStack(this) )
{
  mUi->setupUi(this);

  mUi->tableView->setModel( mTableModel );
  using Delegate = Mdt::Entity::FieldAttributesItemDelegate<Entity::VehicleTypeClassEntity>;
  mUi->tableView->setItemDelegate( new Delegate(this) );

  mEditVehicleTypeClassTableModel->setCache(&mEditVehicleTypeClassCache);
  mUi->tvEditVehicleTypeClass->setModel(mEditVehicleTypeClassTableModel);

  mEditVehicleTypeClassTableModelAsync->setCache(&mEditVehicleTypeClassCacheAsync);
  mUi->tvEditVehicleTypeClassAsync->setModel(mEditVehicleTypeClassTableModelAsync);

  mUi->undoView->setStack(mUndoStack);

  connect( mUi->pbAdd, &QPushButton::clicked, this, &VehicleTypeClassEditionWindow::addVehicleTypeClass );
  connect( mUi->pbRemove, &QPushButton::clicked, this, &VehicleTypeClassEditionWindow::removeSelectedVehicleTypeClasses );
  connect( mUi->pbUnremove, &QPushButton::clicked, this, &VehicleTypeClassEditionWindow::unremoveSelectedVehicleTypeClasses );
  connect( mUi->actionSave, &QAction::triggered, this, &VehicleTypeClassEditionWindow::save );
//   connect( mUi->actionRevert, &QAction::triggered, this, &VehicleTypeClassEditionWindow::revert );

  createUndoActions();
}

VehicleTypeClassEditionWindow::~VehicleTypeClassEditionWindow()
{
}

void VehicleTypeClassEditionWindow::setVehicleTypeClassRepository(const VehicleTypeClassRepositoryHandle& repository)
{
  mVehicleTypeClass->setRepository(repository);
  mTableModel->setRepository(repository);
}

void VehicleTypeClassEditionWindow::setVehicleTypeClassRepository(const std::shared_ptr<VehicleTypeClassRepository> & repository)
{
  mEditVehicleTypeClassCache.setVehicleTypeClassRepository(repository);
//   mEditVehicleTypeClassTableModel->setVehicleTypeClassRepository(repository);
}

void VehicleTypeClassEditionWindow::setSelectQueryFactory(const std::shared_ptr<QueryExpression::AbstractSelectQueryFactory> & factory)
{
  Q_ASSERT(factory.get() != nullptr);
  Q_ASSERT(factory->isValid());

  mEditVehicleTypeClassCache.setSelectQueryFactory(factory);
  if(!mEditVehicleTypeClassCache.fetchAll()){
    displayError(mEditVehicleTypeClassCache.lastError());
  }
//   mEditVehicleTypeClassTableModel->setSelectQueryFactory(factory);
//   if(!mEditVehicleTypeClassTableModel->fetchAll()){
//     displayError(mEditVehicleTypeClassTableModel->lastError());
//   }
}

void VehicleTypeClassEditionWindow::setAsyncSelectQueryFactory(const std::shared_ptr< QueryExpression::AbstractAsyncSelectQueryFactory >& factory)
{
  Q_ASSERT(factory.get() != nullptr);
//   Q_ASSERT(factory->isValid());

  mEditVehicleTypeClassCacheAsync.setQueryFactory(factory);
  mEditVehicleTypeClassCacheAsync.fetchAll();
}

void VehicleTypeClassEditionWindow::addVehicleTypeClass()
{
  mTableModel->insertRow(mTableModel->rowCount());
  mEditVehicleTypeClassCache.appendRow();

//   auto *command = new AddVehicleTypeClassCommand(mTableModel);
//   mUndoStack->push(command);
}

void VehicleTypeClassEditionWindow::removeSelectedVehicleTypeClasses()
{
  Mdt::ItemModel::removeRows(mTableModel, mUi->tableView->selectionModel());
  Mdt::ItemModel::removeRows(mEditVehicleTypeClassTableModel, mUi->tvEditVehicleTypeClass->selectionModel());
}

void VehicleTypeClassEditionWindow::unremoveSelectedVehicleTypeClasses()
{
  Mdt::ItemModel::cancelRemoveRows(mTableModel, mUi->tableView->selectionModel());
}

void VehicleTypeClassEditionWindow::save()
{
//   if(!mVehicleTypeClass->submitChanges()){
//     displayError(mVehicleTypeClass->lastError());
//   }
  if(!mEditVehicleTypeClassCache.submitChanges()){
    displayError(mEditVehicleTypeClassCache.lastError());
  }
}

void VehicleTypeClassEditionWindow::displayError(const Error& error)
{
  Mdt::ErrorDialog dialog(error);
  dialog.exec();
}

void VehicleTypeClassEditionWindow::createUndoActions()
{
  mUndoAction = mUndoStack->createUndoAction(this, tr("&Undo"));
  mUndoAction->setIcon(QIcon::fromTheme(QLatin1String("edit-undo")));
  mUndoAction->setShortcuts(QKeySequence::Undo);

  mUi->menu_File->addAction(mUndoAction);
  mUi->toolBar->addAction(mUndoAction);

  mRedoAction = mUndoStack->createRedoAction(this, tr("&Redo"));
  mRedoAction->setIcon(QIcon::fromTheme(QLatin1String("edit-redo")));
  mRedoAction->setShortcuts(QKeySequence::Redo);

  mUi->menu_File->addAction(mRedoAction);
  mUi->toolBar->addAction(mRedoAction);
}

}} // namespace Mdt{ namespace Railway{
