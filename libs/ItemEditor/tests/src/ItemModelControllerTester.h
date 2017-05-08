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
#ifndef MDT_ITEM_EDITOR_ITEM_MODEL_CONTROLLER_TESTER_H
#define MDT_ITEM_EDITOR_ITEM_MODEL_CONTROLLER_TESTER_H

#include "Mdt/ItemEditor/AbstractController.h"
#include "Mdt/ItemEditor/ControllerStateMachine.h"
#include <QVariant>

class ItemModelControllerTester : public Mdt::ItemEditor::AbstractController
{
 Q_OBJECT

 public:

  explicit ItemModelControllerTester(QObject* parent = nullptr, Mdt::ItemEditor::ControllerStateMachine *stateMachine = nullptr);
  explicit ItemModelControllerTester(Mdt::ItemEditor::ControllerStateMachine *stateMachine);

  int primaryKeyChangedEventCount() const;
  void clearPrimaryKeyChangedEventCount();
  int foreignKeyChangedEventCount() const;
  void clearForeignKeyChangedEventCount();

  void startEditing();
  void stopEditing();

  int dataSetToModelCount() const;
  void clearDataSetToModelCount();
  int dataRevertFromModelCount() const;
  void clearDataRevertFromModelCount();

  void setSelectedRows(const Mdt::ItemModel::RowList & rowList);
  void clearSelectedRows();
  /*
   * Returns list of rows set with setSelectedRows() if not empty,
   * otherwise the implementation of AbstractController
   */
  Mdt::ItemModel::RowList getSelectedRows() const override;

  /// \todo Wrong: must simulate a editor with a cache (use, f.ex., a QMap<column,data>) - then, set to model (or revert) in submitDataToModel() and revertDataFromModel()
//   bool setModelData(int column, const QVariant & data);

 private:

  void setModelToView(QAbstractItemModel *model) override;
  bool setDataToModel() override;
  void revertDataFromModel() override;
  void primaryKeyChangedEvent(const Mdt::ItemModel::PrimaryKey& oldPrimaryKey, const Mdt::ItemModel::PrimaryKey& newPrimaryKey) override;
  void foreignKeysChangedEvent(const Mdt::ItemModel::ForeignKeyList& newForeignKeys) override;
//   void columnsPartOfForeignKeyChangedEvent(const Mdt::ItemModel::ColumnList& oldColumnList, const Mdt::ItemModel::ColumnList& newColumnList) override;
//   void foreignKeyChangedEvent(const Mdt::ItemModel::ForeignKey& oldForeignKey, const Mdt::ItemModel::ForeignKey& newForeignKey) override;

  int mPrimaryKeyChangedEventCount = 0;
  int mForeignKeyChangedEventCount = 0;
  int mDataSetToModelCount = 0;
  int mDataRevertFromModelCount = 0;

  Mdt::ItemModel::RowList mSelectedRows;
};

#endif // #ifndef MDT_ITEM_EDITOR_ITEM_MODEL_CONTROLLER_TESTER_H
