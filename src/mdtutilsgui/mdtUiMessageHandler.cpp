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
#include "mdtUiMessageHandler.h"
#include <QMessageBox>

void mdtUiMessageHandler::setError(const mdtError& err)
{
  pvText = err.text();
  pvInformativeText = err.informativeText();
  pvDetailedText = err.systemText();
  switch(err.level()){
    case mdtError::NoError:
    case mdtError::Info:
      pvType = Information;
      break;
    case mdtError::Error:
      pvType = Error;
      break;
    case mdtError::Warning:
      pvType = Warning;
      break;
  }
}

void mdtUiMessageHandler::displayToUser()
{
  QMessageBox msgBox(pvParentWidget);
  QMessageBox::Icon icon;

  icon = QMessageBox::NoIcon;
  switch(pvType){
    case Information:
      icon = QMessageBox::Information;
      break;
    case Question:
      icon = QMessageBox::Question;
      break;
    case Warning:
      icon = QMessageBox::Warning;
      break;
    case Error:
      icon = QMessageBox::Critical;
      break;
  }
  msgBox.setText(pvText);
  msgBox.setInformativeText(pvInformativeText);
  msgBox.setDetailedText(pvDetailedText);
  msgBox.setIcon(icon);
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.exec();
}

bool mdtUiMessageHandler::promptUser()
{
  QMessageBox msgBox(pvParentWidget);
  QMessageBox::Icon icon;

  icon = QMessageBox::NoIcon;
  switch(pvType){
    case Information:
      icon = QMessageBox::Information;
      break;
    case Question:
      icon = QMessageBox::Question;
      break;
    case Warning:
      icon = QMessageBox::Warning;
      break;
    case Error:
      icon = QMessageBox::Critical;
      break;
  }
  msgBox.setText(pvText);
  msgBox.setInformativeText(pvInformativeText);
  msgBox.setDetailedText(pvDetailedText);
  msgBox.setIcon(icon);
  msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
  switch(msgBox.exec()){
    case QMessageBox::Ok:
    case QMessageBox::Open:
    case QMessageBox::Save:
    case QMessageBox::Apply:
    case QMessageBox::SaveAll:
    case QMessageBox::Yes:
    case QMessageBox::YesToAll:
      return true;
    default:
      return false;
  }
}

bool mdtUiMessageHandler::askUser()
{
  QMessageBox msgBox(pvParentWidget);
  QMessageBox::Icon icon;

  icon = QMessageBox::NoIcon;
  switch(pvType){
    case Information:
      icon = QMessageBox::Information;
      break;
    case Question:
      icon = QMessageBox::Question;
      break;
    case Warning:
      icon = QMessageBox::Warning;
      break;
    case Error:
      icon = QMessageBox::Critical;
      break;
  }
  msgBox.setText(pvText);
  msgBox.setInformativeText(pvInformativeText);
  msgBox.setDetailedText(pvDetailedText);
  msgBox.setIcon(icon);
  msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Cancel);
  switch(msgBox.exec()){
    case QMessageBox::Ok:
    case QMessageBox::Open:
    case QMessageBox::Save:
    case QMessageBox::Apply:
    case QMessageBox::SaveAll:
    case QMessageBox::Yes:
    case QMessageBox::YesToAll:
      return true;
    default:
      return false;
  }
}
