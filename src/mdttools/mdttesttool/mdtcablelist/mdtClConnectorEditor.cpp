/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
///#include "mdtSqlFormWidget.h"
#include "mdtSqlTableWidget.h"

mdtClConnectorEditor::mdtClConnectorEditor(QWidget *parent, QSqlDatabase db)
 : mdtSqlForm(parent, db)
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
  if(!setupUnitUsageTable()){
    return false;
  }

  return true;
}

bool mdtClConnectorEditor::setupConnectorTable()
{
  ///Ui::mdtClConnectorEditor ce;

  // Setup main form widget
  ///ce.setupUi(mainSqlWidget());
  setMainTableUi<Ui::mdtClConnectorEditor>();
  ///connect(this, SIGNAL(unitEdited()), form()->mainSqlWidget(), SIGNAL(dataEdited()));
  // Setup form
  if(!setMainTable("Connector_tbl", "Connector", database())){
    return false;
  }
  setWindowTitle(tr("Connector edition"));

  return true;
}

bool mdtClConnectorEditor::setupConnectorContactTable()
{
  mdtSqlTableWidget *widget;
  mdtSqlRelationInfo relationInfo;

  relationInfo.setChildTableName("ConnectorContact_tbl");
  relationInfo.addRelation("Id_PK", "Connector_Id_FK", true);
  if(!addChildTable(relationInfo, tr("Contacts"))){
    return false;
  }
  /**
  if(!addChildTable("ConnectorContact_tbl", tr("Contacts"), database())){
    return false;
  }
  if(!addRelation("Id_PK", "ConnectorContact_tbl", "Connector_Id_FK")){
    return false;
  }
  */
  widget = sqlTableWidget("ConnectorContact_tbl");
  Q_ASSERT(widget != 0);
  // Enable edition
  widget->enableLocalEdition();
  widget->addStretchToLocalBar();
  // Hide technical fields
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("Connector_Id_FK", true);
  // Set fields a user friendly name
  ///widget->setHeaderData("", tr(""));

  return true;
}

bool mdtClConnectorEditor::setupUnitUsageTable()
{
  mdtSqlTableWidget *widget;
  mdtSqlRelationInfo relationInfo;

  relationInfo.setChildTableName("UnitConnectorUsage_view");
  relationInfo.addRelation("Id_PK", "Connector_Id_FK", false);
  if(!addChildTable(relationInfo, tr("Usage (in units)"))){
    return false;
  }
  /**
  if(!addChildTable("UnitConnectorUsage_view", tr("Used by units"), database())){
    return false;
  }
  if(!addRelation("Id_PK", "UnitConnectorUsage_view", "Connector_Id_FK")){
    return false;
  }
  */
  widget = sqlTableWidget("UnitConnectorUsage_view");
  Q_ASSERT(widget != 0);
  // Hide technical fields
  widget->setColumnHidden("Id_PK", true);
  widget->setColumnHidden("Connector_Id_FK", true);
  widget->setColumnHidden("Unit_Id_FK", true);
  widget->setColumnHidden("ArticleConnector_Id_FK", true);
  // Set fields a user friendly name
  widget->setHeaderData("UnitConnectorName", tr("Unit\nConnector name"));
  widget->setHeaderData("SchemaPosition", tr("Schema\npos."));
  widget->setHeaderData("Type", tr("Vehicle\ntype"));
  widget->setHeaderData("SubType", tr("Vehicle\nsub type"));
  widget->setHeaderData("SeriesNumber", tr("Vehicle\nserie"));
  // Setup sorting
  widget->addColumnToSortOrder("Type", Qt::AscendingOrder);
  widget->addColumnToSortOrder("SubType", Qt::AscendingOrder);
  widget->addColumnToSortOrder("SeriesNumber", Qt::AscendingOrder);
  widget->addColumnToSortOrder("SchemaPosition", Qt::AscendingOrder);
  widget->addColumnToSortOrder("UnitConnectorName", Qt::AscendingOrder);
  widget->sort();
  // Other stuff
  widget->resizeViewToContents();

  return true;
}
