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
#include "mdtSqlFormOld.h"
#include "mdtSqlFormWindow.h"
#include "mdtSqlWindowOld.h"
#include "mdtSqlFormDialog.h"
#include "mdtSqlDialogOld.h"
#include <QMessageBox>

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

bool mdtClEditor::select(const QString & tableName)
{
  if(pvForm == 0){
    pvLastError.setError(tr("Cannot call select because form was not set."), mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtClEditor");
    pvLastError.commit();
    return false;
  }
  if(!pvForm->select(tableName)){
    pvLastError = pvForm->lastError();
    return false;
  }

  return true;
}

mdtError mdtClEditor::lastError() const
{
  return pvLastError;
}

mdtSqlFormOld *mdtClEditor::form()
{
  return pvForm;
}

mdtSqlWindowOld *mdtClEditor::sqlWindow()
{
  if(pvFormWindow == 0){
    return 0;
  }
  return pvFormWindow->sqlWindow();
}

void mdtClEditor::displayLastError()
{
  QMessageBox msgBox;

  msgBox.setText(pvLastError.text());
  msgBox.setDetailedText(pvLastError.systemText());
  msgBox.setIcon(pvLastError.levelIcon());
  msgBox.exec();
}
