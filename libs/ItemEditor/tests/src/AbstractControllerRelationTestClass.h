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
#ifndef MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_RELATION_TEST_CLASS_H
#define MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_RELATION_TEST_CLASS_H

#include "Mdt/ItemEditor/AbstractControllerRelation.h"
#include <QPointer>

class QAbstractItemModel;

class AbstractControllerRelationTestClass : public Mdt::ItemEditor::AbstractControllerRelation
{
 Q_OBJECT

 public:

  explicit AbstractControllerRelationTestClass(QObject* parent = nullptr);
  Mdt::ItemEditor::AbstractController *storedOldParentController() const;
  Mdt::ItemEditor::AbstractController *storedOldChildController() const;
  Mdt::ItemEditor::AbstractController *storedParentController() const;
  Mdt::ItemEditor::AbstractController *storedChildController() const;
  QAbstractItemModel *parentControllerStoredModel() const;
  QAbstractItemModel *childControllerStoredModel() const;

 private:

  void parentControllerAboutToChangeEvent(Mdt::ItemEditor::AbstractController* oldController) override;
  void parentControllerChangedEvent(Mdt::ItemEditor::AbstractController* controller) override;
  void childControllerAboutToChangeEvent(Mdt::ItemEditor::AbstractController* oldController) override;
  void childControllerChangedEvent(Mdt::ItemEditor::AbstractController* controller) override;
  void parentControllerModelChangedEvent(QAbstractItemModel* model) override;
  void childControllerModelChangedEvent(QAbstractItemModel* model) override;

  QPointer<Mdt::ItemEditor::AbstractController> mOldParentController;
  QPointer<Mdt::ItemEditor::AbstractController> mOldChildController;
  QPointer<Mdt::ItemEditor::AbstractController> mParentController;
  QPointer<Mdt::ItemEditor::AbstractController> mChildController;
  QPointer<QAbstractItemModel> mParentModel;
  QPointer<QAbstractItemModel> mChildModel;
};

#endif // #ifndef MDT_ITEM_EDITOR_ABSTRACT_CONTROLLER_RELATION_TEST_CLASS_H
