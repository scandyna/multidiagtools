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
#include "mdtTtTestSystemEditor.h"
#include "ui_mdtTtTestSystemEditor.h"

mdtTtTestSystemEditor::mdtTtTestSystemEditor(QWidget* parent, QSqlDatabase db)
 : mdtSqlForm(parent, db)
{
}

bool mdtTtTestSystemEditor::setupTables()
{
  if(!setupTestSystemTable()){
    return false;
  }
  return true;
}

bool mdtTtTestSystemEditor::setupTestSystemTable()
{
  setMainTableUi<Ui::mdtTtTestSystemEditor>();
  if(!setMainTable("TestSystem_tbl", "Test system", database())){
    return false;
  }
  setWindowTitle(tr("Test system edition"));

  return true;
}
