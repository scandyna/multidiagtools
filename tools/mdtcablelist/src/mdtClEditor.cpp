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
#include "mdtClEditor.h"
#include "mdtSqlForm.h"
#include "mdtSqlFormWindow.h"
#include "mdtSqlWindow.h"
#include "mdtSqlFormDialog.h"
#include "mdtSqlDialog.h"

mdtClEditor::mdtClEditor(QObject *parent, QSqlDatabase db)
 : QObject(parent)
{
  pvDatabase = db;
  pvForm = 0;
  pvFormWindow = 0;
  pvFormDialog = 0;
}

mdtClEditor::~mdtClEditor()
{
  ///delete pvForm;
}

bool mdtClEditor::setupAsWindow() 
{
  Q_ASSERT(pvForm == 0);

  pvFormWindow = new mdtSqlFormWindow(this);
  pvForm = pvFormWindow;

  return setupTables();
}

bool mdtClEditor::setupAsDialog(QWidget *parent) 
{
  Q_ASSERT(pvForm == 0);

  pvFormDialog = new mdtSqlFormDialog;
  pvForm = pvFormDialog;
  pvFormDialog->sqlDialog()->setParent(parent);

  return setupTables();
}

QSqlDatabase &mdtClEditor::database()
{
  return pvDatabase;
}

void mdtClEditor::show() 
{
  Q_ASSERT(pvForm != 0);

  if(pvFormWindow != 0){
    pvFormWindow->sqlWindow()->show();
  }else{
    Q_ASSERT(pvFormDialog != 0);
    pvFormDialog->sqlDialog()->show();
  }
}

int mdtClEditor::exec() 
{
  Q_ASSERT(pvFormDialog != 0);

  return pvFormDialog->exec();
}

mdtSqlForm *mdtClEditor::form()
{
  return pvForm;
}

mdtSqlWindow *mdtClEditor::sqlWindow()
{
  if(pvFormWindow == 0){
    return 0;
  }
  return pvFormWindow->sqlWindow();
}
