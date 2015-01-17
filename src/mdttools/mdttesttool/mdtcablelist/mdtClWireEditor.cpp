/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
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
#include "mdtClWireEditor.h"
#include "ui_mdtClWireEditor.h"
#include "mdtSqlTableWidget.h"
#include "mdtAbstractSqlTableController.h"
#include "mdtSqlDataWidgetController.h"
#include "mdtSqlRelationInfo.h"
#include "mdtSqlSelectionDialog.h"
#include "mdtSqlTableSelection.h"
#include "mdtError.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QPushButton>
#include <QStringList>
#include <QString>
#include <QList>
#include <memory>

mdtClWireEditor::mdtClWireEditor(QWidget* parent, QSqlDatabase db)
 : mdtSqlForm(parent, db)
{
}

bool mdtClWireEditor::setupTables()
{
  return setupWireTable();
}

bool mdtClWireEditor::setupWireTable()
{
  // Setup main form widget
  setMainTableUi<Ui::mdtClWireEditor>();
  // Setup form
  if(!setMainTable("Wire_tbl", "Wires", database())){
    return false;
  }
  return true;
}
