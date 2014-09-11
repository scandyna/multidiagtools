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
#include "mdtSqlFormWidgetDataValidator.h"
#include "mdtSqlFieldHandler.h"
#include <QWidget>

mdtSqlFormWidgetDataValidator::mdtSqlFormWidgetDataValidator(QObject *parent, const QList<mdtSqlFieldHandler*> &fieldHandlers)
 : mdtSqlDataValidator(parent)
{
  pvFieldHandlers = fieldHandlers;
}

mdtSqlFormWidgetDataValidator::~mdtSqlFormWidgetDataValidator()
{
}

bool mdtSqlFormWidgetDataValidator::checkBeforeSubmit()
{
  int i;
  bool allOk = true;
  QWidget *firstNokWidget = 0;

  // We call checkBeforeSubmit() for all items
  for(i = 0; i < pvFieldHandlers.size(); ++i){
    Q_ASSERT(pvFieldHandlers.at(i) != 0);
    if(!pvFieldHandlers.at(i)->checkBeforeSubmit()){
      allOk = false;
      if(firstNokWidget == 0){
        firstNokWidget = pvFieldHandlers.at(i)->dataWidget();
      }
    }
  }
  // We set the focus on first Nok widget
  if(firstNokWidget != 0){
    firstNokWidget->setFocus();
  }
  // Display a warning to the user
  if(!allOk){
    if(messageHandler()){
      messageHandler()->setText(tr("There are some errors in edited data."));
      messageHandler()->setInformativeText(tr("Fields that are not correct should be highlighted.\nMoving cursor over field with error should display the reason\nPlease correct errors, or cancel modifications, and try again."));
      messageHandler()->setType(mdtUiMessageHandler::Error);
      messageHandler()->displayToUser();
    }
  }

  return allOk;
}

