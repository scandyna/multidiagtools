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

#include "Mdt/ItemEditor/AbstractItemModelController.h"

class ItemModelControllerTester : public Mdt::ItemEditor::AbstractItemModelController
{
 Q_OBJECT

 public:

  explicit ItemModelControllerTester(QObject* parent = nullptr);

  int primaryKeyChangedEventCount() const;
  void clearPrimaryKeyChangedEventCount();

 private:

  void setModelToView(QAbstractItemModel *model) override;
  bool submitDataToModel() override;
  void revertDataFromModel() override;
  void primaryKeyChangedEvent(const Mdt::ItemModel::PrimaryKey& oldPrimaryKey, const Mdt::ItemModel::PrimaryKey& newPrimaryKey) override;

  int mPrimaryKeyChangedEventCount = 0;
};

#endif // #ifndef MDT_ITEM_EDITOR_ITEM_MODEL_CONTROLLER_TESTER_H
