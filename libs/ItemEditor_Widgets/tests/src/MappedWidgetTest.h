/****************************************************************************
 **
 ** Copyright (C) 2011-2018 Philippe Steinmann.
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
#ifndef MDT_TEST_MAIN
#define MDT_TEST_MAIN

#include "TestBase.h"
#include "Mdt/ItemEditor/MappedWidget.h"
#include <memory>

class EditionStartedSignalSpy : public QObject
{
 Q_OBJECT

 public:

  EditionStartedSignalSpy(Mdt::ItemEditor::MappedWidget *const mappedWidget);

  int editionStartedCount() const;
  void clear();

 public slots:

  void incrementEditionStartedCount();

 private:

  int mEditionStartedCount = 0;
  Mdt::ItemEditor::MappedWidget *mMappedWidget;
};


class MappedWidgetTest : public QObject
{
  Q_OBJECT

 private slots:

  void constructDeleteTest();

  void widgetValueQLineEditTest();
  void widgetValueQLineEditGenericTest();
  void widgetValueQComboBoxTest();
  void widgetValueRelationalQComboBoxTest();
  void widgetValueQSpinBoxTest();
  void widgetValueQDoubleSpinBoxTest();
  void widgetValueQPlainTextEditTest();
  void widgetValueQTextEditTest();

  void widgetEditablePropertyTest();
  void widgetEditableUnhandledWidgetTest();
  void widgetEditableQLineEditGenericTest();
  void widgetEditableQLineEditTest();
//   void widgetEditableQComboBoxTest();
  void widgetEditableReadOnlyQComboBoxTest();
  void widgetEditableEditableQComboBoxTest();
  void widgetEditableQSpinBoxTest();
  void widgetEditableQDoubleSpinBoxTest();
  void widgetEditableQPlainTextEditTest();
  void widgetEditableQTextEditTest();

  void editionStartedQLineEditTest();
  void editionStartedQLineEditGenericTest();
  void editionStartedReadOnlyQComboBoxTest();
  void editionStartedEditableQComboBoxTest();
  void editionStartedQSpinBoxTest();
  void editionStartedQDoubleSpinBoxTest();
  void editionStartedQPlainTextEditTest();
  void editionStartedQTextEditTest();

  void readOnlyComboBoxPopulateThenSetModelTest();
  void editableComboBoxPopulateThenSetModelTest();
  void readOnlyComboBoxSetModelThenPopulateTest();
  void editableComboBoxSetModelThenPopulateTest();

  void listAddGetRemoveTest();
  void listAddMappingKnownEditorWidgetsTest();
};

#endif // #ifndef MDT_TEST_MAIN
