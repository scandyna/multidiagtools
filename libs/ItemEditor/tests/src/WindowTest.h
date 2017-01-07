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
#ifndef MDT_ITEM_EDITOR_WINDOW_TEST_H
#define MDT_ITEM_EDITOR_WINDOW_TEST_H

#include "Mdt/ItemEditor/AbstractWindow.h"
#include "Mdt/ItemEditor/NavigationActions.h"
#include "Mdt/ItemEditor/EditionActions.h"
#include "Mdt/ItemEditor/InsertAction.h"
#include "Mdt/ItemEditor/RemoveAction.h"
#include <QObject>
#include <QtTest/QtTest>

using namespace Mdt::ItemEditor;

class AbstractWindowTester : public AbstractWindow
{
 Q_OBJECT

 public:

  AbstractWindowTester(QWidget* parent = nullptr);
  NavigationActions *navigationActions();
  EditionActions *editionActions();
  InsertAction *insertAction();
  RemoveAction *removeAction();
};

class WindowTest : public QObject
{
  Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void initActionsTest();

  void setupControllerBeforeSetTest();
  void setupControllerAfterSetTest();

  void standardWindowTest();
};


#endif // #ifndef MDT_ITEM_EDITOR_WINDOW_TEST_H
