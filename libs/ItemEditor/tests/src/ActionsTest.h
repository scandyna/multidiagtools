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
#ifndef MDT_ITEM_EDITOR_ACTIONS_TEST_H
#define MDT_ITEM_EDITOR_ACTIONS_TEST_H

#include "Mdt/ItemEditor/AbstractActionContainer.h"
#include <QObject>
#include <QtTest/QtTest>

/*
 * Test class for AbstractActionContainer
 */
class ActionContainerTester : public Mdt::ItemEditor::AbstractActionContainer
{
 Q_OBJECT

 public:

  explicit ActionContainerTester(QObject* parent = nullptr);
  int stateChangedCount()
  {
    return mStateChangedCount;
  }
  void resetStateChangedCount()
  {
    mStateChangedCount = 0;
  }
  bool disableForced() const
  {
    return mDisableForced;
  }
//   void resetDisableForced()
//   {
//     mDisableForced = false;
//   }

 private:

  void updateEnableState() override
  {
    ++mStateChangedCount;
    mDisableForced = false;
  }
  void disableAllActions() override
  {
    mDisableForced = true;
  }

  int mStateChangedCount = 0;
  bool mDisableForced = false;
};

class ActionsTest : public QObject
{
  Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void abstractActionContainerTest();
  void navigationActionsTest();
  void editionActionsTest();
  void insertActionTest();
  void removeActionTest();
  void resizeToContentsTest();
};

#endif // #ifndef MDT_ITEM_EDITOR_ACTIONS_TEST_H
