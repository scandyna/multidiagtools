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
#include "mdtSqlDataValidator.h"
#include "mdtError.h"
#include <QMessageBox>
#include <QSqlTableModel>

mdtSqlDataValidator::mdtSqlDataValidator(QSqlTableModel *model, QObject *parent)
 : QObject(parent)
{
  Q_ASSERT(model != 0);

  pvModel = model;
}

mdtSqlDataValidator::~mdtSqlDataValidator()
{
}

QSqlTableModel *mdtSqlDataValidator::model()
{
  Q_ASSERT(pvModel != 0);

  return pvModel;
}

bool mdtSqlDataValidator::checkBeforeSubmit()
{
  return false;
}

void mdtSqlDataValidator::displayWarning(const QString &text, const QString &informativeText, const QSqlError &sqlError)
{
  QMessageBox msgBox;

  // Log warning
  mdtError e(MDT_DATABASE_ERROR, text + "\nInfo: " + informativeText, mdtError::Warning);
  if(sqlError.isValid()){
    e.setSystemError(sqlError.number(), sqlError.text());
  }
  MDT_ERROR_SET_SRC(e, "mdtSqlDataValidator");
  e.commit();
  // Show warning
  msgBox.setText(text);
  msgBox.setInformativeText(informativeText);
  if(sqlError.isValid()){
    msgBox.setDetailedText(sqlError.text());
  }
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.exec();
}

void mdtSqlDataValidator::displayError(const QString &text, const QString &informativeText, const QSqlError &sqlError)
{
  QMessageBox msgBox;

  // Log warning
  mdtError e(MDT_DATABASE_ERROR, text + "\nInfo: " + informativeText, mdtError::Error);
  if(sqlError.isValid()){
    e.setSystemError(sqlError.number(), sqlError.text());
  }
  MDT_ERROR_SET_SRC(e, "mdtSqlDataValidator");
  e.commit();
  // Show warning
  msgBox.setText(text);
  msgBox.setInformativeText(informativeText);
  if(sqlError.isValid()){
    msgBox.setDetailedText(sqlError.text());
  }
  msgBox.setIcon(QMessageBox::Critical);
  msgBox.exec();
}

