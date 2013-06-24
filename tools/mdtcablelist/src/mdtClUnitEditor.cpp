/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtClUnitEditor.h"
#include "ui_mdtClUnitEditor.h"
#include "mdtSqlWindow.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlRelation.h"
#include <QSqlTableModel>

mdtClUnitEditor::mdtClUnitEditor(const QSqlDatabase db)
{
  pvDatabase = db;
  // Setup unit
  pvUnitWidget = new mdtSqlFormWidget;
  Ui::mdtClUnitEditor *e = new Ui::mdtClUnitEditor;
  e->setupUi(pvUnitWidget);
  pvUnitModel = new QSqlTableModel;
  // Setup connection
  pvConnectionWidget = new mdtSqlTableWidget;
  pvConnectionModel = new QSqlTableModel;

  pvRelation = new mdtSqlRelation;
}

mdtClUnitEditor::~mdtClUnitEditor()
{
  ///delete pvUnitWidget;
  ///delete pvConnectionWidget;
  delete pvRelation;
  delete pvUnitModel;
  delete pvConnectionModel;
}

bool mdtClUnitEditor::setupTables(bool includeConnections)
{
  // Setup Unit table
  pvUnitModel = new QSqlTableModel;
  pvUnitModel->setTable("Unit_tbl");
  if(!pvUnitModel->select()){
    return false;
  }
  pvUnitWidget->setModel(pvUnitModel);
  pvUnitWidget->mapFormWidgets();
  // Setup connection table
  pvConnectionModel = new QSqlTableModel;
  pvConnectionModel->setTable("Connection_tbl");
  if(!pvConnectionModel->select()){
    return false;
  }
  pvConnectionWidget->setModel(pvConnectionModel);
  // Setup relation
  pvRelation->setParentModel(pvUnitModel);
  pvRelation->setChildModel(pvConnectionModel);
  if(!pvRelation->addRelation("Id_PK", "UnitId_FK")){
    return false;
  }
  pvUnitWidget->addChildWidget(pvConnectionWidget, pvRelation);

  return true;
}

void mdtClUnitEditor::setupUi(mdtSqlWindow *window)
{
  Q_ASSERT(window != 0);

  window->setSqlWidget(pvUnitWidget);
  window->addChildWidget(pvConnectionWidget, "Connections");
  window->enableNavigation();
  window->enableEdition();
  window->resize(700, 500);
}

