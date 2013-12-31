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
#include "mdtClConnectorEditor.h"
#include "ui_mdtClConnectorEditor.h"
///#include "mdtSqlFormOld.h"
#include "mdtSqlFormWindow.h"
#include "mdtSqlFormWidget.h"
#include "mdtSqlTableWidget.h"
#include "mdtSqlWindowOld.h"

mdtClConnectorEditor::mdtClConnectorEditor(QObject *parent, QSqlDatabase db)
 : mdtClEditor(parent, db)
{
}

mdtClConnectorEditor::~mdtClConnectorEditor() 
{
}

bool mdtClConnectorEditor::setupTables()
{
  if(!setupConnectorTable()){
    return false;
  }
  if(!setupConnectorContactTable()){
    return false;
  }

  return true;
}

bool mdtClConnectorEditor::setupConnectorTable()
{
  Q_ASSERT(form() != 0);

  Ui::mdtClConnectorEditor ce;

  // Setup main form widget
  ce.setupUi(form()->mainSqlWidget());
  ///connect(this, SIGNAL(unitEdited()), form()->mainSqlWidget(), SIGNAL(dataEdited()));
  // Setup form
  if(!form()->setTable("Connector_tbl", "Connector", database())){
    return false;
  }
  if(sqlWindow() != 0){
    sqlWindow()->enableNavigation();
    sqlWindow()->enableEdition();
    sqlWindow()->resize(800, 500);
    sqlWindow()->setWindowTitle(tr("Connector edition"));
  }

  return true;
}

bool mdtClConnectorEditor::setupConnectorContactTable()
{
  Q_ASSERT(form() != 0);

  mdtSqlTableWidget *widget;

  if(!form()->addChildTable("ConnectorContact_tbl", tr("Contacts"), database())){
    return false;
  }
  if(!form()->addRelation("Id_PK", "ConnectorContact_tbl", "Connector_Id_FK")){
    return false;
  }
  widget = form()->sqlTableWidget("ConnectorContact_tbl");
  Q_ASSERT(widget != 0);
  // Enable edition
  widget->enableLocalEdition();
  // Hide technical fields
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("Connector_Id_FK", true);
  ///widget->setColumnHidden("", true);
  // Set fields a user friendly name
  ///widget->setHeaderData("", tr(""));

  return true;
}
